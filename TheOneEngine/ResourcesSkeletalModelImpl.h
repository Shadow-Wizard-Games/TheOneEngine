#pragma once
#include "ResourcesImpl.h"

//--SPECIALIZATION FOR MODEL
template<>
inline std::string Resources::PathToLibrary<SkeletalModel>(const std::string& folderName)
{
	if (!folderName.empty())
		return "Library/Meshes/" + folderName + "/";
	else
		return "Library/Meshes/";
}
template<>
inline bool Resources::Import<SkeletalModel>(const std::string& file, SkeletalModel* model)
{
	std::filesystem::path import_path = file + ".animator";

	model->SetMeshPath(import_path.string());

	PreparePath(import_path.parent_path().string());

	SkeletalModel::SaveMesh(model, import_path.string().c_str());

	LOG(LogType::LOG_INFO, "Model at %s imported succesfully!", import_path.string().c_str());
	return true;
}
template<>
inline ResourceId Resources::Load<SkeletalModel>(const std::string& file)
{
	std::filesystem::path file_path = FindFileInAssets(file);

	if (file_path.empty())
		return -1;


	ResourceId position = getResourcePosition(RES_SKELETALMODEL, file_path.string().c_str());
	size_t size = m_Resources[RES_SKELETALMODEL].size();

	ResourceId resourceId;

	if (position == size) {
		SkeletalModel* model = new SkeletalModel(file_path.string());

		PushResource(RES_SKELETALMODEL, file_path.string().c_str(), model, true);

		resourceId = size;
	}
	else {
		resourceId = position;
	}

	return resourceId;
}
template<>
inline ResourceId Resources::LoadFromLibrary<SkeletalModel>(const std::string& file)
{
	std::filesystem::path file_path = FindFileInLibrary(file);

	if (file_path.empty())
		return -1;


	ResourceId position = getResourcePosition(RES_SKELETALMODEL, file_path.string().c_str());
	size_t size = m_Resources[RES_SKELETALMODEL].size();

	ResourceId resourceId;

	if (position == size) {
		SkeletalModel* model = new SkeletalModel(file_path.string());

		PushResource(RES_SKELETALMODEL, file_path.string().c_str(), model, true);

		resourceId = size;
	}
	else {
		resourceId = position;
	}

	return resourceId;
}
template<>
inline SkeletalModel* Resources::GetResourceById<SkeletalModel>(ResourceId id)
{
	SkeletalModel* model = NULL;

	if (id >= 0 && id < m_Resources[RES_SKELETALMODEL].size()) {
		model = static_cast<SkeletalModel*>(m_Resources[RES_SKELETALMODEL][id]->resource);
	}

	return model;
}

template<>
inline const char* Resources::getResourcePathById<SkeletalModel>(size_t id)
{
	return getPathById(RES_SKELETALMODEL, id);
}