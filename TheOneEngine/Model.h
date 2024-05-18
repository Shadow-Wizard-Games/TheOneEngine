#pragma once
#include "VertexArray.h"
#include "Animation/animations/OzzAnimationPartialBlending.h"


class Texture;
struct aiMesh;

struct MeshVertex
{
    vec3f position;
    vec2f texCoords;
};

struct ModelData
{
    std::vector<float> vertexData;
    std::vector<unsigned int> indexData;

    BufferLayout format;

    StackVertexArray meshVAO;
    VertexBuffer meshVBO;
    IndexBuffer meshIBO;

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

    std::string GetMaterialPath() { return materials[materialIndex]; }
    std::string GetMeshPath() { return path; }
    std::string GetMeshName() { return meshName; }
    glm::mat4 GetMeshTransform() { return meshTransform; }

    void SetMeshPath(const std::string& newPath) {path = newPath;}

    void serializeMeshData(const std::string& filename, const ModelData& data);
    void deserializeMeshData(const std::string& filename);

    uint32_t rendererID = 0;

private:
    void GenBufferData(ModelData& data);

    std::string meshName;

    glm::mat4 meshTransform;

    std::string path;

    std::vector<std::string> materials;
    uint materialIndex = INVALID_INDEX;
};
