#include "PanelAbout.h"
#include "App.h"
#include "Gui.h"
#include "imgui.h"

PanelAbout::PanelAbout(PanelType type, std::string name) : Panel(type, name) {}

PanelAbout::~PanelAbout() {}

bool PanelAbout::Draw()
{
	ImGuiWindowFlags settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5, 0.9));

	if (ImGui::Begin("About TheOneEngine", &enabled, settingsFlags))
	{
		//hekbas: change this for project3!
		ImGui::Text("TheOneEngine by Hector Bascones Zamora, Arnau Jimenez Gallego & Julia Serra Trujillo.");
		ImGui::Text("This is a demo for the subject of Game Engines, CITM - UPC");
	}	
	ImGui::End();

	return true;
}