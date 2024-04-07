#include "PanelProject.h"
#include "App.h"
#include "Gui.h"
#include "imgui.h"
#include "SceneManager.h"

#include "..\TheOneEngine\UIDGen.h"

#include <IL/il.h>

#include <fstream>
#include <filesystem>
#include <string>
#include <locale>
#include <codecvt>

#define BIT(x) (1 << x)

namespace fs = std::filesystem;

PanelProject::PanelProject(PanelType type, std::string name) : Panel(type, name)
{
	LoadIcons();
	directoryPath = ASSETS_PATH;
	fontSize = 50.0f;
}

PanelProject::~PanelProject() {}


bool PanelProject::Draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });

	if (ImGui::Begin("Project"))
	{
		int width = ImGui::GetContentRegionAvail().x;

		static ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable;

		if (ImGui::BeginTable("table", 2, tableFlags))
		{
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 100.0f);

			// Directory Tree View ----------------------------
			ImGui::TableNextColumn();
			ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;
			if (ImGui::TreeNodeEx("Assets", base_flags))
			{
				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
				{
					directoryPath = ASSETS_PATH;
					refresh = true;
				}

				uint32_t count = 0;
				for (const auto& entry : fs::recursive_directory_iterator(ASSETS_PATH))
					count++;

				static int selection_mask = 0;

				auto clickState = DirectoryTreeViewRecursive(ASSETS_PATH, &count, &selection_mask);

				if (clickState.first)
				{
					// Update selection state
					// (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
					if (ImGui::GetIO().KeyCtrl)
						selection_mask ^= BIT(clickState.second);		// CTRL+click to toggle
					else //if (!(selection_mask & (1 << clickState.second))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
						selection_mask = BIT(clickState.second);		// Click to single-select
				}
			}
			
			// Inspector ----------------------------
			ImGui::TableNextColumn();

			//Historn: NEED TO CREATE CHILD WINDOW FOR DRAG AND DROP
			//if(ImGui::BeginChild("FileExplorer"))
			if (fileSelected)
			{
				ImGui::Text("%s", fileSelected->path.string().c_str());
			}

			ImGui::Separator();

			FileExplorerDraw();

			ImGui::EndTable();
		}

		SaveWarning();
	}

	ImGui::PopStyleVar();
	ImGui::End();
	return true;
}

bool PanelProject::CleanUp()
{
	// Clean up icon textures
	for (auto& pair : iconTextures) {
		glDeleteTextures(1, &pair.second);
	}
	iconTextures.clear();

	UnloadImagePreviews();

	ilShutDown();
	return true;
}

std::pair<bool, uint32_t> PanelProject::DirectoryTreeViewRecursive(const fs::path& path, uint32_t* count, int* selection_mask)
{
	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

	bool any_node_clicked = false;
	uint32_t node_clicked = 0;

	for (const auto& entry : fs::directory_iterator(path))
	{
		ImGuiTreeNodeFlags node_flags = base_flags;
		const bool is_selected = (*selection_mask & BIT(*count)) != 0;
		if (is_selected)
			node_flags |= ImGuiTreeNodeFlags_Selected;

		std::string name = entry.path().string();

		auto lastSlash = name.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		name = name.substr(lastSlash, name.size() - lastSlash);

		bool entryIsFile = !fs::is_directory(entry.path());
		if (entryIsFile)
			node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)(*count), node_flags, name.c_str());

		if (ImGui::IsItemClicked())
		{
			node_clicked = *count;
			any_node_clicked = true;
			if (entry.is_directory())
			{
				directoryPath = entry.path().string();
				refresh = true;
			}
		}

		(*count)--;

		if (!entryIsFile)
		{
			if (node_open)
			{
				auto clickState = DirectoryTreeViewRecursive(entry.path(), count, selection_mask);

				if (!any_node_clicked)
				{
					any_node_clicked = clickState.first;
					node_clicked = clickState.second;
				}

				ImGui::TreePop();
			}
			else
			{
				for (const auto& e : fs::recursive_directory_iterator(entry.path()))
					(*count)--;
			}
		}
	}

	return { any_node_clicked, node_clicked };
}

void PanelProject::FileExplorerDraw()
{
	if (refresh)
	{
		fileSelected = nullptr;
		files.clear();
		files = ListFiles(directoryPath);
		refresh = false;
	}

	float contentWidth = ImGui::GetWindowContentRegionWidth();
	GLuint textureID = 0;

	for (auto& file : files) 
	{
		std::string displayName = (file.name.size() >= 10) ? file.name.substr(0, 10) + "..." : file.name;

		if (ImGui::GetCursorPosX() + fontSize > contentWidth)
			ImGui::NewLine();

		ImGui::BeginGroup();

		GLuint iconTexture = 0;

		if (file.fileType == FileType::TEXTURE)
		{
			iconTexture = imagePreviews[file.name];
		}
		else
		{
			iconTexture = iconTextures[file.fileType];
		}

		ImGui::Indent();
		ImVec2 textPos(ImGui::GetCursorPos().x + (fontSize - ImGui::CalcTextSize(displayName.c_str()).x) * 0.5f, ImGui::GetCursorPos().y + fontSize + 10);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
		
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));

		ImGui::ImageButton((void*)(intptr_t)iconTexture, ImVec2(fontSize, fontSize), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), -1, ImVec4(.30f, .30f, .30f, 0.00f));

		if (ImGui::IsItemHovered() && (ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1)))
		{
			fileSelected = &file;
		}

		if (DragAndDrop(file))
			break;

		DoubleClickFile();

		ContextMenu();

		ImGui::SetCursorPos(textPos);

		ImGui::Text("%s", displayName.c_str(), ImVec2(fontSize, fontSize));

		ImGui::EndGroup();

		ImGui::SameLine(0, ImGui::GetStyle().ItemSpacing.y + fontSize);
		
		ImGui::PopStyleVar(5);
	}
}

std::vector<FileInfo> PanelProject::ListFiles(const std::string& path)
{
	UnloadImagePreviews();
	std::vector<FileInfo> files;
	for (const auto& entry : fs::directory_iterator(path)) {
		FileInfo fileInfo;

		fileInfo.path = entry.path();
		fileInfo.name = entry.path().stem().string();
		fileInfo.isDirectory = entry.is_directory();

		if (fileInfo.isDirectory)
		{
			fileInfo.fileType = FileType::FOLDER;
		}
		else
		{
			std::string extension = entry.path().extension().string();
			fileInfo.fileType = FindFileType(extension);
			LoadImagePreviews(fileInfo);
		}

		files.push_back(fileInfo);
	}
	return files;
}

FileType PanelProject::FindFileType(const std::string& fileExtension)
{
	// Convert fileExtension to lowercase
	std::string lowercaseExtension = fileExtension;
	std::transform(lowercaseExtension.begin(), lowercaseExtension.end(), lowercaseExtension.begin(),
		[](unsigned char c) { return std::tolower(c); });

	if (lowercaseExtension == ".fbx")
	{
		return FileType::MODEL3D;
	}
	else if (lowercaseExtension == ".png" || lowercaseExtension == ".dds")
	{
		return FileType::TEXTURE;
	}
	else if (lowercaseExtension == ".cs")
	{
		return FileType::SCRIPT;
	}
	else if (lowercaseExtension == ".toe")
	{
		return FileType::SCENE;
	}
	else if (lowercaseExtension == ".prefab")
	{
		return FileType::PREFAB;
	}
	else if (lowercaseExtension == ".txt")
	{
		return FileType::TXT;
	}
	else if (lowercaseExtension == ".particles")
	{
		return FileType::PARTICLES;
	}

	return FileType::UNKNOWN;
}

GLuint PanelProject::LoadTexture(const std::string& path)
{
	// Generate a new texture ID
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Load image using DevIL
	ILuint ilImage;
	ilGenImages(1, &ilImage);
	ilBindImage(ilImage);

	if (!ilLoadImage((const wchar_t*)path.c_str())) {
		LOG(LogType::LOG_ERROR, "Error loading texture: %s", path.c_str());
		return 0;
	}

	// Convert image to RGBA format
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	// Set texture parameters
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
		0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Cleanup DevIL resources
	ilDeleteImages(1, &ilImage);

	return textureID;
}

void PanelProject::LoadIcons()
{
	// Initialize DevIL
	ilInit();

	// Load and store icon textures
	iconTextures[FileType::FOLDER] = LoadTexture("Config\\Icons/PanelProject/folder.png");
	iconTextures[FileType::MODEL3D] = LoadTexture("Config\\Icons/PanelProject/fbx.png");
	iconTextures[FileType::PREFAB] = LoadTexture("Config\\Icons/PanelProject/prefab.png");
	iconTextures[FileType::SCENE] = LoadTexture("Config\\Icons/PanelProject/scene.png");
	iconTextures[FileType::SCRIPT] = LoadTexture("Config\\Icons/PanelProject/script.png");
	iconTextures[FileType::TEXTURE] = LoadTexture("Config\\Icons/PanelProject/texture.png");
	iconTextures[FileType::TXT] = LoadTexture("Config\\Icons/PanelProject/txt.png");
	iconTextures[FileType::PARTICLES] = LoadTexture("Config\\Icons/PanelProject/particles.png");
	iconTextures[FileType::UNKNOWN] = LoadTexture("Config\\Icons/PanelProject/unknown.png");
}

void PanelProject::LoadImagePreviews(const FileInfo& info)
{
	if (info.fileType == FileType::TEXTURE)
	{
		imagePreviews[info.name] = LoadTexture(info.path.string());
	}
}

void PanelProject::UnloadImagePreviews()
{
	// Clean up icon textures
	for (auto& pair : imagePreviews) {
		glDeleteTextures(1, &pair.second);
	}
	imagePreviews.clear();
}

void PanelProject::SaveWarning()
{
	if (warningScene)
	{
		ImGui::OpenPopup("WarningScene");
		warningScene = false;
	}

	ImGui::SetNextWindowSize(ImVec2(415, 70));
	ImVec2 mainViewportPos = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(ImVec2(mainViewportPos.x, mainViewportPos.y), ImGuiCond_Appearing, ImVec2(0.5, 0.9));

	if (ImGui::BeginPopup("WarningScene"))
	{
		ImGui::SetCursorPosY(10.0f);
		ImGui::Indent();
		ImGui::Text("You have unsaved changes in this scene. Are you sure?");

		ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 100 * 2.0f - ImGui::GetStyle().ItemSpacing.x) / 2.0f);
		if (ImGui::Button("Yes", { 100, 20 })) {
			engine->N_sceneManager->LoadScene(fileSelected->name);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("No", { 100, 20 })) {
			ImGui::CloseCurrentPopup();
		}
	
	}
}

void PanelProject::DoubleClickFile()
{
	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
	{
		switch (fileSelected->fileType)
		{
		case FileType::FOLDER:
			directoryPath = fileSelected->path.string();
			refresh = true;
			break;
		case FileType::SCENE:
			if (engine->N_sceneManager->currentScene->IsDirty())
			{
				warningScene = true;
			}
			else
			{
				engine->N_sceneManager->LoadScene(fileSelected->name);
			}
			break;
		case FileType::PREFAB:
			if (engine->N_sceneManager->currentScene->IsDirty())
			{
				warningScene = true;
			}
			else
			{
				engine->N_sceneManager->LoadScene(fileSelected->name);
			}
			break;
		default:
			break;
		}
	}
	
}

bool PanelProject::DragAndDrop(const FileInfo& info)
{
	// Check if the window is being hovered over while dragging
	if (ImGui::IsWindowHovered() && ImGui::IsMouseDragging(0)) 
	{
		ImGui::SetWindowFocus("Project");
	}

	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload(fileSelected->name.c_str(), &info, sizeof(FileInfo));

		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget()) 
	{
		if (engine->N_sceneManager->GetSelectedGO().get())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(engine->N_sceneManager->GetSelectedGO().get()->GetName().c_str()))
			{
				GameObject* gameObject = *(GameObject**)payload->Data;
				if (gameObject)
				{
					// Save the GameObject as a prefab
					SaveGameObjectAsPrefab(*gameObject, info);
				}
			}
		}
		else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(fileSelected->name.c_str()))
		{
			FileInfo* fileInfo = (FileInfo*)payload->Data;
			if (fileInfo) 
			{
				if (ImGui::IsItemHovered())
				{
					//LOG(LogType::LOG_INFO, "Path Hovered File: %s", info.path.string().c_str());
					// Perform the file move operation
					//FileDropping(info);
					fs::rename(fileSelected->path, info.path / fileSelected->path.filename());
					fileSelected = nullptr;
					refresh = true;
				}
			}
			
		}
		ImGui::EndDragDropTarget();
	}

	return refresh;
}

void PanelProject::FileDropping(const FileInfo& info)
{
	if (fileSelected->fileType != FileType::FOLDER && info.fileType == FileType::FOLDER)
	{
		fs::rename(fileSelected->path, info.path / fileSelected->path.filename());
	}
	else if (fileSelected->fileType == FileType::FOLDER && info.fileType == FileType::FOLDER)
	{
		fs::rename(fileSelected->path, info.path / fileSelected->path.filename());
	}

	fileSelected = nullptr;
	refresh = true;
}

void PanelProject::ContextMenu()
{
	if (ImGui::BeginPopupContextItem())
	{
		
		if (ImGui::MenuItem("Delete"))
		{
			fs::remove(fileSelected->path);
			fileSelected = nullptr;
			refresh = true;
		}

		ImGui::EndPopup();
	}
}

void PanelProject::SaveGameObjectAsPrefab(GameObject& gameObject, const FileInfo& info) {
	gameObject.SetPrefab(UIDGen::GenerateUID());

	std::string prefabName = gameObject.GetName() + ".prefab";

	// Serialize the GameObject and save it as a prefab file
	json gameObjectJSON = gameObject.SaveGameObject();
	if (info.isDirectory)
	{
		fs::path filename = info.path / prefabName;
		std::ofstream(filename) << gameObjectJSON.dump(2);
	}
	else
	{
		fs::path filename = directoryPath + prefabName;
		std::ofstream(filename) << gameObjectJSON.dump(2);
	}
	LOG(LogType::LOG_OK, "PREFAB CREATED SUCCESSFULLY");
	refresh = true;
	engine->N_sceneManager->currentScene->SetIsDirty(true);
}