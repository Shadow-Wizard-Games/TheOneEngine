#pragma once
#include "ResourcesImpl.h"

#include "stb_image.h"
#include "Log.h"

template<>
inline ResourceId Resources::LoadFromLibrary<Texture>(const std::string& file)
{
	std::filesystem::path file_path = AssetToLibPath(file);
	file_path.replace_extension(".dds");
	ResourceId position = getResourcePosition(RES_IMAGE, file_path.string().c_str());
	size_t size = m_Resources[RES_IMAGE].size();

	ResourceId resourceId;

	if (position == size) {
		Texture* image = new Texture(file_path.string().c_str(), true);

		PushResource(RES_IMAGE, file_path.string().c_str(), image, true);

		resourceId = size;
	}
	else {
		resourceId = position;
	}

	return resourceId;
}
template<>
inline ResourceId Resources::Load<Texture>(const std::string& file)
{
	std::string path = file;
	StandarizePath(path);
	ResourceId position = getResourcePosition(RES_IMAGE, path.c_str());
	size_t size = m_Resources[RES_IMAGE].size();
	ResourceId resourceId;

	if (position == size) {
		Texture* image = new Texture(path.c_str(), false);

		PushResource(RES_IMAGE, path.c_str(), image);

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
inline void Resources::Import<Texture>(const std::string& file)
{
	std::filesystem::path import_path = ImportPathImpl(file, ".dds");

	int w, h, ch;

	// STBI_rgb_alpha loads image as 32 bpp (4 channels), ch = image origin channels
	unsigned char* image = stbi_load(file.c_str(), &w, &h, &ch, STBI_rgb_alpha);
	if (w != h)
	{
		LOG(LogType::LOG_ERROR, "Image at %s needs to be square in order to be imported", file.c_str());
		return;
	}
	Texture::raw_to_dds_file(import_path.string().c_str(), image, w, h, 32);

	stbi_image_free(image);

	LOG(LogType::LOG_INFO, "Image at %s imported succesfully!", import_path.string().c_str());
}
template<>
inline bool Resources::CheckImport<Texture>(const std::string& file)
{
	return CheckImport(file.c_str(), ".dds");
}
template<>
inline const char* Resources::getResourcePathById<Texture>(size_t id)
{
	return getPathById(RES_IMAGE, id);
}