#ifndef __PANEL_H__
#define __PANEL_H__
#pragma once

#include <string>

enum class PanelType
{
	ABOUT,

	CONSOLE,
	HIERARCHY,
	INSPECTOR,
	PROJECT,
	SCENE,
	GAME,
	SETTINGS,
	BUILD,

	UNKNOWN
};

class Panel
{
public:

	Panel(PanelType type, std::string name);
	virtual ~Panel();
	virtual void Start() {}

	virtual bool Draw() = 0;
	virtual bool CleanUp() { return true; }

	virtual PanelType GetType();
	virtual std::string GetName();

	virtual bool GetState();
	virtual void SetState(bool state);
	virtual void SwitchState();


protected:

	PanelType type = PanelType::UNKNOWN;
	std::string name;
	bool enabled = false;
};

#endif // !__PANEL_H__