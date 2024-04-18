#include "PanelBuild.h"
#include "App.h"
#include "Gui.h"
#include "imgui.h"

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

PanelBuild::PanelBuild(PanelType type, std::string name) : Panel(type, name)
{
}

PanelBuild::~PanelBuild()
{
}

bool PanelBuild::Draw()
{
	ImGuiWindowFlags settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	ImGui::SetNextWindowSize(ImVec2(400, 400));
	ImVec2 mainViewportPos = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(ImVec2(mainViewportPos.x, mainViewportPos.y), ImGuiCond_Appearing, ImVec2(0.5, 0.7));
	

	uint treeFlags = ImGuiTreeNodeFlags_DefaultOpen;

	if (ImGui::Begin(name.c_str(), &enabled, settingsFlags))
	{
		if (ImGui::TreeNodeEx("Scenes", treeFlags))
		{
			//for (auto sceneFile : fs::iterator)

			ImGui::TreePop();
		}

	}
	ImGui::End();

	ImGui::PopStyleVar();

	return true;
}
