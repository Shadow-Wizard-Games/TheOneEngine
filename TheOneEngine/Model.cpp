#include "Model.h"
#include "Log.h"

#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <filesystem>
#include <fstream>
#include "Resources.h"

Model::Model(const std::string& path)
{
    if (path.ends_with(".mesh"))
    {
        deserializeMeshData(path);
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
        aiMatrix4x4 globalTransform = scene->mRootNode->mTransformation;

        for (size_t m = 0; m < scene->mNumMeshes; ++m)
        {
            Model* model = new Model();
            auto mesh = scene->mMeshes[m];
            auto faces = mesh->mFaces;
            vec3f* verts = (vec3f*)mesh->mVertices;
            vec3f* texCoords = (vec3f*)mesh->mTextureCoords[0];


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

                    std::filesystem::path matPath = path;
                    matPath += name.C_Str();
                    matPath += ".toematerial";

                    ResourceId matID = Resources::Load<Material>(matPath.string().c_str());

                    model->materials.push_back(matPath.string());

                    if (matID != -1)
                        continue;

                    // Import material
                    Material material;
                    size_t id;

                    if (texture_diffuse.length > 0)
                    {

                        std::filesystem::current_path(path);

                        std::filesystem::path texPath = texture_diffuse.C_Str();
                        texPath = std::filesystem::absolute(texPath);

                        std::filesystem::current_path(currPath);

                        texPath = std::filesystem::relative(texPath);

                        const char* defaultShader = "resources/shaders/light/toon_textured";

                        id = Resources::Load<Shader>(defaultShader);
                        material.setShader(Resources::GetResourceById<Shader>(id), defaultShader);
                        bool imported = Resources::Import<Texture>(texPath.string().c_str());

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
                        id = Resources::Load<Shader>("resources/shaders/light/toon_color");
                        material.setShader(Resources::GetResourceById<Shader>(id), "resources/shaders/light/toon_color");

                        material.SetUniformData("u_Color", glm::vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a));
                        material.SetUniformData("u_ToonLevels", 4);
                    }


                    Resources::Import<Material>(matPath.string().c_str(), &material);
                }
            }

            // Apply global transformation to vertices
            for (size_t i = 0; i < mesh->mNumVertices; ++i)
            {
                aiVector3D transformedVertex = globalTransform * mesh->mVertices[i];
                verts[i] = vec3f(transformedVertex.x, transformedVertex.y, transformedVertex.z);
            }

            std::vector<MeshVertex> vertex_data;
            std::vector<unsigned int> index_data;

            if (texCoords != nullptr)
            {
                for (size_t i = 0; i < mesh->mNumVertices; ++i)
                {
                    MeshVertex v = { verts[i], vec2f(texCoords[i].x, texCoords[i].y) };
                    vertex_data.push_back(v);
                }
            }
            else
            {
                for (size_t i = 0; i < mesh->mNumVertices; ++i)
                {
                    MeshVertex v = { verts[i], vec2f(0, 0) };
                    vertex_data.push_back(v);
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

            model->meshName = mesh->mName.C_Str();
            model->format = BufferLayout({
            { ShaderDataType::Float3, "aPos"           },
            { ShaderDataType::Float2, "aTex"           } });
            model->materialIndex = mesh->mMaterialIndex;

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

            meshes.push_back(model);
        }
        aiReleaseImport(scene);
    }
    else
    {
        LOG(LogType::LOG_ERROR, "Failed to load mesh from: %s", path.data());
    }

    return meshes;
}

void Model::GenBufferData()
{
    meshVAO = std::make_shared<VertexArray>();

    if (glGetError() != 0)
    {
        LOG(LogType::LOG_ERROR, "Check error %s", glewGetErrorString(glGetError()));
    }
    meshVBO = std::make_shared<VertexBuffer>(vertexData.data(), vertexData.size() * sizeof(MeshVertex));

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


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

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
    outFile.write(reinterpret_cast<const char*>(&format), format.GetSize());

    // Write vertex_data size and data
    uint numVerts = static_cast<uint>(vertexData.size());
    outFile.write(reinterpret_cast<const char*>(&numVerts), sizeof(uint));
    outFile.write(reinterpret_cast<const char*>(&vertexData[0]), numVerts * sizeof(MeshVertex));

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
    inFile.read(reinterpret_cast<char*>(&format), sizeof(BufferLayout));

    // Read vertex_data size and allocate memory
    uint numVerts;
    inFile.read(reinterpret_cast<char*>(&numVerts), sizeof(uint));
    vertexData.resize(numVerts);

    // Read vertex_data
    inFile.read(reinterpret_cast<char*>(&vertexData[0]), numVerts * sizeof(MeshVertex));

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

    LOG(LogType::LOG_INFO, "-%s loaded", filename.data());
    inFile.close();
}


void Model::SaveMesh(Model* mesh, const std::string& path)
{
    mesh->serializeMeshData(path);
}
