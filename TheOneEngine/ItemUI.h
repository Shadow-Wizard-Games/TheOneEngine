#ifndef __ITEMUI_H__
#define __ITEMUI_H__
#pragma once

#include "GameObject.h"
#include <string>

enum class UiType {
	IMAGE,
	BUTTONIMAGE,
	UNKNOWN
};

enum class UiState {
	IDLE,
	HOVERED,
	SELECTED,
	HOVEREDSELECTED,
	DISABLED,
	UNKNOWN
};

struct Rect2D
{
	float x, y, w, h;
};

class ItemUI
{
public:
	ItemUI(std::shared_ptr<GameObject> containerGO, UiType type, std::string name = "Name", bool interactuable = false, Rect2D rect = {0,0,1,1});
	virtual ~ItemUI();

	virtual void Draw2D();

	Rect2D GetRect() const;
	void SetSize(float width, float height);

	virtual json SaveUIElement();
	virtual void LoadUIElement(const json& UIElementJSON);

	void SetName(std::string newName)
	{
		this->name = newName;
	}

	std::string GetName() const
	{
		return name;
	}
	unsigned int GetID() const
	{
		return id;
	}

	UiType GetType()
	{
		return type;
	}

	Rect2D GetRect()
	{
		return imageRect;
	}

	void SetRect(float x, float y, float w, float h)
	{
		imageRect = { x, y, w, h };
	}

	UiState GetState()
	{
		return state;
	}

	void SetState(UiState state)
	{
		this->state = state;
	}

	virtual void UpdateState();

protected:

	Rect2D imageRect;
	bool interactuable;

	std::shared_ptr<GameObject> containerGO;
	UiType type;

	UiState state;

	unsigned int id;
	std::string name;
};

#endif // !__ITEMUI_H__