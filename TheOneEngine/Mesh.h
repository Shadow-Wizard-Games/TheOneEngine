#ifndef __MESH_H__
#define __MESH_H__
#pragma once

#include "Defs.h"
#include "Component.h"
#include "Resources.h"
#include "FbxImporter.h"
#include "SkeletalModel.h"

#include "../TheOneAnimation/samples/framework/mesh.h"
#include "ozz/base/maths/simd_math.h"
#include "ozz/base/span.h"

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

    json SaveComponent();
    void LoadComponent(const json& meshJSON);

private:
    //void DrawVertexNormals();
    //void DrawFaceNormals();
    //void DrawWireframe();

    bool RenderOzzSkinnedMesh(
        SkeletalModel* mesh,
        int matID,
        const ozz::span<ozz::math::Float4x4> _skinning_matrices,
        const mat4& _transform);
    bool RenderMesh(Model* mesh, Material* material, const mat4& transform);

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

    MeshType type;
};

#endif // !__MESH_H__
