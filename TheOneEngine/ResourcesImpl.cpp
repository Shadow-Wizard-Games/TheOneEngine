#include "ResourcesImpl.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "ResourcesImageImpl.h"
#include "ResourcesMaterialImpl.h"
#include "ResourcesModelImpl.h"
#include "ResourcesShaderImpl.h"

std::vector<Resources::Resource*> Resources::m_Resources[Resources::RES_LAST];

Resources::Resources()
{
}

void Resources::PushResource(ResourceType rt, const char* file, void* rsc, bool isNative)
{
	Resource* resource = new Resource();
	resource->isNative = isNative;
	resource->filePath = file;
	resource->resource = rsc;
	resource->name = std::filesystem::path(file).filename().replace_extension().string();

	m_Resources[rt].push_back(resource);

	std::string message = "Loaded resource \"" + resource->filePath + "\" successfully.";

	LOG(LogType::LOG_INFO, message.c_str());
}


ResourceId Resources::getResourcePosition(ResourceType rt, const char* file)
{
	size_t size = m_Resources[rt].size();

	ResourceId resourceId = size;

	for (size_t i = 0; i < size; i++) {
		if (m_Resources[rt][i]->filePath == file) {
			resourceId = i;
			break;
		}
	}

	return resourceId;
}

std::string Resources::AssetToLibPath(std::string path)
{
	StandarizePath(path);
	size_t a_ind = path.find("Assets");

	if (a_ind != path.npos) {
		path.replace(a_ind, 6, "Library");
	}
	else //resources path impl
	{
		if (path.find("Library") != path.npos)
			return path;
		std::string res_path = "Library/";
		res_path += path;
		path = res_path;
	}
	return path;
}

std::string Resources::SetAssetPath(std::string path)
{
	//StandarizePath(path);

	size_t a_ind = path.find("library");

	if (a_ind != path.npos) {
		path.replace(a_ind, 7, "assets");
	}

	return path;
}

std::filesystem::path Resources::ImportPathImpl(const std::filesystem::path& path, const char* extension)
{
	std::string pathFile = AssetToLibPath(path.string());
	std::filesystem::path importPath = pathFile;
	PreparePath(importPath.parent_path().string());
	importPath.replace_extension(extension);
	return importPath;
}

std::string Resources::FindFileInAssets(const std::string& name)
{
	std::string standarizedPath = name;
	StandarizePath(standarizedPath);

	if (name.find("Assets") && std::filesystem::exists(name))
		return name;

	for (const auto& p : fs::recursive_directory_iterator("Assets")) 
	{
		std::string relPath = p.path().string();
		std::string fileName = p.path().filename().replace_extension("").string();
		StandarizePath(relPath);
		if (fileName == name || relPath == standarizedPath)
		{
			// Parche pocho pero esque estoy hasta los c* de los .fbm
			if(relPath.ends_with(".fbm"))
			{
				std::filesystem::path finalPath = relPath;
				return finalPath.replace_extension(".fbx").string();
			}
			return relPath;
		}
	}

	return std::string();
}

std::string Resources::FindFileInLibrary(const std::string& name)
{
	if (name.find("Library") && std::filesystem::exists(name))
		return name;

	for (const auto& p : fs::recursive_directory_iterator("Library"))
	{
		std::string relPath = p.path().string();
		std::string fileName = p.path().filename().replace_extension("").string();
		StandarizePath(relPath);
		if (fileName == name || relPath == name)
			return relPath;
	}

	return std::string();
}

std::vector<std::string> Resources::GetAllFilesFromFolder(const std::string& path, const std::string& extension1, const std::string& extension2)
{
	bool compareExtension = false;
	if (!extension1.empty())
		compareExtension = true;

	std::vector<std::string> fileNames;

	if (fs::is_directory(path))
	{
		for (const auto& entry : fs::directory_iterator(path)) {
			if (fs::is_regular_file(entry)) 
			{
				if (compareExtension)
				{
					if (entry.path().extension().string() == extension1 || entry.path().extension().string() == extension2)
					{
						std::string path = entry.path().filename().string();
						LOG(LogType::LOG_WARNING, "- %s is in", path.data());
						fileNames.push_back(entry.path().string());
					}
					else
						continue;
				}
				else
				{
					std::string path = entry.path().filename().string();
					LOG(LogType::LOG_WARNING, "- %s is in", path.data());
					fileNames.push_back(entry.path().string());
				}
			}
		}
	}
	return fileNames;
}

inline void Resources::StandarizePath(std::string& file_path, bool toLower)
{
	size_t index = 0;

	while ((index = file_path.find('\\', index)) != std::string::npos) {
		file_path.replace(index, 1, "/");
		index++;
	}
	if (toLower)
		ToLowerCase(file_path);
}

bool Resources::PreparePath(std::string path)
{
	if (!std::filesystem::exists(path)) {
		if (!std::filesystem::create_directories(path)) {
			LOG(LogType::LOG_ERROR, "Couldn't create directory {}", path.c_str());
			return false;
		}
	}

	return true;
}

void Resources::ToLowerCase(std::string& path)
{
	size_t len = path.size();

	for (size_t i = 0; i < len; i++) {
		if (path[i] >= 'A' && path[i] <= 'Z') path[i] -= ('Z' - 'z');
	}
}


json Resources::OpenJSON(const std::string& filename)
{
	if (!fs::exists(filename))
	{
		LOG(LogType::LOG_ERROR, "JSON file does not exist: {}", filename.data());
		return NULL;
	}

	// Read the scene JSON from the file
	std::ifstream file(filename);
	if (!file.is_open())
	{
		LOG(LogType::LOG_ERROR, "Failed to open JSON file: {}", filename.data());
		return NULL;
	}

	json tempJSON;
	try
	{
		file >> tempJSON;
	}
	catch (const json::parse_error& e)
	{
		LOG(LogType::LOG_ERROR, "Failed to parse JSON: {}", e.what());
		return NULL;
	}

	// Close the file
	file.close();
	return tempJSON;
}

void Resources::SaveJSON(const std::string& path, json toSave)
{
	std::ofstream(path) << toSave.dump(2);
}

bool Resources::CheckImport(const char* file, const char* extension)
{
	std::filesystem::path fileFS = FindFileInLibrary(file);
	fileFS.replace_extension(extension);
	return std::filesystem::exists(fileFS);
}

void Resources::_import_image_impl(const char* origin, const char* destination)
{
	int w, h, ch;

	// STBI_rgb_alpha loads image as 32 bpp (4 channels), ch = image origin channels
	unsigned char* image = stbi_load(origin, &w, &h, &ch, STBI_rgb_alpha);
	if (w != h)
	{
		LOG(LogType::LOG_ERROR, "Image at %s needs to be square in order to be imported", origin);
		return;
	}

	Texture::raw_to_dds_file(destination, image, w, h, 32);

	stbi_image_free(image);
}

void Resources::Clear()
{
	for (int i = 0; i < RES_LAST; i++) {
		size_t rsize = m_Resources[i].size();

		for (size_t j = 0; j < rsize; j++) {
			delete m_Resources[i][j]->resource;
			delete m_Resources[i][j];
			j--;
		}
	}
}

template<class T>
inline std::string Resources::PathToLibrary(const std::string& folderName)
{
	return std::string();
}