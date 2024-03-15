#pragma once
#include "VertexArray.h"


class Texture;

struct MeshVertex
{
    vec3f position;
    vec2f texCoords;
};

class Model
{
    friend class ModelScene;

    Model();
    ~Model();

    std::string meshName;

    uint numFaces;
    BufferLayout format;

    std::string texturePath;
    uint materialIndex;

    std::shared_ptr<VertexArray> meshVAO;
    std::shared_ptr<VertexBuffer> meshVBO;
    std::shared_ptr<IndexBuffer> meshIBO;

    std::vector<MeshVertex> vertexData;
    std::vector<unsigned int> indexData;

    std::vector<vec3f> meshVerts;
    std::vector<vec3f> meshNorms;
    std::vector<vec3f> meshFaceCenters;
    std::vector<vec3f> meshFaceNorms;

    void LoadExistingMesh(const std::string& path);

    void serializeMeshData(const std::string& filename);
    void deserializeMeshData(const std::string& filename);

private:
    void GenBufferData();
};

class ModelScene
{
public:

    ModelScene() {}
    ~ModelScene() {}

    static void LoadMeshes(const std::string& path);

    static const std::vector<std::shared_ptr<Model>> GetMeshes() { return meshes; }

private:

private:
    static std::vector<std::shared_ptr<Model>> meshes;
};
