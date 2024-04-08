#pragma once
#include "VertexArray.h"


class Texture;
struct aiMesh;

struct MeshVertex
{
    vec3f position;
    vec2f texCoords;
};

class Model
{
public:
    friend class ModelScene;

    Model(){}
    Model(const std::string& path);
    ~Model(){}

    static std::vector<Model*> LoadMeshes(const std::string& path);
    static void SaveMesh(Model* mesh, const std::string& path);

    void Render();

    std::string GetMaterialPath() { return materials[materialIndex]; }

    std::string meshName;
    std::vector<float> vertexData;
    std::vector<unsigned int> indexData;

    glm::mat4 meshTransform;

    BufferLayout format;

    std::shared_ptr<VertexArray> meshVAO;
    std::shared_ptr<VertexBuffer> meshVBO;
    std::shared_ptr<IndexBuffer> meshIBO;

    std::vector<vec3f> meshVerts;
    std::vector<vec3f> meshNorms;
    std::vector<vec3f> meshFaceCenters;
    std::vector<vec3f> meshFaceNorms;


    void serializeMeshData(const std::string& filename);
    void deserializeMeshData(const std::string& filename);

private:
    void GenBufferData();

    std::vector<std::string> materials;
    uint materialIndex = 0;
};
