#pragma once
#include "ResourcesImpl.h"

template<>
inline ResourceId Resources::Load<Texture>(const char* file)
{
	std::filesystem::path file_path = _assetToLibPath(file);
	file_path.replace_extension(".dds");
	ResourceId position = getResourcePosition(RES_IMAGE, file_path.string().c_str());
	size_t size = m_Resources[RES_IMAGE].size();

	ResourceId resourceId;

	if (position == size) {
		Texture* image = new Texture(file_path.string().c_str(), true);

		PushResource(RES_IMAGE, file_path.string().c_str(), image);

		resourceId = size;
	}
	else {
		resourceId = position;
	}

	return resourceId;
}
template<>
inline ResourceId Resources::LoadNative<Texture>(const char* file)
{
	std::string path = file;
	standarizePath(path);
	ResourceId position = getResourcePosition(RES_IMAGE, path.c_str());
	size_t size = m_Resources[RES_IMAGE].size();
	ResourceId resourceId;

	if (position == size) {
		Texture* image = new Texture(path.c_str(), false);

		PushResource(RES_IMAGE, path.c_str(), image, true);

		resourceId = size;
	}
	else {
		resourceId = position;
	}

	return resourceId;
}
template<>
inline Texture* Resources::GetResourceById<Texture>(ResourceId id)
{
	Texture* image = NULL;

	if (id >= 0 && id < m_Resources[RES_IMAGE].size()) {
		image = static_cast<Texture*>(m_Resources[RES_IMAGE][id]->resource);
	}

	return image;
}

template<>
inline void Resources::Import<Texture>(const char* file)
{
	std::filesystem::path import_path = _import_path_impl(file, ".dds");
	_import_image_impl(file, import_path.string().c_str());

	LOG(LogType::LOG_INFO, "Image at %s imported succesfully!", import_path.string().c_str());
}
template<>
inline bool Resources::CheckImport<Texture>(const char* file)
{
	return _check_import_impl(file, ".dds");
}
template<>
inline const char* Resources::getResourcePathById<Texture>(size_t id)
{
	return getPathById(RES_IMAGE, id);
}