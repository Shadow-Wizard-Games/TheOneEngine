#ifndef __GUI_H__
#define __GUI_H__
#pragma once

//hekbas is this necessary?
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

#include "Module.h"
#include "implot.h"
#include <list>

// Forward declaration
class Panel;
class PanelAbout;
class PanelConsole;
class PanelHierarchy;
class PanelInspector;
class PanelProject;
class PanelScene;
class PanelGame;
class PanelAnimation;
class PanelSettings;
class PanelBuild;


class Gui : public Module
{
public:

	Gui(App* app);
	~Gui();

	bool Awake();
	bool Start();
	bool PreUpdate();
	bool Update(double dt);
	bool PostUpdate();
	bool CleanUp();

	void HandleInput(SDL_Event* event);
	void OpenURL(const char* url) const;
	void PlotChart(const char* label, const std::vector<int>& data, ImPlotFlags plotFlags = 0, ImPlotAxisFlags axisFlags = 0);
	bool isInitialized(Panel* panel);

	void CalculateSizeAspectRatio(int maxWidth, int maxHeight, int& width, int& height);
	void AssetContainer(const char* label);

private:

	// Main Dockspace
	void MainWindowDockspace();

	bool MainMenuFile();
	void MainMenuEdit();
	void MainMenuAssets();
	void MainMenuGameObject();
	void MainMenuComponent();
	void MainMenuWindow();
	void MainMenuHelp();

	void OpenSceneFileWindow();

public:

	// Panels
	PanelAbout* panelAbout;
	PanelConsole* panelConsole;
	PanelHierarchy* panelHierarchy;
	PanelInspector* panelInspector;
	PanelProject* panelProject;
	PanelScene* panelScene;
	PanelGame* panelGame;
	PanelAnimation* panelAnimation;
	PanelSettings* panelSettings;
	PanelBuild* panelBuild;

private:

	std::list<Panel*> panels;

	/*Create bools for each window to show and the general that contains all of them*/
	bool show_gui = true;


	/*GuiWindow1 Vars*/
	bool show_guiwindow_1 = false;
	bool full_desktop = false;
	bool minimize_window = false;

	bool openSceneFileWindow = false;
	//ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};

#endif // !__GUI_H__