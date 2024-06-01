#include "Model.h"
#include "Defs.h"
#include "BBox.hpp"

#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <filesystem>
#include <fstream>
#include "Resources.h"


Model::Model(const std::string& filename) : 
    meshTransform(glm::mat4(1))
{
    // This constructor only loads .mesh
    if (filename.ends_with(".mesh"))
    {
        path = filename;
        deserializeMeshData(filename);
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


        // Basic Data
        std::string standarizedName = mesh->mName.C_Str();
        size_t index = 0;
        while ((index = standarizedName.find('.', index)) != std::string::npos) {
            standarizedName.replace(index, 1, "_");
            index++;
        }
        model->meshName = standarizedName;

        // AABB
        model->initialAABB = AABBox(vec3(mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z), 
                                    vec3(mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z));


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
                            texPath = Resources::PathToLibrary<Texture>() + texPath.filename().replace_extension(".dds").string();
                            ResourceId imgId = Resources::LoadFromLibrary<Texture>(texPath.string().c_str());
                            Texture* img = Resources::GetResourceById<Texture>(imgId);
                            Uniform::SamplerData data;
                            data.tex_id = img->GetTextureId();
                            data.resource_id = imgId;
                            memcpy(data.tex_path, &texPath.string()[0], texPath.string().size() + 1);
                            material.SetUniformData("diffuse", data);
                        }
                }
                else
                {
                    id = Resources::LoadFromLibrary<Shader>("MeshColor");
                    material.setShader(Resources::GetResourceById<Shader>(id), Resources::PathToLibrary<Shader>() + "MeshColor.toeshader");

                    material.SetUniformData("u_Color", glm::vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a));
                }


                Resources::Import<Material>(matPath.string().c_str(), &material);
            }
        }
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
            // Normals
            vertex_data.push_back(mesh->mNormals[i].x);
            vertex_data.push_back(mesh->mNormals[i].y);
            vertex_data.push_back(mesh->mNormals[i].z);
        }

        ModelData data;
        data.vertexData = vertex_data;

        for (size_t f = 0; f < mesh->mNumFaces; ++f)
        {
            index_data.push_back(faces[f].mIndices[0]);
            index_data.push_back(faces[f].mIndices[1]);
            index_data.push_back(faces[f].mIndices[2]);
        }
        data.indexData = index_data;

        // Extra data
        for (size_t i = 0; i < mesh->mNumVertices; i++) {
            aiVector3D normal = mesh->mNormals[i];
            vec3f glmNormal(normal.x, normal.y, normal.z);
            data.meshNorms.push_back(glmNormal);
        }
        for (size_t i = 0; i < mesh->mNumVertices; i++) {
            aiVector3D vert = mesh->mVertices[i];
            vec3f glmNormal(vert.x, vert.y, vert.z);
            data.meshVerts.push_back(glmNormal);
        }
        for (size_t f = 0; f < mesh->mNumFaces; ++f)
        {
            aiFace face = mesh->mFaces[f];

            vec3f v0(mesh->mVertices[face.mIndices[0]].x, mesh->mVertices[face.mIndices[0]].y, mesh->mVertices[face.mIndices[0]].z);
            vec3f v1(mesh->mVertices[face.mIndices[1]].x, mesh->mVertices[face.mIndices[1]].y, mesh->mVertices[face.mIndices[1]].z);
            vec3f v2(mesh->mVertices[face.mIndices[2]].x, mesh->mVertices[face.mIndices[2]].y, mesh->mVertices[face.mIndices[2]].z);

            vec3f faceNormal = glm::cross(v1 - v0, v2 - v0);
            faceNormal = glm::normalize(faceNormal);
            data.meshFaceNorms.push_back(faceNormal);

            vec3f faceCenter = (v0 + v1 + v2) / 3.0f;
            data.meshFaceCenters.push_back(faceCenter);
        }
        model->GenBufferData(data);
        meshes.push_back(model);

        Resources::Import<Model>(Resources::PathToLibrary<Model>(sceneName) + model->meshName, model, data);
    }
    aiReleaseImport(scene);

    return meshes;
}

void Model::GenBufferData(ModelData& data)
{
    meshVAO.Create();

    VertexBuffer meshVBO = VertexBuffer(data.vertexData.data(), data.vertexData.size() * sizeof(float));

    meshVBO.SetLayout({
        { ShaderDataType::Float3, "a_Pos"          },
        { ShaderDataType::Float2, "a_UV"           },
        { ShaderDataType::Float3, "a_Normal"       }
        });
    meshVAO.AddVertexBuffer(meshVBO);

    IndexBuffer meshIBO = IndexBuffer(data.indexData.data(), data.indexData.size());

    meshVAO.SetIndexBuffer(meshIBO);


    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCALL(glBindVertexArray(0));
}

void Model::serializeMeshData(const std::string& filename, const ModelData& data)
{
    std::ofstream outFile(filename, std::ios::binary);

    // Write meshName size and data
    size_t meshNameSize = meshName.size();
    outFile.write(reinterpret_cast<const char*>(&meshNameSize), sizeof(size_t));
    outFile.write(meshName.c_str(), meshNameSize);

    // Write format
    //outFile.write(reinterpret_cast<const char*>(&format), format.GetSize());

    // Write vertex_data size and data
    uint numVerts = static_cast<uint>(data.vertexData.size());
    outFile.write(reinterpret_cast<const char*>(&numVerts), sizeof(uint));
    outFile.write(reinterpret_cast<const char*>(&data.vertexData[0]), numVerts * sizeof(float));

    // Write index_data size and data
    uint numIndexs = static_cast<uint>(data.indexData.size());
    outFile.write(reinterpret_cast<const char*>(&numIndexs), sizeof(uint));
    outFile.write(reinterpret_cast<const char*>(&data.indexData[0]), numIndexs * sizeof(uint));

    // Write meshVerts size and data
    uint numMeshVerts = static_cast<uint>(data.meshVerts.size());
    outFile.write(reinterpret_cast<const char*>(&numMeshVerts), sizeof(uint));
    outFile.write(reinterpret_cast<const char*>(&data.meshVerts[0]), numMeshVerts * sizeof(vec3f));

    // Write meshNorms size and data
    uint numMeshNorms = static_cast<uint>(data.meshNorms.size());
    outFile.write(reinterpret_cast<const char*>(&numMeshNorms), sizeof(uint));
    outFile.write(reinterpret_cast<const char*>(&data.meshNorms[0]), numMeshNorms * sizeof(vec3f));

    // Write meshFaceCenters size and data
    uint numMeshFaceCenters = static_cast<uint>(data.meshFaceCenters.size());
    outFile.write(reinterpret_cast<const char*>(&numMeshFaceCenters), sizeof(uint));
    outFile.write(reinterpret_cast<const char*>(&data.meshFaceCenters[0]), numMeshFaceCenters * sizeof(vec3f));

    // Write meshFaceNorms size and data
    uint numMeshFaceNorms = static_cast<uint>(data.meshFaceNorms.size());
    outFile.write(reinterpret_cast<const char*>(&numMeshFaceNorms), sizeof(uint));
    outFile.write(reinterpret_cast<const char*>(&data.meshFaceNorms[0]), numMeshFaceNorms * sizeof(vec3f));

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

    // Write AABB
    outFile.write(reinterpret_cast<const char*>(&initialAABB.min), sizeof(vec3));
    outFile.write(reinterpret_cast<const char*>(&initialAABB.max), sizeof(vec3));


    LOG(LogType::LOG_OK, "-%s created", filename.data());

    outFile.close();
}

void Model::deserializeMeshData(const std::string& filename)
{
    ModelData data;

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
    data.vertexData.resize(numVerts);

    // Read vertex_data
    inFile.read(reinterpret_cast<char*>(&data.vertexData[0]), numVerts * sizeof(float));

    // Read index_data size and allocate memory
    uint numIndexs;
    inFile.read(reinterpret_cast<char*>(&numIndexs), sizeof(uint));
    data.indexData.resize(numIndexs);

    // Read index_data
    inFile.read(reinterpret_cast<char*>(&data.indexData[0]), numIndexs * sizeof(uint));

    // Read meshVerts size and allocate memory
    uint numMeshVerts;
    inFile.read(reinterpret_cast<char*>(&numMeshVerts), sizeof(uint));
    data.meshVerts.resize(numMeshVerts);

    // Read meshVerts
    inFile.read(reinterpret_cast<char*>(&data.meshVerts[0]), numMeshVerts * sizeof(vec3f));

    // Read meshNorms size and allocate memory
    uint numMeshNorms;
    inFile.read(reinterpret_cast<char*>(&numMeshNorms), sizeof(uint));
    data.meshNorms.resize(numMeshNorms);

    // Read meshNorms
    inFile.read(reinterpret_cast<char*>(&data.meshNorms[0]), numMeshNorms * sizeof(vec3f));

    // Read meshFaceCenters size and allocate memory
    uint numMeshFaceCenters;
    inFile.read(reinterpret_cast<char*>(&numMeshFaceCenters), sizeof(uint));
    data.meshFaceCenters.resize(numMeshFaceCenters);

    // Read meshFaceCenters
    inFile.read(reinterpret_cast<char*>(&data.meshFaceCenters[0]), numMeshFaceCenters * sizeof(vec3f));

    // Read meshFaceNorms size and allocate memory
    uint numMeshFaceNorms;
    inFile.read(reinterpret_cast<char*>(&numMeshFaceNorms), sizeof(uint));
    data.meshFaceNorms.resize(numMeshFaceNorms);

    // Read meshFaceNorms
    inFile.read(reinterpret_cast<char*>(&data.meshFaceNorms[0]), numMeshFaceNorms * sizeof(vec3f));

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

    // Read AABB
    inFile.read(reinterpret_cast<char*>(&initialAABB.min), sizeof(vec3));
    inFile.read(reinterpret_cast<char*>(&initialAABB.max), sizeof(vec3));


    LOG(LogType::LOG_INFO, "-%s loaded", filename.data());
    inFile.close();

    GenBufferData(data);
}


void Model::SaveMesh(Model* mesh, const std::string& path, const ModelData& data)
{
    mesh->serializeMeshData(path, data);
}