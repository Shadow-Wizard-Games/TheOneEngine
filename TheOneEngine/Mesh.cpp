#include "Mesh.h"
#include "EngineCore.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "Light.h"
#include "N_SceneManager.h"
#include "Log.h"
#include "Defs.h"
#include "FbxImporter.h"
#include "Renderer3D.h"

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

    if (!active || meshID == -1)
        return;


	mat4 transform = containerGO.get()->GetComponent<Transform>()->CalculateWorldTransform();
	Model* mesh; SkeletalModel* skMesh;
	switch (type)
	{
	case MeshType::DEFAULT:
		mesh = Resources::GetResourceById<Model>(meshID);
		Renderer3D::AddMeshToQueue(mesh->GetMeshID(), materialID, transform);
		break;
	case MeshType::SKELETAL:
        Renderer3D::AddSkeletalMeshToQueue(meshID, materialID, transform);
		break;
	default:
		break;
	}
}


// Load/Save
json Mesh::SaveComponent()
{
    json meshJSON;

    meshJSON["Name"] = name;
    meshJSON["MeshType"] = type;
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

	switch (type)
	{
	case MeshType::DEFAULT:
	{
		Model* mesh = Resources::GetResourceById<Model>(meshID);
		meshJSON["MeshPath"] = mesh->GetMeshPath();
	}
		break;
	case MeshType::SKELETAL:
	{
		SkeletalModel* mesh = Resources::GetResourceById<SkeletalModel>(meshID);
		meshJSON["MeshPath"] = mesh->GetMeshPath();
	}
		break;
	}

    Material* mat = Resources::GetResourceById<Material>(materialID);
    std::string matPath = mat->getPath();
    meshJSON["MaterialPath"] = matPath;

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

	if (meshJSON.contains("MeshType"))
	{
		type = meshJSON["MeshType"];
	}

    /*if (meshJSON.contains("Path"))
    {
        std::filesystem::path legacyPath = meshJSON["Path"];
        if (!legacyPath.string().empty())
        {
            std::filesystem::path assetsMesh = Resources::FindFileInAssets(legacyPath.parent_path().filename().string());
            Resources::LoadMultiple<Model>(assetsMesh.replace_extension(".fbx").string());
            meshID = Resources::LoadFromLibrary<Model>(legacyPath.string());
            Model* model = Resources::GetResourceById<Model>(meshID);
            materialID = Resources::LoadFromLibrary<Material>(model->GetMaterialPath());
        }
    }*/

    if (meshJSON.contains("MeshPath"))
    {
        std::string meshPath = meshJSON["MeshPath"];
        if (Resources::FindFileInLibrary(meshPath).empty())
        {
            std::filesystem::path libraryToAssets = meshPath;
            std::string assetsMesh = Resources::FindFileInAssets(libraryToAssets.parent_path().filename().string());
			type = FBXIMPORTER::FBXtype(assetsMesh);
			switch (type)
			{
			case MeshType::DEFAULT:
				Resources::LoadMultiple<Model>(assetsMesh);
				meshID = Resources::LoadFromLibrary<Model>(meshPath);
				break;
			case MeshType::SKELETAL:
				meshID = Resources::Load<SkeletalModel>(assetsMesh);
				break;
			}
        }
        else
        {
			if (meshPath.ends_with(".animator"))
				type = MeshType::SKELETAL;
			else
				type = MeshType::DEFAULT;

			switch (type)
			{
			case MeshType::DEFAULT:
				meshID = Resources::LoadFromLibrary<Model>(meshPath);
				break;
			case MeshType::SKELETAL:
				meshID = Resources::LoadFromLibrary<SkeletalModel>(meshPath);
				break;
			}
        }
    }

    if (meshJSON.contains("MaterialPath"))
    {
        materialID = Resources::LoadFromLibrary<Material>(meshJSON["MaterialPath"]);
    }

	if (meshID != -1 && type == MeshType::DEFAULT)
		Renderer3D::AddMesh(Resources::GetResourceById<Model>(meshID)->GetMeshID(), materialID);
}