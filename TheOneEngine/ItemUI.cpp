#include "ItemUI.h"
#include "UIDGen.h"

ItemUI::ItemUI(std::shared_ptr<GameObject> containerGO, UiType type, std::string name, bool interactuable, Rect2D rect) : containerGO(containerGO), type(type), interactuable(interactuable), imageRect(rect), name(name)
{
	this->state = UiState::IDLE;
	this->print = true;
	this->id = UIDGen::GenerateUID();
}

ItemUI::ItemUI(ItemUI* ref)
{
	this->type = ref->type;
	this->interactuable = ref->interactuable;
	this->containerGO = ref->containerGO;
	this->imageRect = ref->GetRect();
	this->state = ref->GetState();
	this->id = UIDGen::GenerateUID();
	this->name = ref->GetName();
	this->print = ref->print;
}

ItemUI::~ItemUI()
{
}

void ItemUI::Draw2D()
{

}

void ItemUI::UpdateState()
{

}

Rect2D ItemUI::GetRect() const
{
	return imageRect;
}

void ItemUI::SetSize(float width, float height)
{
	imageRect.w = width;
	imageRect.h = height;
}

json ItemUI::SaveUIElement()
{
	json uiElementJSON;

	uiElementJSON["ID"] = (unsigned int)id;
	uiElementJSON["Name"] = name.c_str();
	uiElementJSON["Rect"] = { imageRect.x, imageRect.y, imageRect.w, imageRect.h };
	uiElementJSON["Type"] = (int)type;
	uiElementJSON["State"] = (int)state;
	uiElementJSON["Interactuable"] = interactuable;
	uiElementJSON["Print"] = print;

	return uiElementJSON;
}

void ItemUI::LoadUIElement(const json& UIElementJSON)
{
	if (UIElementJSON.contains("ID")) id = (unsigned int)UIElementJSON["ID"];
	if (UIElementJSON.contains("Name")) name = UIElementJSON["Name"];
	if (UIElementJSON.contains("Rect"))
	{
		imageRect.x = UIElementJSON["Rect"][0];
		imageRect.y = UIElementJSON["Rect"][1];
		imageRect.w = UIElementJSON["Rect"][2];
		imageRect.h = UIElementJSON["Rect"][3];
	}
	if (UIElementJSON.contains("Type")) type = (UiType)UIElementJSON["Type"];
	if (UIElementJSON.contains("State")) state = (UiState)UIElementJSON["State"];
	if (UIElementJSON.contains("Interactuable")) interactuable = UIElementJSON["Interactuable"];
	if (UIElementJSON.contains("Print")) print = UIElementJSON["Print"];
}