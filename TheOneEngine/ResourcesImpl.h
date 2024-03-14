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

#define RES_NOT_LOADED -1

typedef size_t ResourceId;

class Resources
{
public:
	// Enum that indicates all the resources that can be loaded by the engine
	enum ResourceType {
		RES_IMAGE,
		RES_MODEL,
		RES_LAST
	};

	enum MetaResult
	{
		NOTFOUND,
		DELETED,
		UPDATED,
		TOUPDATE
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


	// Implementations
	static void _import_image_impl(const char* origin, const char* destination);
	static void _import_model_impl(const char* origin, const char* destination);
	static bool _check_import_impl(const char* file, const char* extension);

public:
	static bool _preparePath(std::string path);
	static std::string _assetToLibPath(std::string path);
	static std::filesystem::path _import_path_impl(const std::filesystem::path& path, const char* extension);
	template <typename TP>
	static inline std::time_t to_time_t(TP tp)
	{
		using namespace std::chrono;
		auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
			+ system_clock::now());
		return system_clock::to_time_t(sctp);
	}
	static inline void standarizePath(std::string& file_path)
	{
		size_t index = 0;

		while ((index = file_path.find('\\', index)) != std::string::npos) {
			file_path.replace(index, 1, "/");
			index++;
		}
		_toLower(file_path);
	}


	static void _toLower(std::string& path);
	inline static std::vector<Resource*>& GetResourcesOf(ResourceType rt) { return m_Resources[rt]; }
	static void UnloadSceneResources();

	// Returns the state of the meta file
	// Deleted: the meta file is deleted due to the reference file not existing
	// Updated: the meta files are up to date
	// ToUpdate: the meta last updated doesnt equal the file reference
	// Not found: there's no meta file for the reference
	static MetaResult CheckMeta(const char* filename);
	static void UpdateMeta(const char* filename);

	template<class T> static const char* getResourcePathById(size_t id);

	template<class T> static ResourceId Load(const char* file);
	template<class T> static ResourceId LoadNative(const char* file);
	template<class T> static T* GetResourceById(ResourceId id);
	template<class T> static bool CheckImport(const char* file);
	template<class T, class... T2> static void Import(const char* file, T2... settings);
	template<class T, class... T2> static void CreateMeta(const char* file, T2... settings);
	template<class T, class... T2> static void LoadMeta(const char* file, T2... settings);

	static void SetAssetPath(std::string& path);
	inline static std::string* getFileData(const char* file)
	{
		std::fstream shaderFile;

		shaderFile.open(file, std::ios::in);

		if (!shaderFile.is_open()) return NULL;

		std::stringstream buffer;

		buffer << shaderFile.rdbuf();

		shaderFile.close();

		return new std::string(buffer.str());
	}
	static void Clear();

};