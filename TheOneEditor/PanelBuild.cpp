#include "PanelBuild.h"
#include "App.h"
#include "Gui.h"
#include "imgui.h"

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

PanelBuild::PanelBuild(PanelType type, std::string name) : Panel(type, name) {}

PanelBuild::~PanelBuild() {}

bool PanelBuild::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5, 0.7));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	
	ImGuiWindowFlags settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

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
