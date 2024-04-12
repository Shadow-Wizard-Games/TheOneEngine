#include "Mesh.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"

#include "Log.h"

#include <span>
#include <vector>
#include <array>
#include <cstdio>
#include <cassert>


Mesh::Mesh(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Mesh)
{
    active = true;
    drawNormalsFaces = false;
    drawNormalsVerts = false;
    drawAABB = true;
    drawChecker = false;

    normalLineWidth = 1;
    normalLineLength = 0.1f;
}

Mesh::Mesh(std::shared_ptr<GameObject> containerGO, Mesh* ref) : Component(containerGO, ComponentType::Mesh)
{
    active = ref->active;
    drawNormalsFaces = ref->drawNormalsFaces;
    drawNormalsVerts = ref->drawNormalsVerts;
    drawAABB = ref->drawAABB;
    drawChecker = ref->drawChecker;

    meshID = ref->meshID;
    materialID = ref->materialID;

    normalLineWidth = ref->normalLineWidth;
    normalLineLength = ref->normalLineLength;
}

Mesh::~Mesh() {}


// Draw
void Mesh::DrawComponent(Camera* camera)
{
    std::shared_ptr<GameObject> containerGO = GetContainerGO();

    Model* mesh = Resources::GetResourceById<Model>(meshID);
    Material* mat = Resources::GetResourceById<Material>(materialID);

    Shader* matShader = mat->getShader();
    matShader->Bind();
    matShader->SetModel(containerGO.get()->GetComponent<Transform>()->CalculateWorldTransform());

    mat->Bind();

    if (!active)
        return;

    if (drawWireframe)
    {
        GLCALL(glDisable(GL_TEXTURE_2D));
        GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
    }
    else
    {
        GLCALL(glEnable(GL_TEXTURE_2D));
        GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
    }

    mesh->Render();

    mat->UnBind();

    /*if (drawNormalsVerts) DrawVertexNormals();
    if (drawNormalsFaces) DrawFaceNormals();*/
}

//void Mesh::DrawVertexNormals()
//{
//    if (meshData.meshVerts.empty() || meshData.meshNorms.empty()) return;
//
//    GLCALL(glLineWidth(normalLineWidth);
//    GLCALL(glColor3f(1.0f, 1.0f, 0.0f);
//    GLCALL(glBegin(GL_LINES);
//
//    for (int i = 0; i < meshData.meshVerts.size(); i++)
//    {
//        GLCALL(glVertex3f(meshData.meshVerts[i].x, meshData.meshVerts[i].y, meshData.meshVerts[i].z);
//        GLCALL(glVertex3f(meshData.meshVerts[i].x + meshData.meshNorms[i].x * normalLineLength,
//            meshData.meshVerts[i].y + meshData.meshNorms[i].y * normalLineLength,
//            meshData.meshVerts[i].z + meshData.meshNorms[i].z * normalLineLength);
//    }
//
//    GLCALL(glColor3f(1.0f, 1.0f, 0.0f);
//    GLCALL(glEnd();
//}

//void Mesh::DrawFaceNormals() 
//{
//    if (meshData.meshFaceCenters.empty() || meshData.meshFaceNorms.empty()) return;
//
//    GLCALL(glLineWidth(normalLineWidth);
//    GLCALL(glColor3f(1.0f, 0.0f, 1.0f);
//    GLCALL(glBegin(GL_LINES);
//
//    for (int i = 0; i < meshData.meshFaceCenters.size(); i++)
//    {
//        glm::vec3 endPoint = meshData.meshFaceCenters[i] + normalLineLength * meshData.meshFaceNorms[i];
//        GLCALL(glVertex3f(meshData.meshFaceCenters[i].x, meshData.meshFaceCenters[i].y, meshData.meshFaceCenters[i].z);
//        GLCALL(glVertex3f(endPoint.x, endPoint.y, endPoint.z);
//    }
//
//    GLCALL(glColor3f(0.0f, 1.0f, 1.0f);
//    GLCALL(glEnd();
//}


// Load/Save
json Mesh::SaveComponent()
{
    json meshJSON;

    meshJSON["Name"] = name;
    meshJSON["Type"] = type;
    if (auto pGO = containerGO.lock())
    {
        meshJSON["ParentUID"] = pGO.get()->GetUID();
    }
    meshJSON["UID"] = UID;
    meshJSON["Active"] = active;
    meshJSON["DrawWireframe"] = drawWireframe;
    meshJSON["DrawAABB"] = drawAABB;
    meshJSON["DrawOBB"] = drawOBB;
    meshJSON["DrawChecker"] = drawChecker;
    meshJSON["DrawNormalsVerts"] = drawNormalsVerts;
    meshJSON["DrawNormalsFaces"] = drawNormalsFaces;

    Model* mesh = Resources::GetResourceById<Model>(meshID);
    meshJSON["MeshPath"] = mesh->path;
    Material* mat = Resources::GetResourceById<Material>(materialID);
    meshJSON["MaterialPath"] = mat->getPath();

    return meshJSON;
}

void Mesh::LoadComponent(const json& meshJSON)
{
    // Load basic properties
    if (meshJSON.contains("UID"))
    {
        UID = meshJSON["UID"];
    }

    if (meshJSON.contains("Name"))
    {
        name = meshJSON["Name"];
    }

    // Load mesh-specific properties
    if (meshJSON.contains("Active"))
    {
        active = meshJSON["Active"];
    }

    if (meshJSON.contains("DrawWireframe"))
    {
        drawWireframe = meshJSON["DrawWireframe"];
    }

    if (meshJSON.contains("DrawAABB"))
    {
        drawAABB = meshJSON["DrawAABB"];
    }

    if (meshJSON.contains("DrawOBB"))
    {
        drawOBB = meshJSON["DrawOBB"];
    }

    if (meshJSON.contains("DrawChecker"))
    {
        drawChecker = meshJSON["DrawChecker"];
    }

    if (meshJSON.contains("DrawNormalsVerts"))
    {
        drawNormalsVerts = meshJSON["DrawNormalsVerts"];
    }

    if (meshJSON.contains("DrawNormalsFaces"))
    {
        drawNormalsFaces = meshJSON["DrawNormalsFaces"];
    }

    if (meshJSON.contains("Path"))
    {
        std::filesystem::path legacyPath = meshJSON["Path"];
        if (!legacyPath.string().empty())
        {
            std::string assetsMesh = Resources::FindFileInAssets(legacyPath.parent_path().filename().string());
            Resources::LoadMultiple<Model>(assetsMesh);
            meshID = Resources::LoadFromLibrary<Model>(legacyPath.string());
            Model* model = Resources::GetResourceById<Model>(meshID);
            materialID = Resources::LoadFromLibrary<Material>(model->GetMaterialPath());
        }
    }

    if (meshJSON.contains("MeshPath"))
    {
        std::string meshPath = meshJSON["MeshPath"];
        if (Resources::FindFileInLibrary(meshPath).empty())
        {
            std::filesystem::path libraryToAssets = meshPath;
            std::string assetsMesh = Resources::FindFileInAssets(libraryToAssets.parent_path().filename().string());
            Resources::LoadMultiple<Model>(assetsMesh);
            meshID = Resources::LoadFromLibrary<Model>(meshPath);
        }
        else
        {
            meshID = Resources::LoadFromLibrary<Model>(meshPath);
        }
    }

    if (meshJSON.contains("MaterialPath"))
    {
        materialID = Resources::LoadFromLibrary<Material>(meshJSON["MaterialPath"]);
    }
}