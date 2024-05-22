#pragma once
#include "Defs.h"
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
}
