#ifndef __MESH_H__
#define __MESH_H__
#pragma once

#include "Defs.h"
#include "Component.h"
#include "Resources.h"

#include "../TheOneEngine/Animation/samples/mesh.h"
#include "../external/ozz/include/base/maths/simd_math.h"
#include "../external/ozz/include/base/span.h"

#include <vector>
#include <memory>
#include <string>


class Camera;
class Material;
class GameObject;
struct MeshBufferData;


class Mesh : public Component
{
public:

    Mesh(std::shared_ptr<GameObject> containerGO);
    Mesh(std::shared_ptr<GameObject> containerGO, Mesh* ref);
    virtual ~Mesh();

    void DrawComponent(Camera* camera);

    bool RenderOzzSkinnedMesh(
        const ozz::sample::Mesh& _mesh,
        Material* material,
        const ozz::span<ozz::math::Float4x4> _skinning_matrices,
        const ozz::math::Float4x4& _transform);

    json SaveComponent();
    void LoadComponent(const json& meshJSON);

private:
    //void DrawVertexNormals();
    //void DrawFaceNormals();
    //void DrawWireframe();

public:
    ResourceId meshID = -1;
    ResourceId materialID = -1;

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
