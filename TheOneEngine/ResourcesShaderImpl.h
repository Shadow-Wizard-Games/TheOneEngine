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
inline ResourceId Resources::Load<Shader>(const std::string& file) {
	ResourceId position = getResourcePosition(RES_SHADER, file);
	size_t size = m_Resources[RES_SHADER].size();

	ResourceId resourceId;

	if (position == size) {
		Shader* shader = new Shader();

		shader->Init(file);
		PushResource(RES_SHADER, file, shader);

		resourceId = size;
	}
	else {
		resourceId = position;
	}

	return resourceId;
}
template<>
inline ResourceId Resources::LoadFromLibrary<Shader>(const std::string& file) {

	std::filesystem::path library_file = AssetToLibPath(file);
	std::string file_path = library_file.string();
	StandarizePath(file_path);

	ResourceId position = getResourcePosition(RES_SHADER, file_path.c_str());
	size_t size = m_Resources[RES_SHADER].size();

	ResourceId resourceId;

	if (position == size) {
		Shader* shader = new Shader();

		shader->LoadFromTOEasset(file_path.c_str());

		//shader->Compile(file);
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
inline void Resources::Import<Shader>(const std::string& file, Shader* shader)
{
	json document;
	std::string filePath = file;
	filePath += ".vs";
	std::string* vertexShader = getFileData(filePath.c_str());

	filePath = file;
	filePath += ".fs";
	std::string* fragmentShader = getFileData(filePath.c_str());

	filePath = file;
	filePath += ".gs";
	std::string* geometryShader = getFileData(filePath.c_str());

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
			uniformsJSON["Name"] = uniform.name.c_str();
			uniformsJSON["Type"] = (int)uniform.type;
		}
	}

	delete vertexShader;
	delete fragmentShader;
	delete geometryShader;

	std::filesystem::path import_file = ImportPathImpl(file, ".toeshader");

	document.save_file(import_file.string().c_str());

	LOG(LogType::LOG_INFO, "Shader at %s imported succesfully!", import_file.string().c_str());
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