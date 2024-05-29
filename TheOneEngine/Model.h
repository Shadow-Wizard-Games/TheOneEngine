#pragma once
#include "VertexArray.h"
#include "Animation/animations/OzzAnimationPartialBlending.h"

struct AABBox;

struct ModelData
{
    std::vector<float> vertexData;
    std::vector<unsigned int> indexData;

    BufferLayout format;

    std::vector<vec3f> meshVerts;
    std::vector<vec3f> meshNorms;
    std::vector<vec3f> meshFaceCenters;
    std::vector<vec3f> meshFaceNorms;
};

class Model
{
public:
    Model(){}
    Model(const std::string& path);
    ~Model(){}

    static std::vector<Model*> LoadMeshes(const std::string& path);
    static void SaveMesh(Model* mesh, const std::string& path, const ModelData& data);

    const StackVertexArray& GetMeshID() { return meshVAO; }
    const std::string& GetMaterialPath() { return materials[materialIndex]; }
    const std::string& GetMeshPath() { return path; }
    const std::string& GetMeshName() { return meshName; }
    const glm::mat4& GetMeshTransform() { return meshTransform; }
    const AABBox& GetMeshAABB() { return initialAABB; }

    void SetMeshPath(const std::string& newPath) {path = newPath;}

    void serializeMeshData(const std::string& filename, const ModelData& data);
    void deserializeMeshData(const std::string& filename);

private:
    void GenBufferData(ModelData& data);

    StackVertexArray meshVAO;

    std::string meshName;

    glm::mat4 meshTransform = glm::mat4(1);

    std::string path;

    std::vector<std::string> materials;
    uint materialIndex = INVALID_INDEX;

    AABBox initialAABB;
};
