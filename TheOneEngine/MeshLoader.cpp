#include "MeshLoader.h"
#include "Texture.h"
#include "Mesh.h"
#include "../TheOneEngine/Log.h"

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

void MeshLoader::FixAssimpRotation(const aiScene* scene)
{

    if (scene->mMetaData)
    {
        int32_t upAxis = 1, upAxisSign = 1, frontAxis = 2, frontAxisSign = 1, coordAxis = 0, coordAxisSign = 1;
        double UnitScaleFactor = 1.0;
        for (unsigned MetadataIndex = 0; MetadataIndex < scene->mMetaData->mNumProperties; ++MetadataIndex)
        {
            if (strcmp(scene->mMetaData->mKeys[MetadataIndex].C_Str(), "UpAxis") == 0)
                scene->mMetaData->Get<int32_t>(MetadataIndex, upAxis);

            if (strcmp(scene->mMetaData->mKeys[MetadataIndex].C_Str(), "UpAxisSign") == 0)
                scene->mMetaData->Get<int32_t>(MetadataIndex, upAxisSign);

            if (strcmp(scene->mMetaData->mKeys[MetadataIndex].C_Str(), "FrontAxis") == 0)
                scene->mMetaData->Get<int32_t>(MetadataIndex, frontAxis);

            if (strcmp(scene->mMetaData->mKeys[MetadataIndex].C_Str(), "FrontAxisSign") == 0)
                scene->mMetaData->Get<int32_t>(MetadataIndex, frontAxisSign);

            if (strcmp(scene->mMetaData->mKeys[MetadataIndex].C_Str(), "CoordAxis") == 0)
                scene->mMetaData->Get<int32_t>(MetadataIndex, coordAxis);

            if (strcmp(scene->mMetaData->mKeys[MetadataIndex].C_Str(), "CoordAxisSign") == 0)
                scene->mMetaData->Get<int32_t>(MetadataIndex, coordAxisSign);

            if (strcmp(scene->mMetaData->mKeys[MetadataIndex].C_Str(), "UnitScaleFactor") == 0)
                scene->mMetaData->Get<double>(MetadataIndex, UnitScaleFactor);
        }

        /*int upAxis = 0;
        scene->mMetaData->Get<int>("UpAxis", upAxis);
        int upAxisSign = 1;
        scene->mMetaData->Get<int>("UpAxisSign", upAxisSign);

        int frontAxis = 0;
        scene->mMetaData->Get<int>("FrontAxis", upAxis);
        int frontAxisSign = 1;
        scene->mMetaData->Get<int>("FrontAxisSign", upAxisSign);

        int coordAxis = 0;
        scene->mMetaData->Get<int>("CoordAxis", upAxis);
        int coordAxisSign = 1;
        scene->mMetaData->Get<int>("CoordAxisSign", upAxisSign);*/

        aiVector3D upVec = upAxis == 0 ? aiVector3D(upAxisSign, 0, 0) : upAxis == 1 ? aiVector3D(0, upAxisSign, 0) : aiVector3D(0, 0, upAxisSign);
        aiVector3D forwardVec = frontAxis == 0 ? aiVector3D(frontAxisSign, 0, 0) : frontAxis == 1 ? aiVector3D(0, frontAxisSign, 0) : aiVector3D(0, 0, frontAxisSign);
        aiVector3D rightVec = coordAxis == 0 ? aiVector3D(coordAxisSign, 0, 0) : coordAxis == 1 ? aiVector3D(0, coordAxisSign, 0) : aiVector3D(0, 0, coordAxisSign);

        aiMatrix4x4 mat(
            rightVec.x, rightVec.y, rightVec.z, 0.0f,
            upVec.x, upVec.y, upVec.z, 0.0f,
            forwardVec.x, forwardVec.y, forwardVec.z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        scene->mRootNode->mTransformation *= mat;
    }


#pragma region TEST2
    /*if (scene->mMetaData)
    {
        int32_t UpAxis = 1, UpAxisSign = 1, FrontAxis = 2, FrontAxisSign = 1, CoordAxis = 0, CoordAxisSign = 1;
        double UnitScaleFactor = 1.0;
        for (unsigned MetadataIndex = 0; MetadataIndex < scene->mMetaData->mNumProperties; ++MetadataIndex)
        {
            if (strcmp(scene->mMetaData->mKeys[MetadataIndex].C_Str(), "UpAxis") == 0)
                scene->mMetaData->Get<int32_t>(MetadataIndex, UpAxis);

            if (strcmp(scene->mMetaData->mKeys[MetadataIndex].C_Str(), "UpAxisSign") == 0)
                scene->mMetaData->Get<int32_t>(MetadataIndex, UpAxisSign);

            if (strcmp(scene->mMetaData->mKeys[MetadataIndex].C_Str(), "FrontAxis") == 0)
                scene->mMetaData->Get<int32_t>(MetadataIndex, FrontAxis);

            if (strcmp(scene->mMetaData->mKeys[MetadataIndex].C_Str(), "FrontAxisSign") == 0)
                scene->mMetaData->Get<int32_t>(MetadataIndex, FrontAxisSign);

            if (strcmp(scene->mMetaData->mKeys[MetadataIndex].C_Str(), "CoordAxis") == 0)
                scene->mMetaData->Get<int32_t>(MetadataIndex, CoordAxis);

            if (strcmp(scene->mMetaData->mKeys[MetadataIndex].C_Str(), "CoordAxisSign") == 0)
                scene->mMetaData->Get<int32_t>(MetadataIndex, CoordAxisSign);
          
            if (strcmp(scene->mMetaData->mKeys[MetadataIndex].C_Str(), "UnitScaleFactor") == 0)
                scene->mMetaData->Get<double>(MetadataIndex, UnitScaleFactor);
        }

        aiVector3D upVec, forwardVec, rightVec;

        upVec[UpAxis] = UpAxisSign * (float)UnitScaleFactor;
        forwardVec[FrontAxis] = FrontAxisSign * (float)UnitScaleFactor;
        rightVec[CoordAxis] = CoordAxisSign * (float)UnitScaleFactor;

        aiMatrix4x4 mat(
            rightVec.x, rightVec.y, rightVec.z, 0.0f,
            upVec.x, upVec.y, upVec.z, 0.0f,
            forwardVec.x, forwardVec.y, forwardVec.z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        scene->mRootNode->mTransformation *= mat;
    }*/
#pragma endregion TEST2

}

std::vector<MeshBufferedData> MeshLoader::LoadMesh(const std::string& path)
{
    std::vector<MeshBufferedData> meshesBufferedData;

    const aiScene* scene = aiImportFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_ForceGenNormals);
    //hekbas - need to fix this :(
    //FixAssimpRotation(scene);

    std::string fileName = scene->GetShortFilename(path.c_str());
    std::string sceneName = fileName.substr(fileName.find_last_of("\\/") + 1, fileName.find_last_of('.') - fileName.find_last_of("\\/") - 1);
    std::string folderName = "Library/Meshes/" + sceneName + "/";

    std::filesystem::create_directories(folderName);

    if (scene != NULL)
    {
        aiMatrix4x4 globalTransform = scene->mRootNode->mTransformation;

        for (size_t m = 0; m < scene->mNumMeshes; ++m)
        {
            // Mesh
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


            // Transform
            aiMatrix4x4 transform;
            glm::mat4 meshTransform;

            if (mesh->mName != (aiString)"Scene")
            {
                transform = scene->mRootNode->FindNode(mesh->mName)->mTransformation;
                meshTransform = glm::transpose(glm::make_mat4(&transform.a1));
            }
            else
            {
                meshTransform = glm::mat4(1);
            }
            
 
            // Texture Path
            aiString aiPath;
            scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &aiPath);
            fs::path texPath = fs::path(path).parent_path() / fs::path(aiPath.C_Str()).filename();

            if (meshTransform != glm::mat4(1))
            {
                meshTransform = meshTransform;
            }

            if (!transform.IsIdentity())
            {
                meshTransform = meshTransform;
            }

            meshData =
            {
                mesh->mName.C_Str(),
                Formats::F_V3T2,
                vertex_data,
                index_data,
                meshTransform,
                texPath.string()
            };           

            BufferData(meshData);
            meshBuffData.meshName = mesh->mName.C_Str();
            meshBuffData.materialIndex = mesh->mMaterialIndex;

            for (size_t i = 0; i < mesh->mNumVertices; i++) {
                aiVector3D normal = mesh->mNormals[i];
                vec3f glmNormal(normal.x, normal.y, normal.z);
                meshData.meshNorms.push_back(glmNormal);
            }
            for (size_t i = 0; i < mesh->mNumVertices; i++) {
                aiVector3D vert = mesh->mVertices[i];
                vec3f glmNormal(vert.x, vert.y, vert.z);
                meshData.meshVerts.push_back(glmNormal);
            }
            for (size_t f = 0; f < mesh->mNumFaces; ++f)
            {
                aiFace face = mesh->mFaces[f];

                vec3f v0(mesh->mVertices[face.mIndices[0]].x, mesh->mVertices[face.mIndices[0]].y, mesh->mVertices[face.mIndices[0]].z);
                vec3f v1(mesh->mVertices[face.mIndices[1]].x, mesh->mVertices[face.mIndices[1]].y, mesh->mVertices[face.mIndices[1]].z);
                vec3f v2(mesh->mVertices[face.mIndices[2]].x, mesh->mVertices[face.mIndices[2]].y, mesh->mVertices[face.mIndices[2]].z);

                vec3f faceNormal = glm::cross(v1 - v0, v2 - v0);
                faceNormal = glm::normalize(faceNormal);
                meshData.meshFaceNorms.push_back(faceNormal);

                vec3f faceCenter = (v0 + v1 + v2) / 3.0f;
                meshData.meshFaceCenters.push_back(faceCenter);
            }

            serializeMeshData(meshData, folderName + meshData.meshName + ".mesh");

            meshesBufferedData.push_back(meshBuffData);
        }
        aiReleaseImport(scene);
    }
    else
    {
        LOG(LogType::LOG_ERROR, "Failed to load mesh from: %s", path.data());
    }
    

    return meshesBufferedData;
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
    std::ofstream outFile(filename, ios::binary);

    // Write meshName size and data
    size_t meshNameSize = data.meshName.size();
    outFile.write(reinterpret_cast<const char*>(&meshNameSize), sizeof(size_t));
    outFile.write(data.meshName.c_str(), meshNameSize);

    // Write format
    outFile.write(reinterpret_cast<const char*>(&data.format), sizeof(Formats));

    // Write vertex_data size and data
    uint numVerts = static_cast<uint>(data.vertex_data.size());
    outFile.write(reinterpret_cast<const char*>(&numVerts), sizeof(uint));
    outFile.write(reinterpret_cast<const char*>(&data.vertex_data[0]), numVerts * sizeof(V3T2));

    // Write index_data size and data
    uint numIndexs = static_cast<uint>(data.index_data.size());
    outFile.write(reinterpret_cast<const char*>(&numIndexs), sizeof(uint));
    outFile.write(reinterpret_cast<const char*>(&data.index_data[0]), numIndexs * sizeof(uint));
    
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

    if (!data.texturePath.empty())
    {
        // Write texture path
        size_t texPathSize = data.texturePath.size();
        outFile.write(reinterpret_cast<const char*>(&texPathSize), sizeof(size_t));
        outFile.write(data.texturePath.c_str(), texPathSize);
    }

    // Write transform
    outFile.write(reinterpret_cast<const char*>(&data.meshTransform), sizeof(glm::mat4));

    LOG(LogType::LOG_OK, "-%s created", filename.data());

    outFile.close();
}

MeshData MeshLoader::deserializeMeshData(const std::string& filename)
{
    MeshData data;

    std::ifstream inFile(filename, ios::binary);

    // Read meshName
    size_t meshNameSize;
    inFile.read(reinterpret_cast<char*>(&meshNameSize), sizeof(size_t));

    data.meshName.resize(meshNameSize);
    inFile.read(&data.meshName[0], meshNameSize);

    // Read format
    inFile.read(reinterpret_cast<char*>(&data.format), sizeof(Formats));

    // Read vertex_data size and allocate memory
    uint numVerts;
    inFile.read(reinterpret_cast<char*>(&numVerts), sizeof(uint));
    data.vertex_data.resize(numVerts);

    // Read vertex_data
    inFile.read(reinterpret_cast<char*>(&data.vertex_data[0]), numVerts * sizeof(V3T2));

    // Read index_data size and allocate memory
    uint numIndexs;
    inFile.read(reinterpret_cast<char*>(&numIndexs), sizeof(uint));
    data.index_data.resize(numIndexs);

    // Read index_data
    inFile.read(reinterpret_cast<char*>(&data.index_data[0]), numIndexs * sizeof(uint));

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

    // Read texture path
    size_t texPathSize = 0;
    inFile.read(reinterpret_cast<char*>(&texPathSize), sizeof(size_t));
    if (texPathSize)
    {
        data.texturePath.resize(texPathSize);
        inFile.read(&data.texturePath[0], texPathSize);
    }

    // Write transform
    inFile.read(reinterpret_cast<char*>(&data.meshTransform[0]), sizeof(glm::mat4));

    LOG(LogType::LOG_INFO, "-%s loaded", filename.data());
    inFile.close();

    return data;
}