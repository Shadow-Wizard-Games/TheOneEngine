#include "Model.h"
#include "Defs.h"

#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <filesystem>
#include <fstream>
#include "Resources.h"

#include "ozz/animation/runtime/animation.h"
#include "ozz/animation/runtime/blending_job.h"
#include "ozz/animation/runtime/local_to_model_job.h"
#include "ozz/animation/runtime/sampling_job.h"
#include "ozz/animation/runtime/skeleton.h"
#include "ozz/animation/runtime/skeleton_utils.h"
#include "ozz/base/containers/vector.h"
#include "ozz/base/log.h"
#include "ozz/base/maths/simd_math.h"
#include "ozz/base/maths/soa_transform.h"
#include "ozz/base/maths/vec_float.h"
#include "ozz/options/options.h"

#include "Animation/animations/OzzAnimationSimple.h"

Model::Model(const std::string& filename) :
m_ActiveAnimationId(INVALID_INDEX),
m_PrevAnimationId(INVALID_INDEX),
m_LoadedMesh(false),
m_LoadedSkeleton(false),
m_BlendOnTransition(true),
m_BlendThreshold(ozz::animation::BlendingJob().threshold),
m_TransitionTime(1.0f),
m_TransitionTimer(0.0f),
m_UpdateState(US_UPDATE)
{
    // This constructor only loads .mesh
    if (filename.ends_with(".mesh"))
    {
        path = filename;
        deserializeMeshData(filename);
        GenBufferData();
    }
}

std::vector<Model*> Model::LoadMeshes(const std::string& path)
{
    std::vector<Model*> meshes;

    const aiScene* scene = aiImportFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_ForceGenNormals);

    std::string fileName = scene->GetShortFilename(path.c_str());
    std::string sceneName = fileName.substr(fileName.find_last_of("\\/") + 1, fileName.find_last_of('.') - fileName.find_last_of("\\/") - 1);

    std::filesystem::path currPath = std::filesystem::current_path();

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOG(LogType::LOG_ERROR, "Failed to load mesh from: %s", path.data());
        return meshes;
    }

    aiMatrix4x4 globalTransform = scene->mRootNode->mTransformation;

    for (size_t m = 0; m < scene->mNumMeshes; ++m)
    {
        Model* model = new Model();
        auto mesh = scene->mMeshes[m];
        auto faces = mesh->mFaces;
        vec3f* verts = (vec3f*)mesh->mVertices;
        vec3f* texCoords = (vec3f*)mesh->mTextureCoords[0];

        // Process Anim
        if (mesh->HasBones())
        {
            // TODO: Cosa de importar el fbx a .mesh del ozz con el fbx2mesh.exe
            //std::string importPath = Resources::PathToLibrary<Model>(sceneName) + model->meshName;
            //model->ImportToOzz(path, importPath);

            //TODO: Process new materials for animated meshes
        }

        // Process materials
        if (scene->HasMaterials())
        {
            for (unsigned int i = 0; i < scene->mNumMaterials; i++)
            {
                aiMaterial* mat = scene->mMaterials[i];

                aiString name;
                aiGetMaterialString(mat, AI_MATKEY_NAME, &name);

                aiString texture_diffuse;
                aiGetMaterialString(mat, AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), &texture_diffuse);

                aiColor4D diffuse(1.0f, 1.0f, 1.0f, 1.0f);
                aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &diffuse);

                aiColor4D specular(1.0f, 1.0f, 1.0f, 1.0f);
                aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &specular);

                float shininess = 0.1f;
                aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &shininess);

                std::filesystem::path matPath = Resources::PathToLibrary<Material>(sceneName);
                matPath += name.C_Str();
                matPath += ".toematerial";

                ResourceId matID = Resources::LoadFromLibrary<Material>(matPath.string().c_str());

                model->materials.push_back(matPath.string());

                if (matID != -1)
                    continue;

                // Import material
                Material material;
                size_t id;

                if (texture_diffuse.length > 0)
                {
                    // TODO: this only loads texture paths if they are in the same folder as the fbx
                    fs::path texPath = fs::path(path).parent_path() / fs::path(texture_diffuse.C_Str()).filename();


                    id = Resources::LoadFromLibrary<Shader>("MeshTexture");
                    material.setShader(Resources::GetResourceById<Shader>(id), Resources::PathToLibrary<Shader>() + "MeshTexture.toeshader");
                    bool imported = Resources::Import<Texture>(texPath.string());

                    if (imported) {
                        ResourceId imgId = Resources::Load<Texture>(texPath.string().c_str());
                        Texture* img = Resources::GetResourceById<Texture>(imgId);
                        Uniform::SamplerData data;
                        data.tex_id = img->GetTextureId();
                        data.resource_id = imgId;
                        memcpy(data.tex_path, &texPath.string()[0], texPath.string().size() + 1);
                        material.SetUniformData("tex", data);
                    }
                }
                else
                {
                    id = Resources::LoadFromLibrary<Shader>("MeshColor");
                    material.setShader(Resources::GetResourceById<Shader>(id), Resources::PathToLibrary<Shader>() + "MeshColor.toeshader");

                    material.SetUniformData("color", glm::vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a));
                }


                Resources::Import<Material>(matPath.string().c_str(), &material);
            }
        }

        // Basic Data

        std::string standarizedName = mesh->mName.C_Str();
        size_t index = 0;
        while ((index = standarizedName.find('.', index)) != std::string::npos) {
            standarizedName.replace(index, 1, "_");
            index++;
        }
        model->meshName = standarizedName;
        model->format = BufferLayout({
        { ShaderDataType::Float3, "aPos"           },
        { ShaderDataType::Float2, "aTex"           } });
        model->materialIndex = mesh->mMaterialIndex;

        // Vertex & Index Data
        std::vector<float> vertex_data;
        std::vector<unsigned int> index_data;
        for (size_t i = 0; i < mesh->mNumVertices; ++i)
        {
            // Vertices
            vertex_data.push_back(mesh->mVertices[i].x);
            vertex_data.push_back(mesh->mVertices[i].y);
            vertex_data.push_back(mesh->mVertices[i].z);
            // Texture coordinates
            if (mesh->mTextureCoords[0])
            {
                vertex_data.push_back(mesh->mTextureCoords[0][i].x);
                vertex_data.push_back(mesh->mTextureCoords[0][i].y);
            }
            else {
                vertex_data.push_back(0.0f);
                vertex_data.push_back(0.0f);
            }
        }
        model->vertexData = vertex_data;

        for (size_t f = 0; f < mesh->mNumFaces; ++f)
        {
            index_data.push_back(faces[f].mIndices[0]);
            index_data.push_back(faces[f].mIndices[1]);
            index_data.push_back(faces[f].mIndices[2]);
        }
        model->indexData = index_data;

        // Transform
        aiMatrix4x4 transform;
        glm::mat4 mTransform(1);
        if (mesh->mName != (aiString)"Scene")
        {
            aiNode* meshNode = scene->mRootNode->FindNode(mesh->mName);
            if (meshNode)
            {
                transform = meshNode->mTransformation;
                mTransform = glm::transpose(glm::make_mat4(&transform.a1));
            }
        }
        model->meshTransform = mTransform;


        // Extra data
        for (size_t i = 0; i < mesh->mNumVertices; i++) {
            aiVector3D normal = mesh->mNormals[i];
            vec3f glmNormal(normal.x, normal.y, normal.z);
            model->meshNorms.push_back(glmNormal);
        }
        for (size_t i = 0; i < mesh->mNumVertices; i++) {
            aiVector3D vert = mesh->mVertices[i];
            vec3f glmNormal(vert.x, vert.y, vert.z);
            model->meshVerts.push_back(glmNormal);
        }
        for (size_t f = 0; f < mesh->mNumFaces; ++f)
        {
            aiFace face = mesh->mFaces[f];

            vec3f v0(mesh->mVertices[face.mIndices[0]].x, mesh->mVertices[face.mIndices[0]].y, mesh->mVertices[face.mIndices[0]].z);
            vec3f v1(mesh->mVertices[face.mIndices[1]].x, mesh->mVertices[face.mIndices[1]].y, mesh->mVertices[face.mIndices[1]].z);
            vec3f v2(mesh->mVertices[face.mIndices[2]].x, mesh->mVertices[face.mIndices[2]].y, mesh->mVertices[face.mIndices[2]].z);

            vec3f faceNormal = glm::cross(v1 - v0, v2 - v0);
            faceNormal = glm::normalize(faceNormal);
            model->meshFaceNorms.push_back(faceNormal);

            vec3f faceCenter = (v0 + v1 + v2) / 3.0f;
            model->meshFaceCenters.push_back(faceCenter);
        }

        Resources::Import<Model>(Resources::PathToLibrary<Model>(sceneName) + model->meshName, model);

        model->GenBufferData();
        meshes.push_back(model);
    }
    aiReleaseImport(scene);

    return meshes;
}

void Model::GenBufferData()
{
    meshVAO = std::make_shared<VertexArray>();

    if (glGetError() != 0)
    {
        LOG(LogType::LOG_ERROR, "Check error %s", glewGetErrorString(glGetError()));
    }
    meshVBO = std::make_shared<VertexBuffer>(vertexData.data(), vertexData.size() * sizeof(float));

    if (glGetError() != 0)
    {
        LOG(LogType::LOG_ERROR, "Check error %s", glewGetErrorString(glGetError()));
    }

    meshVBO->SetLayout({
        { ShaderDataType::Float3, "aPos"           },
        { ShaderDataType::Float2, "aTex"           }
        });
    meshVAO->AddVertexBuffer(meshVBO);

    std::shared_ptr<IndexBuffer> meshIB = std::make_shared<IndexBuffer>(indexData.data(), indexData.size());


    if (glGetError() != 0)
    {
        LOG(LogType::LOG_ERROR, "Check error %s", glewGetErrorString(glGetError()));
    }

    meshVAO->SetIndexBuffer(meshIB);


    if (glGetError() != 0)
    {
        LOG(LogType::LOG_ERROR, "Check error %s", glewGetErrorString(glGetError()));
    }


    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCALL(glBindVertexArray(0));

    if (glGetError() != 0)
    {
        LOG(LogType::LOG_ERROR, "Check error %s", glewGetErrorString(glGetError()));
    }
}

void Model::serializeMeshData(const std::string& filename)
{
    std::ofstream outFile(filename, std::ios::binary);

    // Write meshName size and data
    size_t meshNameSize = meshName.size();
    outFile.write(reinterpret_cast<const char*>(&meshNameSize), sizeof(size_t));
    outFile.write(meshName.c_str(), meshNameSize);

    // Write format
    //outFile.write(reinterpret_cast<const char*>(&format), format.GetSize());

    // Write vertex_data size and data
    uint numVerts = static_cast<uint>(vertexData.size());
    outFile.write(reinterpret_cast<const char*>(&numVerts), sizeof(uint));
    outFile.write(reinterpret_cast<const char*>(&vertexData[0]), numVerts * sizeof(float));

    // Write index_data size and data
    uint numIndexs = static_cast<uint>(indexData.size());
    outFile.write(reinterpret_cast<const char*>(&numIndexs), sizeof(uint));
    outFile.write(reinterpret_cast<const char*>(&indexData[0]), numIndexs * sizeof(uint));

    // Write meshVerts size and data
    uint numMeshVerts = static_cast<uint>(meshVerts.size());
    outFile.write(reinterpret_cast<const char*>(&numMeshVerts), sizeof(uint));
    outFile.write(reinterpret_cast<const char*>(&meshVerts[0]), numMeshVerts * sizeof(vec3f));

    // Write meshNorms size and data
    uint numMeshNorms = static_cast<uint>(meshNorms.size());
    outFile.write(reinterpret_cast<const char*>(&numMeshNorms), sizeof(uint));
    outFile.write(reinterpret_cast<const char*>(&meshNorms[0]), numMeshNorms * sizeof(vec3f));

    // Write meshFaceCenters size and data
    uint numMeshFaceCenters = static_cast<uint>(meshFaceCenters.size());
    outFile.write(reinterpret_cast<const char*>(&numMeshFaceCenters), sizeof(uint));
    outFile.write(reinterpret_cast<const char*>(&meshFaceCenters[0]), numMeshFaceCenters * sizeof(vec3f));

    // Write meshFaceNorms size and data
    uint numMeshFaceNorms = static_cast<uint>(meshFaceNorms.size());
    outFile.write(reinterpret_cast<const char*>(&numMeshFaceNorms), sizeof(uint));
    outFile.write(reinterpret_cast<const char*>(&meshFaceNorms[0]), numMeshFaceNorms * sizeof(vec3f));

    // Write Materials path
    uint matSize = materials.size();
    outFile.write(reinterpret_cast<const char*>(&matSize), sizeof(uint));
    for (uint i = 0; i < matSize; i++)
    {
        uint pathSize = materials[i].size();

        outFile.write(reinterpret_cast<const char*>(&pathSize), sizeof(uint));
        outFile.write(materials[i].c_str(), pathSize);
    }

    // Write Material index
    uint matID = materialIndex;
    outFile.write(reinterpret_cast<const char*>(&matID), sizeof(uint));

    // Write transform
    outFile.write(reinterpret_cast<const char*>(&meshTransform), sizeof(glm::mat4));


    LOG(LogType::LOG_OK, "-%s created", filename.data());

    outFile.close();
}

void Model::deserializeMeshData(const std::string& filename)
{

    std::ifstream inFile(filename, std::ios::binary);

    // Read meshName
    size_t meshNameSize = meshName.size();
    inFile.read(reinterpret_cast<char*>(&meshNameSize), sizeof(size_t));

    meshName.resize(meshNameSize);
    inFile.read(&meshName[0], meshNameSize);

    // Read format
    //inFile.read(reinterpret_cast<char*>(&format), sizeof(BufferLayout));

    // Read vertex_data size and allocate memory
    uint numVerts;
    inFile.read(reinterpret_cast<char*>(&numVerts), sizeof(uint));
    vertexData.resize(numVerts);

    // Read vertex_data
    inFile.read(reinterpret_cast<char*>(&vertexData[0]), numVerts * sizeof(float));

    // Read index_data size and allocate memory
    uint numIndexs;
    inFile.read(reinterpret_cast<char*>(&numIndexs), sizeof(uint));
    indexData.resize(numIndexs);

    // Read index_data
    inFile.read(reinterpret_cast<char*>(&indexData[0]), numIndexs * sizeof(uint));

    // Read meshVerts size and allocate memory
    uint numMeshVerts;
    inFile.read(reinterpret_cast<char*>(&numMeshVerts), sizeof(uint));
    meshVerts.resize(numMeshVerts);

    // Read meshVerts
    inFile.read(reinterpret_cast<char*>(&meshVerts[0]), numMeshVerts * sizeof(vec3f));

    // Read meshNorms size and allocate memory
    uint numMeshNorms;
    inFile.read(reinterpret_cast<char*>(&numMeshNorms), sizeof(uint));
    meshNorms.resize(numMeshNorms);

    // Read meshNorms
    inFile.read(reinterpret_cast<char*>(&meshNorms[0]), numMeshNorms * sizeof(vec3f));

    // Read meshFaceCenters size and allocate memory
    uint numMeshFaceCenters;
    inFile.read(reinterpret_cast<char*>(&numMeshFaceCenters), sizeof(uint));
    meshFaceCenters.resize(numMeshFaceCenters);

    // Read meshFaceCenters
    inFile.read(reinterpret_cast<char*>(&meshFaceCenters[0]), numMeshFaceCenters * sizeof(vec3f));

    // Read meshFaceNorms size and allocate memory
    uint numMeshFaceNorms;
    inFile.read(reinterpret_cast<char*>(&numMeshFaceNorms), sizeof(uint));
    meshFaceNorms.resize(numMeshFaceNorms);

    // Read meshFaceNorms
    inFile.read(reinterpret_cast<char*>(&meshFaceNorms[0]), numMeshFaceNorms * sizeof(vec3f));

    // Read Materials path
    uint mat_size;
    inFile.read(reinterpret_cast<char*>(&mat_size), sizeof(uint));
    materials.resize(mat_size);
    for (uint i = 0; i < mat_size; i++) {
        uint name_size;

        inFile.read(reinterpret_cast<char*>(&name_size), sizeof(uint));
        materials[i].resize(name_size);

        inFile.read(&materials[i][0], name_size);
    }

    // Read material index
    inFile.read(reinterpret_cast<char*>(&materialIndex), sizeof(uint));

    // Read transform
    inFile.read(reinterpret_cast<char*>(&meshTransform[0]), sizeof(glm::mat4));


    LOG(LogType::LOG_INFO, "-%s loaded", filename.data());
    inFile.close();
}


void Model::SaveMesh(Model* mesh, const std::string& path)
{
    mesh->serializeMeshData(path);
}

void Model::Render()
{
    meshVAO->Bind();
    GLCALL(glDrawElements(GL_TRIANGLES, (GLsizei)indexData.size(), GL_UNSIGNED_INT, 0));
    meshVAO->Unbind();
}

/*======================================================= OZZ ANIMATIOR MESH =======================================================*/

bool Model::LoadSkeleton(const std::string& path)
{
    m_LoadedSkeleton = ozz::sample::LoadSkeleton(path.c_str(), &m_Skeleton);

    if (m_LoadedSkeleton)
    {
        m_SkeletonPath = path;

        // Allocates runtime buffers.
        const int num_joints = m_Skeleton.num_joints();
        const int num_soa_joints = m_Skeleton.num_soa_joints();

        models_.resize(num_joints);
        // Allocates local space runtime buffers of blended data.
        blended_locals_.resize(num_soa_joints);
    }

    return m_LoadedSkeleton;
}


size_t Model::_create_anim_impl()
{
    size_t index = 0;

    if (m_RemovedAnimationsIndex.size() > 0)
    {
        index = m_RemovedAnimationsIndex[m_RemovedAnimationsIndex.size() - 1];
        m_RemovedAnimationsIndex.pop_back();
    }
    else
    {
        index = m_AnimationList.size();
        m_AnimationList.emplace_back();
    }

    return index;
}

bool Model::_update(float _dt)
{
    AnimationData& a_data = m_AnimationList[m_ActiveAnimationId];

    OzzAnimation* animation = a_data.animation;

    if (!animation->Update(_dt)) return false;

    ozz::vector<ozz::math::SoaTransform>& locals = animation->getLocals();

    // Converts from local space to model space matrices.
    // Gets the output of the blending stage, and converts it to model space.

    // Setup local-to-model conversion job.
    ozz::animation::LocalToModelJob ltm_job;
    ltm_job.skeleton = &m_Skeleton;
    ltm_job.input = make_span(locals);
    ltm_job.output = make_span(models_);

    // Run ltm job.
    if (!ltm_job.Run())
        return false;

    // Build skinning matrices
    for (size_t i = 0; i < m_Mesh.joint_remaps.size(); ++i)
        skinning_matrices_[i] = models_[m_Mesh.joint_remaps[i]] * m_Mesh.inverse_bind_poses[i];

    return true;
}

bool Model::_update_blend(float _dt)
{
    float weight_ratio = m_TransitionTimer / m_TransitionTime;

    AnimationData& a_data = m_AnimationList[m_ActiveAnimationId];
    AnimationData& pa_data = m_AnimationList[m_PrevAnimationId];

    OzzAnimation* animation = a_data.animation;
    OzzAnimation* prevanimation = pa_data.animation;

    if (!animation->Update(_dt)) return false;
    if (!prevanimation->Update(_dt)) return false;

    ozz::vector<ozz::math::SoaTransform>& locals = animation->getLocals();
    ozz::vector<ozz::math::SoaTransform>& prevlocals = prevanimation->getLocals();

    // Prepares blending layers.
    ozz::animation::BlendingJob::Layer layers[2];
    layers[0].transform = make_span(locals);
    layers[0].weight = weight_ratio;

    layers[1].transform = make_span(prevlocals);
    layers[1].weight = 1.0f - weight_ratio;

    // Setups blending job.
    ozz::animation::BlendingJob blend_job;
    blend_job.threshold = m_BlendThreshold;
    blend_job.layers = layers;
    blend_job.rest_pose = m_Skeleton.joint_rest_poses();
    blend_job.output = make_span(blended_locals_);

    // Blends.
    if (!blend_job.Run())
        return false;

    // Setup local-to-model conversion job.
    ozz::animation::LocalToModelJob ltm_job;
    ltm_job.skeleton = &m_Skeleton;
    ltm_job.input = make_span(blended_locals_);
    ltm_job.output = make_span(models_);

    // Run ltm job.
    if (!ltm_job.Run())
        return false;

    // Build skinning matrices
    for (size_t i = 0; i < m_Mesh.joint_remaps.size(); ++i)
        skinning_matrices_[i] = models_[m_Mesh.joint_remaps[i]] * m_Mesh.inverse_bind_poses[i];

    m_TransitionTimer += _dt;

    if (m_TransitionTimer >= m_TransitionTime)
    {
        m_UpdateState = US_UPDATE;
        m_TransitionTimer = 0.0f;
    }

    return true;
}

ozz::span<const char* const> Model::getSkeletonBoneNames()
{
    if (!m_LoadedSkeleton)
        return ozz::span<const char* const>();

    return m_Skeleton.joint_names();
}

size_t Model::CreateSimpleAnimation(const std::string& name)
{
    // Return NULL if name already in list
    if (HasAnimation(name)) return INVALID_INDEX;

    size_t anim_id = _create_anim_impl();

    // Add index to map
    m_AnimationsIndex[name] = anim_id;

    // Create partial animation
    OzzAnimationSimple* simple_anim = new OzzAnimationSimple();
    simple_anim->SetSkeleton(&m_Skeleton);

    AnimationData& a_data = m_AnimationList[anim_id];

    a_data.name = name;
    a_data.animation = simple_anim;

    return anim_id;
}


size_t Model::CreatePartialAnimation(const std::string& name)
{
    // Return NULL if name already in list
    if (HasAnimation(name)) return INVALID_INDEX;

    size_t anim_id = _create_anim_impl();

    // Add index to map
    m_AnimationsIndex[name] = anim_id;

    // Create partial animation
    OzzAnimationPartialBlending* partial_anim = new OzzAnimationPartialBlending();
    partial_anim->SetSkeleton(&m_Skeleton);

    AnimationData& a_data = m_AnimationList[anim_id];

    a_data.name = name;
    a_data.animation = partial_anim;

    return anim_id;
}

void Model::RemoveAnimationAt(size_t index)
{
    AnimationData& a_data = m_AnimationList[index];

    m_RemovedAnimationsIndex.push_back(index);
    m_AnimationsIndex.erase(a_data.name);

    a_data.name = "Removed";
    delete a_data.animation;
    a_data.animation = nullptr;

    if (m_ActiveAnimationId == index)
    {
        m_ActiveAnimationId = INVALID_INDEX;
        m_ActiveAnimationName = "";
    }
}

bool Model::HasAnimation(const std::string& name)
{
    return m_AnimationsIndex.find(name) != m_AnimationsIndex.end();
}

Model::AnimationData* Model::PlayAnimation(const std::string& name, float time_ratio)
{
    std::unordered_map<std::string, size_t>::iterator it = m_AnimationsIndex.find(name);

    if (it != m_AnimationsIndex.end())
        return PlayAnimation(it->second, time_ratio);

    return nullptr;
}

Model::AnimationData* Model::PlayAnimation(size_t anim_id, float time_ratio)
{
    if (anim_id >= 0 && anim_id < m_AnimationList.size())
    {
        AnimationData& a_data = m_AnimationList[anim_id];
        a_data.animation->Play();

        if (anim_id == m_ActiveAnimationId)
        {
            if (!a_data.animation->HasFinished() || a_data.animation->getLoop())
                return &a_data;
        }

        a_data.animation->SetTimeRatio(time_ratio);

        if (a_data.animation->getStatus() == OzzAnimation::Status::VALID)
        {
            if (m_ActiveAnimationId != INVALID_INDEX)
            {
                if (m_BlendOnTransition)
                {
                    m_UpdateState = US_BLEND;
                    m_PrevAnimationId = m_ActiveAnimationId;
                }
            }

            m_ActiveAnimationId = anim_id;
            m_ActiveAnimationName = a_data.name;

            return &m_AnimationList[m_ActiveAnimationId];
        }

        return nullptr;
    }

    return nullptr;
}

void Model::StopAnimation(bool unbind)
{
    if (m_ActiveAnimationId != INVALID_INDEX)
    {
        AnimationData& a_data = m_AnimationList[m_ActiveAnimationId];

        a_data.animation->Stop();

        if (unbind)
        {
            m_ActiveAnimationId = INVALID_INDEX;
            m_ActiveAnimationName = "";
        }
    }
}

OzzAnimation* Model::getAnimationByName(const std::string& name)
{
    std::unordered_map<std::string, size_t>::iterator it = m_AnimationsIndex.find(name);

    if (it != m_AnimationsIndex.end())
        return m_AnimationList[it->second].animation;

    return nullptr;
}

size_t Model::getAnimationIndex(const std::string& str)
{
    std::unordered_map<std::string, size_t>::iterator it = m_AnimationsIndex.find(str);

    if (it != m_AnimationsIndex.end())
        return it->second;

    return INVALID_INDEX;
}

OzzAnimation* Model::getActiveAnimation()
{
    OzzAnimation* anim = nullptr;

    if (m_ActiveAnimationId != INVALID_INDEX)
        anim = m_AnimationList[m_ActiveAnimationId].animation;

    return anim;
}

void Model::SetUpperPlaybackSpeed(size_t anim_id, float playback_speed)
{
    if (anim_id < 0 || anim_id >= m_AnimationList.size()) return;

    AnimationData& a_data = m_AnimationList[anim_id];

    if (!a_data.animation) return;

    if (a_data.animation->getAnimationType() == AnimationType::AT_PARTIAL_BLEND)
    {
        OzzAnimationPartialBlending* a_partial = (OzzAnimationPartialBlending*)a_data.animation;
        a_partial->SetUpperPlaybackSpeed(playback_speed);
    }
}

void Model::SetLowerPlaybackSpeed(size_t anim_id, float playback_speed)
{
    if (anim_id < 0 || anim_id >= m_AnimationList.size()) return;

    AnimationData& a_data = m_AnimationList[anim_id];

    if (!a_data.animation) return;

    if (a_data.animation->getAnimationType() == AnimationType::AT_PARTIAL_BLEND)
    {
        OzzAnimationPartialBlending* a_partial = (OzzAnimationPartialBlending*)a_data.animation;
        a_partial->SetLowerPlaybackSpeed(playback_speed);
    }
}

bool Model::UpdateAnim(float _dt)
{
    if (m_ActiveAnimationId != INVALID_INDEX)
    {
        switch (m_UpdateState)
        {
        case US_UPDATE: _update(_dt); break;
        case US_BLEND: _update_blend(_dt); break;
        default: break;
        }
    }

    return true;
}

void Model::ImportToOzz(const std::string& file, const std::filesystem::path& importPath)
{
    std::string filename = importPath.filename().string();

    // Skeleton output path
    std::filesystem::path skeleton_path = importPath;
    skeleton_path.replace_filename(filename + "_skeleton");
    skeleton_path.replace_extension(".skeleton");

    // Animations output path
    std::filesystem::path anim_out_path = importPath;
    anim_out_path.replace_filename(filename + "_*");
    anim_out_path.replace_extension(".anim");


    // Mesh output path
    std::filesystem::path mesh_out_path = importPath;
    mesh_out_path.replace_filename(filename + "_animatedmesh");
    mesh_out_path.replace_extension(".mesh");
    path = mesh_out_path.string();

    // === Setup ozz import json config ===
    json ozzImportJSON;

    // skeleton settings
    json skeletonJSON;

    // skeleton filename
    skeletonJSON["filename"] = skeleton_path.string().c_str();

    // skeleton import settings
    json skeletonImportJSON;
    skeletonImportJSON["enable"] = true;
    skeletonImportJSON["raw"] = false;

    skeletonJSON["import"] = skeletonImportJSON;

    ozzImportJSON["skeleton"] = skeletonJSON;

    // animation settings
    json animationsJSON;
    json animJSON;
    animJSON["clip"] = "*";
    animJSON["filename"] = anim_out_path.string().c_str();

    animationsJSON = animJSON;

    ozzImportJSON = animationsJSON;

    Resources::SaveJSON("Tools\\import_anim.json", ozzImportJSON);

    // === Import ozz ===
    std::string cmd = "Tools\\import_anim.bat";
    cmd += " \"";
    cmd += file.c_str(); // Path of file to import (.fbx)
    cmd += "\" \"";
    cmd += mesh_out_path.string().c_str(); // Path of output mesh
    cmd += "\" \"";
    cmd += skeleton_path.string().c_str(); // Path of skeleton to import mesh
    cmd += "\"";
    system(cmd.c_str());
}