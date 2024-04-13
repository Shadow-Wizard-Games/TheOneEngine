#pragma once
#include "ResourcesImpl.h"


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

// SPECIALIZATION FOR SHADER
template<>
inline std::string Resources::PathToLibrary<Shader>(const std::string& folderName)
{
	if (!folderName.empty())
		return "Library/Shaders/" + folderName + "/";
	else
		return "Library/Shaders/";
}
template<>
inline bool Resources::Import<Shader>(const std::string& file, Shader* shader)
{
	json document;

	std::filesystem::path mainPath = FindFileInAssets(file);
	std::string filePath = mainPath.replace_extension("").string();
	if (filePath.empty())
		return false;

	std::string vertexPath = filePath + ".vs";
	std::string* vertexShader = getFileData(vertexPath.c_str());

	std::string fragmentPath = filePath + ".fs";
	std::string* fragmentShader = getFileData(fragmentPath.c_str());

	std::string geometryPath = filePath + ".gs";
	std::string* geometryShader = getFileData(geometryPath.c_str());

	if (vertexShader)
		document["vertex"] = vertexShader->c_str();
	if (fragmentShader)
		document["fragment"] = fragmentShader->c_str();
	if (geometryShader)
		document["geometry"] = geometryShader->c_str();
	json uniformsJSON;
	if (!shader->getUniforms().empty())
	{
		std::vector<UniformField>& uniforms = shader->getUniforms();
		for (UniformField& uniform : uniforms)
		{
			json uJSON;
			uJSON["Name"] = uniform.name.c_str();
			uJSON["Type"] = (int)uniform.type;

			uniformsJSON.push_back(uJSON);
		}
	}

	document["uniforms"] = uniformsJSON;

	delete vertexShader;
	delete fragmentShader;
	delete geometryShader;

	std::string importName = mainPath.filename().replace_extension("").string();
	std::filesystem::path import_file = PathToLibrary<Shader>() + importName + ".toeshader";
	PreparePath(import_file.parent_path().string());

	SaveJSON(import_file.string(), document);

	LOG(LogType::LOG_INFO, "Shader at %s imported succesfully!", import_file.c_str());

	return true;
}
template<>
inline ResourceId Resources::Load<Shader>(const std::string& file) {
	ResourceId position = getResourcePosition(RES_SHADER, file.c_str());
	size_t size = m_Resources[RES_SHADER].size();

	ResourceId resourceId;

	if (position == size) {
		Shader* shader = new Shader();

		shader->Init(file);

		PushResource(RES_SHADER, file.c_str(), shader);

		resourceId = size;
	}
	else {
		resourceId = position;
	}

	return resourceId;
}
template<>
inline ResourceId Resources::LoadFromLibrary<Shader>(const std::string& file) {

	std::filesystem::path library_file = FindFileInLibrary(file);

	if (library_file.empty())
		return -1;

	std::string file_path = library_file.string();
	StandarizePath(file_path);

	ResourceId position = getResourcePosition(RES_SHADER, file_path.c_str());
	size_t size = m_Resources[RES_SHADER].size();

	ResourceId resourceId;

	if (position == size) {
		Shader* shader = new Shader();

		shader->LoadFromTOEasset(file_path.c_str());

		PushResource(RES_SHADER, file_path.c_str(), shader, true);

		resourceId = size;
	}
	else {
		resourceId = position;
	}

	return resourceId;
}


template<>
inline Shader* Resources::GetResourceById<Shader>(ResourceId id) {
	Shader* resource = NULL;

	if (id >= 0 && id < m_Resources[RES_SHADER].size()) {
		resource = static_cast<Shader*>(m_Resources[RES_SHADER][id]->resource);
	}

	return resource;
}
template<>
inline bool Resources::CheckImport<Shader>(const std::string& file)
{
	return CheckImport(file.c_str(), ".toeshader");
}
template<>
inline const char* Resources::getResourcePathById<Shader>(size_t id)
{
	return getPathById(RES_SHADER, id);
}