#ifndef __GUI_H__
#define __GUI_H__
#pragma once

//hekbas is this necessary?
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

#include "Module.h"
#include "implot.h"

#include "SDL2/SDL_events.h"

#include <list>
#include <vector>
#include <cmath>

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

enum class Aspect
{
	A_FREE,
	A_16x9,
	A_21x9
};

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

	bool IsInitialized(Panel* panel);
	std::list<Panel*> GetPanels();
	void HandleInput(SDL_Event* event);

	// Utils
	template <typename T>
	void ApplyAspectRatio(T maxWidth, T maxHeight, T* width, T* height, Aspect aspect = Aspect::A_16x9)
	{
		double givenRatio = static_cast<double>(maxWidth) / static_cast<double>(maxHeight);
		double targetRatio;

		switch (aspect)
		{
		case Aspect::A_FREE: targetRatio = givenRatio;   break;
		case Aspect::A_16x9: targetRatio = 16.0 / 9.0;   break;
		case Aspect::A_21x9: targetRatio = 21.0 / 9.0;   break;
		default: break;
		}

		if (givenRatio <= targetRatio) {
			// The given rectangle is wider, so the width is limited
			*width = maxWidth;
			*height = static_cast<T>(std::round(*width / targetRatio));
		}
		else {
			// The given rectangle is taller, so the height is limited
			*height = maxHeight;
			*width = static_cast<T>(std::round(*height * targetRatio));
		}
	}

	void OpenURL(const char* url) const;
	void PlotChart(const char* label, const std::vector<int>& data, ImPlotFlags plotFlags = 0, ImPlotAxisFlags axisFlags = 0);
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

	bool showImGuiDemo = false;
	bool openSceneFileWindow = false;
};

#endif // !__GUI_H__