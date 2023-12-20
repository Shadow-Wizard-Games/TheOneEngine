#include "MeshLoader.h"
#include "Texture.h"
#include "Mesh.h"
#include "../TheOneEditor/Log.h"

#include "nlohmann/json.hpp"

#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>

#include <GL/glew.h>

#include <span>
#include <vector>
#include <array>
#include <filesystem>
#include <fstream>


namespace fs = std::filesystem;
using namespace std;
using namespace nlohmann::literals;

struct aiMeshExt : aiMesh
{
    auto verts() const { return span((vec3f*)mVertices, mNumVertices); }
    auto texCoords() const { return span((vec3f*)mTextureCoords[0], mNumVertices); }
    auto faces() const { return span(mFaces, mNumFaces); }
};

struct aiSceneExt : aiScene
{
    auto materials() const { return span(mMaterials, mNumMaterials); }
    auto meshes() const { return span((aiMeshExt**)mMeshes, mNumMeshes); }
};



MeshLoader::MeshLoader() {}

//MeshLoader::MeshLoader(MeshLoader&& b) noexcept :
//    Component(containerGO.lock(), ComponentType::Mesh),
//    format(b.format),
//    vertex_buffer_id(b.vertex_buffer_id),
//    numVerts(b.numVerts),
//    indexs_buffer_id(b.indexs_buffer_id),
//    numIndexs(b.numIndexs),
//    texture(b.texture)
//{
//    b.vertex_buffer_id = 0;
//    b.indexs_buffer_id = 0;
//}

MeshLoader::~MeshLoader()
{
    if (meshBuffData.vertex_buffer_id) glDeleteBuffers(1, &meshBuffData.vertex_buffer_id);
    if (meshBuffData.indexs_buffer_id) glDeleteBuffers(1, &meshBuffData.indexs_buffer_id);
}

void MeshLoader::BufferData(MeshData meshData)
{
    //extension = ".fbx";
    //this->path = ASSETS_PATH + std::to_string(ID) + extension;
    meshBuffData.numVerts = meshData.vertex_data.size();
    meshBuffData.numIndexs = meshData.index_data.size();
    glGenBuffers(1, &meshBuffData.vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, meshBuffData.vertex_buffer_id);

    switch (meshData.format)
    {
    case Formats::F_V3:
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3) * meshData.vertex_data.size(), meshData.vertex_data.data(), GL_STATIC_DRAW);
        meshBuffData.format = Formats::F_V3;
        break;
    case Formats::F_V3C4:
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3C4) * meshData.vertex_data.size(), meshData.vertex_data.data(), GL_STATIC_DRAW);
        meshBuffData.format = Formats::F_V3C4;
        break;
    case Formats::F_V3T2:
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3T2) * meshData.vertex_data.size(), meshData.vertex_data.data(), GL_STATIC_DRAW);
        meshBuffData.format = Formats::F_V3T2;
        break;
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (meshData.index_data.data())
    {
        glGenBuffers(1, &meshBuffData.indexs_buffer_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBuffData.indexs_buffer_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * meshData.index_data.size(), meshData.index_data.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    else {
        meshBuffData.indexs_buffer_id = 0;
    }
}

std::vector<MeshBufferedData> MeshLoader::LoadMesh(const std::string& path)
{
    std::vector<MeshBufferedData> meshesData;

    auto scene = aiImportFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_ForceGenNormals);

    if (scene != NULL)
    {
        for (size_t m = 0; m < scene->mNumMeshes; ++m)
        {
            auto mesh = scene->mMeshes[m];
            auto faces = mesh->mFaces;
            vec3f* verts = (vec3f*)mesh->mVertices;
            vec3f* texCoords = (vec3f*)mesh->mTextureCoords[0];

            std::vector<V3T2> vertex_data;
            std::vector<unsigned int> index_data;

            if (texCoords != nullptr)
            {
                for (size_t i = 0; i < mesh->mNumVertices; ++i)
                {
                    V3T2 v = { verts[i], vec2f(texCoords[i].x, texCoords[i].y) };
                    vertex_data.push_back(v);
                }
            }
            else
            {
                for (size_t i = 0; i < mesh->mNumVertices; ++i)
                {
                    V3T2 v = { verts[i], vec2f(0, 0) };
                    vertex_data.push_back(v);
                }
            }

            for (size_t f = 0; f < mesh->mNumFaces; ++f)
            {
                index_data.push_back(faces[f].mIndices[0]);
                index_data.push_back(faces[f].mIndices[1]);
                index_data.push_back(faces[f].mIndices[2]);
            }

            meshData =
            {
                mesh->mName.C_Str(),
                Formats::F_V3T2,
                vertex_data,
                index_data
            };

            BufferData(meshData);
            meshBuffData.meshName = mesh->mName.C_Str();
            meshBuffData.materialIndex = mesh->mMaterialIndex;

            for (size_t i = 0; i < mesh->mNumVertices; i++) {
                aiVector3D normal = mesh->mNormals[i];
                vec3f glmNormal(normal.x, normal.y, normal.z);
                meshBuffData.meshNorms.push_back(glmNormal);
            }
            for (size_t i = 0; i < mesh->mNumVertices; i++) {
                aiVector3D vert = mesh->mVertices[i];
                vec3f glmNormal(vert.x, vert.y, vert.z);
                meshBuffData.meshVerts.push_back(glmNormal);
            }
            for (size_t f = 0; f < mesh->mNumFaces; ++f)
            {
                aiFace face = mesh->mFaces[f];

                vec3f v0(mesh->mVertices[face.mIndices[0]].x, mesh->mVertices[face.mIndices[0]].y, mesh->mVertices[face.mIndices[0]].z);
                vec3f v1(mesh->mVertices[face.mIndices[1]].x, mesh->mVertices[face.mIndices[1]].y, mesh->mVertices[face.mIndices[1]].z);
                vec3f v2(mesh->mVertices[face.mIndices[2]].x, mesh->mVertices[face.mIndices[2]].y, mesh->mVertices[face.mIndices[2]].z);

                vec3f faceNormal = glm::cross(v1 - v0, v2 - v0);
                faceNormal = glm::normalize(faceNormal);
                meshBuffData.meshFaceNorms.push_back(faceNormal);

                vec3f faceCenter = (v0 + v1 + v2) / 3.0f;
                meshBuffData.meshFaceCenters.push_back(faceCenter);
            }

            std::string folderName = "Library/Models/" + meshData.meshName + "/";

            std::filesystem::create_directories(folderName);

            serializeMeshData(meshData, folderName + meshData.meshName + ".mesh");

            meshesData.push_back(meshBuffData);
        }
        aiReleaseImport(scene);
    }
    else
    {
        LOG(LogType::LOG_ERROR, "Failed to load mesh from: %s", path.data());
    }
    

    return meshesData;
}

std::vector<std::shared_ptr<Texture>> MeshLoader::LoadTexture(const std::string& path, std::shared_ptr<GameObject> containerGO)
{
    const auto scene_ptr = aiImportFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

    const aiSceneExt& scene = *(aiSceneExt*)scene_ptr;

    std::vector<std::shared_ptr<Texture>> texture_ptrs;

    if (scene_ptr != NULL)
    {
        for (auto& material : scene.materials())
        {
            aiString aiPath;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &aiPath);
            fs::path texPath = fs::path(path).parent_path() / fs::path(aiPath.C_Str()).filename();
            auto texture_ptr = make_shared<Texture>(texPath.string());
            texture_ptrs.push_back(texture_ptr);
        }

        aiReleaseImport(scene_ptr);
    }
    else
    {
        LOG(LogType::LOG_ERROR, "Failed to load textures from: %s", path.data());
    }

    return texture_ptrs;
}

void MeshLoader::serializeMeshData(const MeshData& data, const std::string& filename)
{
    nlohmann::json json;

    // Serialize MeshData members to JSON
    json["meshName"] = data.meshName;
    json["format"] = data.format;

    // Serialize vertex_data to JSON array
    for (const auto& vertex : data.vertex_data) {
        nlohmann::json vertexJson;
        vertexJson["v"]["x"] = vertex.v.x;
        vertexJson["v"]["y"] = vertex.v.y;
        vertexJson["v"]["z"] = vertex.v.z;
        vertexJson["t"]["x"] = vertex.t.x;
        vertexJson["t"]["y"] = vertex.t.y;
        json["vertex_data"].push_back(vertexJson);
    }

    // Serialize index_data to JSON array
    for (const auto& index : data.index_data) {
        json["index_data"].push_back(index);
    }

    std::ofstream outFile(filename);
    outFile << nlohmann::to_string(json) << std::endl;
    outFile.close();
}

MeshData MeshLoader::deserializeMeshData(const std::string& filename)
{
    MeshData data;

    // Load the JSON document from a file
    std::ifstream inFile(filename);
    nlohmann::json json;
    inFile >> json;
    inFile.close();

    // Deserialize MeshData members from JSON
    data.meshName = json["meshName"].get<std::string>();
    data.format = json["format"].get<Formats>();

    // Deserialize vertex_data from JSON array
    for (const auto& vertexJson : json["vertex_data"]) {
        V3T2 vertex;
        vertex.v.x = vertexJson["v"]["x"].get<float>();
        vertex.v.y = vertexJson["v"]["y"].get<float>();
        vertex.v.z = vertexJson["v"]["z"].get<float>();
        vertex.t.x = vertexJson["t"]["x"].get<float>();
        vertex.t.y = vertexJson["t"]["y"].get<float>();
        data.vertex_data.push_back(vertex);
    }

    // Deserialize index_data from JSON array
    for (const auto& index : json["index_data"]) {
        data.index_data.push_back(index.get<unsigned int>());
    }

    return data;
}