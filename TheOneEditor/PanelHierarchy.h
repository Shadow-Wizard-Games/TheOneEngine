#ifndef __PANEL_HIERARCHY_H__
#define __PANEL_HIERARCHY_H__
#pragma once

#include "Panel.h"

#include "../TheOneEngine/GameObject.h"

#include <string>
#include <vector>
#include <list>
#include <memory>

class PanelHierarchy : public Panel
{
public:
	PanelHierarchy(PanelType type, std::string name);
	~PanelHierarchy();

	void RecurseShowChildren(std::shared_ptr<GameObject> parent);

	bool Draw();

	void ContextMenu(std::shared_ptr<GameObject> go);

	bool ReparentDragDrop(std::shared_ptr<GameObject> childGO);

	void SaveGameobjectAsPrefab(std::shared_ptr<GameObject> goToSaveAsPrefab);

private:
	bool open_selected;
	bool reparent;

	bool remove = false;
	bool duplicate = false;
	bool createEmpty = false;
	std::vector<std::shared_ptr<GameObject>> toDeleteList;
};

#endif // !__PANEL_HIERARCHY_H__