#pragma once
#include "ResourcesImpl.h"

//template<>
//inline std::string Resources::PathToLibrary<Font>(const std::string& folderName)
//{
//	if (!folderName.empty())
//		return "Library/Fonts/" + folderName + "/";
//	else
//		return "Library/Fonts/";
//}
//
//template<>
//inline bool Resources::Import<Font>(const std::string& file, Font* font)
//{
//	if (std::filesystem::is_directory(file)) return false;
//	if (!std::filesystem::exists(file)) return false;
//
//	std::filesystem::path origin_path = FindFileInAssets(file);
//	if (origin_path.string().empty())
//	{
//		origin_path = file;
//	}
//	std::filesystem::path import_path = PathToLibrary<Font>() + origin_path.filename().replace_extension(".dds").string();
//	PreparePath(import_path.parent_path().string());
//	if (!file.ends_with(".dds"))
//	{
//		_import_image_impl(origin_path.string().c_str(), import_path.string().c_str());
//	}
//
//	LOG(LogType::LOG_INFO, "Image at %s imported succesfully!", import_path.string().c_str());
//
//	return true;
//}
//template<>
//inline ResourceId Resources::LoadFromLibrary<Font>(const std::string& file)
//{
//	std::filesystem::path file_path = FindFileInLibrary(file);
//
//	if (file_path.empty())
//		return -1;
//
//	file_path.replace_extension(".dds");
//	ResourceId position = getResourcePosition(RES_FONT, file_path.string().c_str());
//	size_t size = m_Resources[RES_FONT].size();
//
//	ResourceId resourceId;
//
//	if (position == size) {
//		Texture* image = new Texture(file_path.string().c_str());
//
//		PushResource(RES_FONT, file_path.string().c_str(), image, true);
//
//		resourceId = size;
//	}
//	else {
//		resourceId = position;
//	}
//
//	return resourceId;
//}
template<>
inline ResourceId Resources::Load<Font>(const std::string& file)
{
	std::string path = FindFileInAssets(file);
	StandarizePath(path);
	ResourceId position = getResourcePosition(RES_FONT, path.c_str());
	size_t size = m_Resources[RES_FONT].size();
	ResourceId resourceId;

	if (position == size) {
		Font* font = new Font(path.c_str());

		PushResource(RES_FONT, path.c_str(), font);

		resourceId = size;
	}
	else {
		resourceId = position;
	}

	return resourceId;
}
template<>
inline Font* Resources::GetResourceById<Font>(ResourceId id)
{
	Font* image = NULL;

	if (id >= 0 && id < m_Resources[RES_FONT].size()) {
		image = static_cast<Font*>(m_Resources[RES_FONT][id]->resource);
	}

	return image;
}
template<>
inline bool Resources::CheckImport<Font>(const std::string& file)
{
	return CheckImport(file.c_str(), ".ttf");
}
template<>
inline const char* Resources::getResourcePathById<Font>(size_t id)
{
	return getPathById(RES_FONT, id);
}