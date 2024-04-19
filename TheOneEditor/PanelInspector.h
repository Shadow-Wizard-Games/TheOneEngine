#ifndef __PANEL_INSPECTOR_H__
#define __PANEL_INSPECTOR_H__
#pragma once

#include "Panel.h"
#include "../TheOneEngine/Defs.h"

class GameObject;

class PanelInspector : public Panel
{
public:
	PanelInspector(PanelType type, std::string name);
	~PanelInspector();

	void OnSelectGO(std::shared_ptr<GameObject> gameObj);
	bool Draw();

	void ChooseScriptNameWindow();
	void ChooseParticlesToImportWindow();

	void OverridePrefabFile(GameObject& gameObject);

private:
	GameObject* selectedGO = nullptr;

	vec3f view_pos;
	vec3f view_rot_rad;
	vec3f view_rot_deg;
	vec3f view_sca;

	bool matrixDirty;
	bool chooseScriptNameWindow;
	bool chooseParticlesToImportWindow;

	const char* colliders[2] =
	{
		"Circle",
		"Square",
	};

	int slilderDesignOptionToModify = 0;
};

#endif // !__PANEL_INSPECTOR_H__