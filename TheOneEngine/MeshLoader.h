#ifndef __MESH_LOADER_H__
#define __MESH_LOADER_H__
#pragma once

#include "Defs.h"
#include "Component.h"
#include "Mesh.h"
#include "Texture.h"

#include <vector>
#include <memory>
#include <string>

class MeshLoader
{
public:

	enum Formats { F_V3, F_V3C4, F_V3T2 };
	struct V3 { vec3f v; };
	struct V3C4 { vec3f v; vec4f c; };
	struct V3T2 { vec3f v; vec2f t; };

	struct MeshData
	{
		Formats format;
		const void* vertex_data;
		uint numVerts;
		const uint* index_data = nullptr;
		uint numIndexs = 0;
	};

	struct MeshBufferedData
	{
		const enum Formats format;

		uint vertex_buffer_id;
		const uint numVerts;

		uint indexs_buffer_id;
		const uint numIndexs;

		std::string texturePath;
	};

public:

	MeshLoader();
	//MeshLoader(MeshLoader&& b) noexcept;
	~MeshLoader();

	std::vector<MeshBufferedData> loadFromFile(std::shared_ptr<GameObject> containerGO, const std::string& path);
	void BufferData(MeshData meshData);


private:

	/*MeshLoader(const MeshLoader& cpy);
	MeshLoader& operator=(const MeshLoader&);*/

private:

	MeshData meshData;
	MeshBufferedData meshBuffData;
	
};

#endif // !__MESH_LOADER_H__