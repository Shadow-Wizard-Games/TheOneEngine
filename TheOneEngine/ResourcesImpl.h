#pragma once
#pragma warning(disable : 4251)

// Resources
#include "Shader.h"
#include "Texture.h"
#include "Model.h"
#include "Material.h"

#include "Defs.h"
#include "Log.h"
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <time.h>

namespace fs = std::filesystem;

#define RES_NOT_LOADED -1

typedef int ResourceId;

class Resources
{
public:
	// Enum that indicates all the resources that can be loaded by the engine
	enum ResourceType {
		RES_IMAGE,
		RES_MODEL,
		RES_SHADER,
		RES_MATERIAL,
		RES_LAST
	};

	struct Resource {
		// Path to resource
		std::string filePath;
		bool isNative;
		// Any type
		void* resource;

		Resource() = default;
	};

private:
	Resources();

	static std::vector<Resource*> m_Resources[RES_LAST];

	static void PushResource(ResourceType rt, const char* file, void* rsc, bool isNative = false);
	static ResourceId getResourcePosition(ResourceType rt, const char* file);

	// Resource path for importing
	inline static const char* getPathById(ResourceType type, size_t id)
	{
		if (id >= m_Resources[type].size())
			return "";

		return m_Resources[type][id]->filePath.c_str();
	}


	static bool CheckImport(const char* file, const char* extension);


	static void _import_image_impl(const char* origin, const char* destination);
public:
	// Checks if path exists and if not creates folders
	static bool PreparePath(std::string path); 
	static std::string AssetToLibPath(std::string path);
	static std::string SetAssetPath(std::string path);
	static std::filesystem::path ImportPathImpl(const std::filesystem::path& path, const char* extension);
	static std::string FindFileInAssets(const std::string& name);
	static std::string FindFileInLibrary(const std::string& name);
	static std::vector<std::string> GetAllFilesFromFolder(const std::string& path);
	
	static inline void StandarizePath(std::string& file_path, bool toLower = false);


	static void ToLowerCase(std::string& path);
	inline static std::vector<Resource*>& GetResourcesOf(ResourceType rt) { return m_Resources[rt]; }

	static json OpenJSON(const std::string& filename);
	static void SaveJSON(const std::string& path, json toSave);

	template<class T> static const char* getResourcePathById(size_t id);

	// Load from fbx, png....
	template<class T> static ResourceId Load(const std::string& file);
	template<class T> static std::vector<ResourceId> LoadMultiple(const std::string& file);
	// Load from already imported resources
	template<class T> static ResourceId LoadFromLibrary(const std::string& file);
	template<class T> static T* GetResourceById(ResourceId id);
	template<class T> static T* GetResourceByPath(std::string filename);
	template<class T> static bool CheckImport(const std::string& file);
	template<class T, class... T2> static bool Import(const std::string& file, T2... settings);
	template<class T> static std::string PathToLibrary(const std::string& folderName = std::string());

	static void Clear();

};
