#ifndef __PANEL_PROJECT_H__
#define __PANEL_PROJECT_H__
#pragma once

#include "Panel.h"

#include "..\TheOneEngine\EngineCore.h"

#include <filesystem>
#include <unordered_map>

enum class FileType {
	MODEL3D,
	TEXTURE,
	FOLDER,
	SCRIPT,
	SCENE,
	PREFAB,
	TXT,
	PARTICLES,

	UNKNOWN
};

struct FileInfo {
	std::string name;
	FileType fileType = FileType::UNKNOWN;
	bool isDirectory = false;
	bool isSelected = false;
	std::filesystem::path path;
};

class PanelProject : public Panel
{
public:
	PanelProject(PanelType type, std::string name);
	~PanelProject();

	bool Draw();
	bool CleanUp();

	std::pair<bool, uint32_t> DirectoryTreeViewRecursive(const std::filesystem::path& path, uint32_t* count, int* selection_mask);

	//Function to move files through the folders and some to the scene
	bool DragAndDrop();

	void ContextMenu();

	//Serializes a GameObject into a Prefab file
	void SaveGameObjectAsPrefab(GameObject* gameObject);

	//Function that shows the files of a selected folder (take into account selected Directory in DirectoryTreeViewRecursive)
	void FileExplorerDraw();

	// Function to list files in a directory
	std::vector<FileInfo> ListFiles(const std::string& path);
	FileType FindFileType(const std::string& fileExtension);

	GLuint LoadTexture(const std::string& path);
	void LoadIcons();

	//Loading and Unloading functions for the previews of texture files
	void LoadImagePreviews(const FileInfo& info);
	void UnloadImagePreviews();

	void SaveWarning();

private:

	void DoubleClickFile();

public:
	std::string directoryPath;

private:
	float fontSize;
	bool refresh = true;
	std::vector<FileInfo> files;
	FileInfo* fileSelected;

	std::unordered_map<FileType, GLuint> iconTextures;
	std::unordered_map<std::string, GLuint> imagePreviews;

	bool warningScene = false;
};

#endif // !__PANEL_PROJECT_H__