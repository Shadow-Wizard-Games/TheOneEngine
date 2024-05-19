#ifndef __PANEL_SETTINGS_H__
#define __PANEL_SETTINGS_H__
#pragma once

#include "Panel.h"
#include "imgui.h"
#include <vector>

#define MAX_HISTORY_SIZE 240

enum class SelectedSetting
{
	PERFORMANCE,
	WINDOW,
	INPUT,
	RENDERER,
	COLOR_PICKER,
	HARDWARE,
	SOFTWARE
};

class PanelSettings : public Panel
{
public:

	PanelSettings(PanelType type, std::string name);
	~PanelSettings();

	bool Draw();
	void AddFpsValue(int fps);
	ImGuiColorEditFlags GetColorFlags() { return colorPickerFlags; }

private:

	void Performance();
	void Window();
	void Input();
	void Renderer();
	void ColorPicker();
	void Hardware();
	void Software();
	

private:

	SelectedSetting selected = SelectedSetting::PERFORMANCE;

	std::vector<int> fpsHistory;
	std::vector<double> delayHistory;

	const char* displayModes[4] =
	{
		"Windowed",
		"Full Screen",
		"Full Screen Desktop",
		"Borderless"
	};

	const char* resolutions[8] =
	{
		"3840x2160",
		"2560x1440",
		"1920x1080",
		"1280x720",
		"854x480",
		"640x360",
		"426x240",
		"Native"
	};

	const char* fpsList[8] =
	{
		"30",
		"60",
		"120",
		"144",
		"160",
		"180",
		"240",
		"Unlimited"
	};

	// Color Picker
	bool alpha_preview = true;
	bool alpha_half_preview = false;
	bool drag_and_drop = true;
	bool options_menu = true;
	bool hdr = false;

	ImGuiColorEditFlags colorPickerFlags;
};

#endif // !__PANEL_SETTINGS_H__