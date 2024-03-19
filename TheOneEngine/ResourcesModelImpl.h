#pragma once
#include "ResourcesImpl.h"

//--SPECIALIZATION FOR MODEL
template<>
inline ResourceId Resources::LoadNative<Model>(const std::string& file)
{
	ResourceId position = getResourcePosition(RES_MODEL, file);
	size_t size = m_Resources[RES_MODEL].size();

	ResourceId resourceId;

	if (position == size) {
		Model* model = new Model(file);

		PushResource(RES_MODEL, file, model, true);

		resourceId = size;
	}
	else {
		resourceId = position;
	}

	return resourceId;
}
template<>
inline ResourceId Resources::Load<Model>(const std::string& file)
{
	std::filesystem::path file_path = _assetToLibPath(file);
	file_path.replace_extension(".toemodel");
	ResourceId position = getResourcePosition(RES_MODEL, file_path.string().c_str());
	size_t size = m_Resources[RES_MODEL].size();

	ResourceId resourceId;

	if (position == size) {
		Model* model = new Model(NULL);

		model->LoadTOEMesh(file_path.string().c_str());

		PushResource(RES_MODEL, file_path.string().c_str(), model);

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
inline void Resources::Import<Model>(const std::string& file)
{
	std::filesystem::path import_path = file;
	import_path = _import_path_impl(import_path, ".toemodel");


	_import_model_impl(file.c_str(), import_path.string().c_str());
	LOG(LogType::LOG_INFO, "Model at %s imported succesfully!", import_path.string().c_str());
}
template<>
inline bool Resources::CheckImport<Model>(const std::string& file)
{
	return _check_import_impl(file.c_str(), ".toemodel");
}

template<>
inline const char* Resources::getResourcePathById<Model>(size_t id)
{
	return getPathById(RES_MODEL, id);
}