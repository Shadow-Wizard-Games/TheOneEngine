#include "ResourcesImpl.h"

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
	StandarizePath(path);

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
	if (name.find("Assets") && fs::exists(name))
		return name;

	for (const auto& p : fs::recursive_directory_iterator("Assets")) {
		if (p.path().filename() == name)
			return p.path().string();
	}

	LOG(LogType::LOG_ERROR, "Wrong Path");
	return std::string();
}

std::string Resources::FindFileInLibrary(const std::string& name)
{
	if (name.find("Library") && fs::exists(name))
		return name;

	for (const auto& p : fs::recursive_directory_iterator("Library")) {
		if (p.path().filename() == name)
			return p.path().string();
	}

	LOG(LogType::LOG_ERROR, "Wrong Path");
	return std::string();
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