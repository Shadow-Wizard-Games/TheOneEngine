#ifndef __MESH_H__
#define __MESH_H__
#pragma once

#include "Defs.h"
#include "MeshLoader.h"
#include "Component.h"
#include "Texture.h"
#include "Graphic.h"

#include <vector>
#include <memory>
#include <string>

class GameObject;
struct MeshBufferData;

class Mesh : public Component
{
public:

    Mesh(std::shared_ptr<GameObject> containerGO);
    Mesh(std::shared_ptr<GameObject> containerGO, Mesh* ref);
    virtual ~Mesh();

    void DrawComponent(Camera* camera);

    json SaveComponent();
    void LoadComponent(const json& meshJSON);

private:

    void ConfigureVertexFormat();
    void DrawVertexNormals();
    void DrawFaceNormals();
    //void DrawWireframe();

public:

    MeshBufferedData mesh;
    MeshData meshData;

    bool active;
    bool drawWireframe;
    bool drawNormalsVerts;
    bool drawNormalsFaces;
    bool drawAABB;
    bool drawOBB;
    bool drawChecker;
   
    int normalLineWidth;
    float normalLineLength;

    std::string path;

private:

    MeshLoader* meshLoader;
};

#endif // !__MESH_H__
