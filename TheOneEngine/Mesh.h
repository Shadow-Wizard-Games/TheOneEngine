#ifndef __MESH_H__
#define __MESH_H__
#pragma once

#include "Defs.h"

#include "Component.h"
#include "Resources.h"

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
    //void DrawVertexNormals();
    //void DrawFaceNormals();
    //void DrawWireframe();

public:
    ResourceId meshID;
    ResourceId materialID;

    bool active;
    bool drawWireframe;
    bool drawNormalsVerts;
    bool drawNormalsFaces;
    bool drawAABB;
    bool drawOBB;
    bool drawChecker;
   
    int normalLineWidth;
    float normalLineLength;
};

#endif // !__MESH_H__
