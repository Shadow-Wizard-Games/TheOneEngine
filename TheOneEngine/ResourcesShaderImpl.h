#pragma once
#include "ResourcesImpl.h"

// SPECIALIZATION FOR SHADER
template<>
inline ResourceId Resources::LoadNative<Shader>(const char* file) {
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
inline ResourceId Resources::Load<Shader>(const char* file) {

	std::filesystem::path library_file = _assetToLibPath(file);
	std::string file_path = library_file.string();
	standarizePath(file_path);

	ResourceId position = getResourcePosition(RES_SHADER, file_path.c_str());
	size_t size = m_Resources[RES_SHADER].size();

	ResourceId resourceId;

	if (position == size) {
		Shader* shader = new Shader();

		shader->LoadFromTOEasset(file_path.c_str());

		//shader->Compile(file);
		PushResource(RES_SHADER, file_path.c_str(), shader);

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
inline void Resources::Import<Shader>(const char* file, Shader* shader)
{
	JSONDocument document;
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
		document.AddMember("vertex", vertexShader->c_str());
	if (fragmentShader)
		document.AddMember("fragment", fragmentShader->c_str());
	if (geometryShader)
		document.AddMember("geometry", geometryShader->c_str());
	JSONValue uniformObj = document.AddMemberObject("uniforms");
	if (!shader->getUniforms().empty())
	{
		std::vector<UniformField>& uniforms = shader->getUniforms();
		for (UniformField& uniform : uniforms)
		{
			uniformObj.AddMember(uniform.name.c_str(), (int)uniform.type);
		}
	}

	delete vertexShader;
	delete fragmentShader;
	delete geometryShader;

	std::filesystem::path import_file = _import_path_impl(file, ".toeshader");
	std::filesystem::path assets_file = file;
	assets_file.replace_extension(".toeshader");

	document.save_file(import_file.string().c_str());
	document.save_file(assets_file.string().c_str());

	LOG(LogType::LOG_INFO, "Shader at %s imported succesfully!", import_file.string().c_str());
}
template<>
inline bool Resources::CheckImport<Shader>(const char* file)
{
	return _check_import_impl(file, ".toeshader");
}
template<>
inline const char* Resources::getResourcePathById<Shader>(size_t id)
{
	return getPathById(RES_SHADER, id);
}