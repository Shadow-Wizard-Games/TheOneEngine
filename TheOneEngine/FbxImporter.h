#pragma once
#include "Defs.h"
#include "BBox.hpp"
#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>

enum class MeshType
{
	DEFAULT,
	STATIC,
	SKELETAL
};

namespace FBXIMPORTER {
	static MeshType FBXtype(const std::string& path, bool isStatic = false)
	{
		const aiScene* scene = aiImportFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_ForceGenNormals);

		for (int i = 0; i < scene->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[i];
			if (mesh->HasBones())
				return MeshType::SKELETAL;
		}
		return MeshType::DEFAULT;
	}

	/*static std::vector<AABBox> GetAABBfromFBX(const std::string& path)
	{
		std::vector<AABBox> AABBvector;
		const aiScene* scene = aiImportFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_ForceGenNormals);

		for (int i = 0; i < scene->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[i];
			AABBvector.emplace_back(mesh->mAABB.mMin, mesh->mAABB.mMax);
		}

		return AABBvector;
	}*/
}
