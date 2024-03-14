#pragma once
#include "ResourcesImpl.h"

template<>
inline ResourceId Resources::Load<Texture2D>(const char* file)
{
	std::filesystem::path file_path = _assetToLibPath(file);
	file_path.replace_extension(".dds");
	ResourceId position = getResourcePosition(RES_IMAGE, file_path.string().c_str());
	size_t size = m_Resources[RES_IMAGE].size();

	ResourceId resourceId;

	if (position == size) {
		Texture2D* image = new Texture2D(file_path.string().c_str(), true);

		PushResource(RES_IMAGE, file_path.string().c_str(), image);

		resourceId = size;
	}
	else {
		resourceId = position;
	}

	return resourceId;
}
template<>
inline ResourceId Resources::LoadNative<Texture2D>(const char* file)
{
	std::string path = file;
	standarizePath(path);
	ResourceId position = getResourcePosition(RES_IMAGE, path.c_str());
	size_t size = m_Resources[RES_IMAGE].size();
	ImageSettings* settings = new ImageSettings();
	ResourceId resourceId;

	if (position == size) {
		Texture2D* image = new Texture2D(path.c_str(), false);

		PushResource(RES_IMAGE, path.c_str(), image, true);

		resourceId = size;
	}
	else {
		resourceId = position;
	}

	return resourceId;
}
template<>
inline Texture2D* Resources::GetResourceById<Texture2D>(ResourceId id)
{
	Texture2D* image = NULL;

	if (id >= 0 && id < m_Resources[RES_IMAGE].size()) {
		image = static_cast<Texture2D*>(m_Resources[RES_IMAGE][id]->resource);
	}

	return image;
}

template<>
inline void Resources::Import<Texture2D>(const char* file)
{
	std::filesystem::path import_path = _import_path_impl(file, ".dds");
	_import_image_impl(file, import_path.string().c_str());

	SMOL_CORE_INFO("Image at {} imported succesfully!", import_path.string().c_str());
}
template<>
inline bool Resources::CheckImport<Texture2D>(const char* file)
{
	return _check_import_impl(file, ".dds");
}
template<>
inline const char* Resources::getResourcePathById<Texture2D>(size_t id)
{
	return getPathById(RES_IMAGE, id);
}