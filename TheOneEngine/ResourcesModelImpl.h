#pragma once
#include "ResourcesImpl.h"

//--SPECIALIZATION FOR MODEL
template<>
inline std::string Resources::PathToLibrary<Model>(const std::string& folderName)
{
	if (!folderName.empty())
		return "Library/Meshes/" + folderName + "/";
	else
		return "Library/Meshes/";
}
template<>
inline bool Resources::Import<Model>(const std::string& file, Model* model)
{
	std::filesystem::path import_path = file;
	import_path = ImportPathImpl(import_path, ".mesh");

	Model::SaveMesh(model, import_path.string().c_str());

	LOG(LogType::LOG_INFO, "Model at %s imported succesfully!", import_path.string().c_str());
	return true;
}
template<>
inline bool Resources::CheckImport<Model>(const std::string& file)
{
	return CheckImport(file.c_str(), ".mesh");
}
template<>
inline ResourceId Resources::Load<Model>(const std::string& file)
{
	ResourceId position = getResourcePosition(RES_MODEL, file.c_str());
	size_t size = m_Resources[RES_MODEL].size();

	ResourceId resourceId;

	if (position == size) 
	{
		std::vector<Model*> meshes = Model::LoadMeshes(file);

		for (auto& mesh : meshes)
		{
			PushResource(RES_MODEL, file.c_str(), mesh);
		}

		resourceId = size;
	}
	else 
	{
		resourceId = position;
	}

	return resourceId;
}
template<>
inline ResourceId Resources::LoadFromLibrary<Model>(const std::string& file)
{
	std::filesystem::path file_path = FindFileInLibrary(file);

	if (file_path.empty())
		return -1;

	file_path.replace_extension(".mesh");
	ResourceId position = getResourcePosition(RES_MODEL, file_path.string().c_str());
	size_t size = m_Resources[RES_MODEL].size();

	ResourceId resourceId;

	if (position == size) {
		Model* model = new Model(file_path.string());

		PushResource(RES_MODEL, file_path.string().c_str(), model, true);

		resourceId = size;
	}
	else {
		resourceId = position;
	}

	return resourceId;
}
template<>
inline Model* Resources::GetResourceById<Model>(ResourceId id)
{
	Model* model = NULL;

	if (id >= 0 && id < m_Resources[RES_MODEL].size()) {
		model = static_cast<Model*>(m_Resources[RES_MODEL][id]->resource);
	}

	return model;
}

template<>
inline const char* Resources::getResourcePathById<Model>(size_t id)
{
	return getPathById(RES_MODEL, id);
}