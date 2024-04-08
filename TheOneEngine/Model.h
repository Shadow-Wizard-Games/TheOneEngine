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

    std::string meshName;

    uint numFaces = 0;
    BufferLayout format;

    std::string texturePath;
    uint materialIndex = 0;

    std::shared_ptr<VertexArray> meshVAO;
    std::shared_ptr<VertexBuffer> meshVBO;
    std::shared_ptr<IndexBuffer> meshIBO;

    std::vector<float> vertexData;
    std::vector<unsigned int> indexData;

    std::vector<vec3f> meshVerts;
    std::vector<vec3f> meshNorms;
    std::vector<vec3f> meshFaceCenters;
    std::vector<vec3f> meshFaceNorms;

    std::vector<std::string> materials;

    void serializeMeshData(const std::string& filename);
    void deserializeMeshData(const std::string& filename);

private:
    void GenBufferData();
};
