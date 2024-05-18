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
	MeshType FBXtype(const std::string& path, bool isStatic = false)
	{
		const aiScene* scene = aiImportFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_ForceGenNormals);

		if (scene->hasSkeletons())
			return MeshType::SKELETAL;
		if (isStatic)
			return MeshType::STATIC;
		else
			return MeshType::DEFAULT;
	}
}
