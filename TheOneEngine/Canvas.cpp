#include "Canvas.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "ItemUI.h"
#include "ImageUI.h"
#include "ButtonImageUI.h"
#include "SliderUI.h"
#include "CheckerUI.h"
#include "Renderer2D.h"

Canvas::Canvas(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Canvas)
{}

Canvas::Canvas(std::shared_ptr<GameObject> containerGO, Canvas* ref) : Component(containerGO, ComponentType::Canvas)
{
	//this->uiElements = ref->GetUiElements();
	for (auto& itemPtr : ref->GetUiElements())
	{
		std::unique_ptr<ItemUI> uniquePtr(itemPtr);
		this->uiElements.push_back(std::move(uniquePtr));
	}
	this->debugDraw = ref->debugDraw;
	this->rect = ref->rect;
}

Canvas::~Canvas() 
{
	uiElements.clear();
}

void Canvas::DrawComponent(Camera* camera)
{
	if (!camera)
		return;

	if (debugDraw)
	{
		Renderer2D::DrawRect({ rect.x, rect.y, 0.0f }, { rect.w, rect.h }, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	for (auto const& element : uiElements)
	{
		element->Draw2D();
	}
}

Rect2D Canvas::GetRect() const
{
	return rect;
}

void Canvas::SetSize(float width, float height)
{
	rect.w = width;
	rect.h = height;
}

json Canvas::SaveComponent()
{
	json canvasJSON;

	canvasJSON["UID"] = UID;
	canvasJSON["Name"] = name;
	canvasJSON["Type"] = type;
	canvasJSON["Enabled"] = enabled;
	canvasJSON["Rect"] = { rect.x, rect.y, rect.w, rect.h };
	canvasJSON["DebugDraw"] = debugDraw;

	if (auto pGO = containerGO.lock())
		canvasJSON["ParentUID"] = pGO.get()->GetUID();

	if (!uiElements.empty())
	{
		json uiElementsJSON;
		for (auto& item : uiElements)
		{
			uiElementsJSON.push_back(item.get()->SaveUIElement());
		}
		canvasJSON["UiElements"] = uiElementsJSON;
	}

	return canvasJSON;
}

void Canvas::LoadComponent(const json& canvasJSON)
{
	if (canvasJSON.contains("UID")) UID = canvasJSON["UID"];
	if (canvasJSON.contains("Name")) name = canvasJSON["Name"];
	if (canvasJSON.contains("Type")) type = canvasJSON["Type"];
	if (canvasJSON.contains("Enabled")) enabled = canvasJSON["Enabled"];
	if (canvasJSON.contains("Rect"))
	{
		rect.x = canvasJSON["Rect"][0];
		rect.y = canvasJSON["Rect"][1];
		rect.w = canvasJSON["Rect"][2];
		rect.h = canvasJSON["Rect"][3];
	}
	if (canvasJSON.contains("DebugDraw")) debugDraw = canvasJSON["DebugDraw"];

	if (canvasJSON.contains("UiElements"))
	{
		const json& uiElementsJSON = canvasJSON["UiElements"];

		for (auto& item : uiElementsJSON)
		{			
			if (item["Type"] == (int)UiType::IMAGE)
			{
				int id = this->AddItemUI<ImageUI>();
				this->GetItemUI<ImageUI>(id)->LoadUIElement(item);
			}
			if (item["Type"] == (int)UiType::BUTTONIMAGE)
			{
				int id = this->AddItemUI<ButtonImageUI>();
				this->GetItemUI<ButtonImageUI>(id)->LoadUIElement(item);
			}
			if (item["Type"] == (int)UiType::SLIDER)
			{
				int id = this->AddItemUI<SliderUI>();
				this->GetItemUI<SliderUI>(id)->LoadUIElement(item);
			}
			if (item["Type"] == (int)UiType::CHECKER)
			{
				int id = this->AddItemUI<CheckerUI>();
				this->GetItemUI<CheckerUI>(id)->LoadUIElement(item);
			}
			if (item["Type"] == (int)UiType::UNKNOWN)
			{
				//default
			}
		}
	}
}

bool Canvas::RemoveItemUI(unsigned int id)
{
	bool ret = true;

	for (auto it = uiElements.begin(); it != uiElements.end(); ++it)
	{
		if ((*it)->GetID() == id)
		{
			it = uiElements.erase(it);
			break;
		}
	}

	return ret;
}

std::vector<ItemUI*> Canvas::GetUiElements()
{
	std::vector<ItemUI*> tempUiElements;
	for (const auto& item : uiElements)
	{
		tempUiElements.push_back(item.get());
	}
	return tempUiElements;
}

std::vector<std::unique_ptr<ItemUI>>& Canvas::GetUiElementsPtr()
{
	return uiElements;
}
