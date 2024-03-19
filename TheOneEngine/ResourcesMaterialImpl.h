#pragma once
#include "ResourcesImpl.h"

//--SPECIALIZATION FOR MATERIAL
template<>
inline ResourceId Resources::Load<Material>(const std::string& file)
{
	if (!std::filesystem::exists(file))
	{
		return -1;
	}
	std::filesystem::path library_file = _assetToLibPath(file);

	std::string file_path = library_file.string();
	standarizePath(file_path);

	ResourceId position = getResourcePosition(RES_MATERIAL, file_path.c_str());
	size_t size = m_Resources[RES_MATERIAL].size();

	ResourceId resourceId;

	if (position == size) {

		Material* material = new Material(file_path);

		PushResource(RES_MATERIAL, file_path.c_str(), material);

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
inline void Resources::Import<Material>(const std::string& file)
{
	std::filesystem::path import_file = file;
	std::filesystem::path export_file = _assetToLibPath(file);
	export_file.replace_extension(".toematerial");

	std::filesystem::path export_path = export_file.parent_path();

	if (_preparePath(export_path.string())) {
		std::ifstream source(file, std::ios::binary);
		std::ofstream dest(export_file.c_str(), std::ios::binary);

		dest << source.rdbuf();

		source.close();
		dest.close();
	}

	LOG(LogType::LOG_INFO, "Material at %s imported succesfully!", import_file.string().c_str());
}
template<>
inline bool Resources::CheckImport<Material>(const std::string& file)
{
	return _check_import_impl(file.c_str(), ".toematerial");
}
template<>
inline const char* Resources::getResourcePathById<Material>(size_t id)
{
	return getPathById(RES_MATERIAL, id);
}