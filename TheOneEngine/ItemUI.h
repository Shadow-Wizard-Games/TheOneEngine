#ifndef __ITEMUI_H__
#define __ITEMUI_H__
#pragma once

#include "GameObject.h"
#include "Texture.h"
#include <string>
#include "Renderer2D.h"
#include <TheOneEngine/Renderer2D.h>

enum class UiType {
	IMAGE,
	BUTTONIMAGE,
	SLIDER,
	CHECKER,
	TEXT,
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
	ItemUI(ItemUI* ref);
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

	Rect2D GetImageSection(const Rect2D& activeSection, const Rect2D& inactiveSection, bool activePart, Texture* texture) const {
		const Rect2D& section = activePart ? activeSection : inactiveSection;
		return {
			section.x * texture->GetSize().x,
			section.y * texture->GetSize().y,
			section.w * texture->GetSize().x,
			section.h * texture->GetSize().y
		};
	}

	void SetImageSection(Rect2D& activeSection, Rect2D& inactiveSection, bool activePart, float x, float y, float w, float h, Texture* texture) {
		Rect2D& section = activePart ? activeSection : inactiveSection;
		section.x = x / texture->GetSize().x;
		section.y = y / texture->GetSize().y;
		section.w = w / texture->GetSize().x;
		section.h = h / texture->GetSize().y;
	}

	Renderer2D::TexCoordsSection Rect2DToTexCoordsSection(const Rect2D& rect) {
		Renderer2D::TexCoordsSection texCoords;

		texCoords.leftBottom = { rect.x, rect.y };
		texCoords.rightBottom = { rect.x + rect.w, rect.y };
		texCoords.rightTop = { rect.x + rect.w, rect.y + rect.h };
		texCoords.leftTop = { rect.x, rect.y + rect.h };

		return texCoords;
	}
};

#endif // !__ITEMUI_H__