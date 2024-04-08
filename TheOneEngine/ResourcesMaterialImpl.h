#pragma once
#include "ResourcesImpl.h"
#include "ResourcesImageImpl.h"

//--SPECIALIZATION FOR MATERIAL
template<>
inline std::string Resources::PathToLibrary<Material>(const std::string& folderName)
{
	if (!folderName.empty())
		return "Library/Materials/" + folderName + "/";
	else
		return "Library/Materials/";
}
template<>
inline bool Resources::Import<Material>(const std::string& file, Material* mat)
{
	/*std::filesystem::path importPath = AssetToLibPath(file);
	importPath.replace_extension(".toematerial");*/
	std::filesystem::path importPath = file;

	if (!PreparePath(importPath.parent_path().string())) return false;

	std::vector<Uniform>& uniforms = mat->getUniforms();
	size_t size = uniforms.size();

	for (size_t i = 0; i < size; i++)
	{
		if (uniforms[i].getType() == UniformType::Sampler2D)
		{
			Uniform::SamplerData* sdata = uniforms[i].getPtrData<Uniform::SamplerData>();

			if (sdata->tex_path[0] == '\0')
				continue;

			std::string texpath = sdata->tex_path;

			if (!Resources::Import<Texture>(SetAssetPath(texpath)))
				return false;

			std::filesystem::path p = ImportPathImpl(sdata->tex_path, ".dds");

			memcpy(sdata->tex_path, &p.string()[0], p.string().size() + 1);
		}
	}

	mat->Save(importPath.string());

	LOG(LogType::LOG_INFO, "Material %s imported succesfully!", file.c_str());
	return true;
}
template<>
inline ResourceId Resources::LoadFromLibrary<Material>(const std::string& file)
{
	std::filesystem::path library_file = FindFileInLibrary(file);

	if (library_file.empty())
		return -1;

	std::string file_path = library_file.string();
	StandarizePath(file_path);

	ResourceId position = getResourcePosition(RES_MATERIAL, file_path.c_str());
	size_t size = m_Resources[RES_MATERIAL].size();

	ResourceId resourceId;

	if (position == size) {

		Material* material = new Material(file_path);

		PushResource(RES_MATERIAL, file_path.c_str(), material, true);

		resourceId = size;
	}
	else {
		resourceId = position;
	}

	return resourceId;
}
template<>
inline Material* Resources::GetResourceById<Material>(ResourceId id)
{
	Material* material = NULL;

	if (id >= 0 && id < m_Resources[RES_MATERIAL].size()) {
		material = static_cast<Material*>(m_Resources[RES_MATERIAL][id]->resource);
	}

	return material;
}
template<>
inline bool Resources::CheckImport<Material>(const std::string& file)
{
	return CheckImport(file.c_str(), ".toematerial");
}
template<>
inline const char* Resources::getResourcePathById<Material>(size_t id)
{
	return getPathById(RES_MATERIAL, id);
}