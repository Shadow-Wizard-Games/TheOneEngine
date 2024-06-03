#include "ButtonImageUI.h"
#include "Canvas.h"
#include "Renderer2D.h"

ButtonImageUI::ButtonImageUI(std::shared_ptr<GameObject> containerGO, Rect2D rect) : ItemUI(containerGO, UiType::BUTTONIMAGE, name, false, rect)
{
	this->name = "ButtonImage";
	imagePath = "Assets/Meshes/HUD.png";
	imageID = Resources::Load<Texture>(imagePath);
	UpdateState();
	countAsRealButton = true;
}

ButtonImageUI::ButtonImageUI(std::shared_ptr<GameObject> containerGO, const std::string& path, std::string name, Rect2D rect) : ItemUI(containerGO, UiType::BUTTONIMAGE, name, false, rect), imagePath(path)
{
	imageID = Resources::Load<Texture>(imagePath);
	UpdateState();
	countAsRealButton = true;
}

ButtonImageUI::ButtonImageUI(ButtonImageUI* ref) : ItemUI(ref)
{
	this->imagePath = ref->imagePath;
	this->imageID = ref->imageID;
	this->imageIdleSection = ref->imageIdleSection;
	this->imageHoveredSection = ref->imageHoveredSection;
	this->imageSelectedSection = ref->imageSelectedSection;
	this->currentSection = ref->currentSection;
	this->countAsRealButton = ref->countAsRealButton;

	UpdateState();
}

ButtonImageUI::~ButtonImageUI() {}

void ButtonImageUI::Draw2D()
{
	UpdateState();

	auto canvas = containerGO.get()->GetComponent<Canvas>();

	float posX = canvas->GetRect().x + GetRect().x;
	float posY = canvas->GetRect().y + GetRect().y;

	float width = (canvas->GetRect().w * imageRect.w);
	float height = (canvas->GetRect().h * imageRect.h);

	Renderer2D::DrawQuad(BatchType::UI, { posX, posY }, { width, height }, imageID, Rect2DToTexCoordsSection(*currentSection));
}

json ButtonImageUI::SaveUIElement()
{
	json uiElementJSON;

	uiElementJSON["ID"] = (unsigned int)id;
	uiElementJSON["Name"] = name.c_str();
	uiElementJSON["Rect"] = { imageRect.x, imageRect.y, imageRect.w, imageRect.h };
	uiElementJSON["ImageSectionIdle"] = { imageIdleSection.x, imageIdleSection.y, imageIdleSection.w, imageIdleSection.h };
	uiElementJSON["ImageSectionHovered"] = { imageHoveredSection.x, imageHoveredSection.y, imageHoveredSection.w, imageHoveredSection.h };
	uiElementJSON["ImageSectionSelected"] = { imageSelectedSection.x, imageSelectedSection.y, imageSelectedSection.w, imageSelectedSection.h };
	uiElementJSON["Type"] = (int)type;
	uiElementJSON["State"] = (int)state;
	uiElementJSON["Interactuable"] = interactuable;
	uiElementJSON["Print"] = print;
	uiElementJSON["CountAsRealButton"] = countAsRealButton;

	uiElementJSON["ImagePath"] = imagePath;

	return uiElementJSON;
}

void ButtonImageUI::LoadUIElement(const json& UIElementJSON)
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
	if (UIElementJSON.contains("ImageSectionIdle"))
	{
		imageIdleSection.x = UIElementJSON["ImageSectionIdle"][0];
		imageIdleSection.y = UIElementJSON["ImageSectionIdle"][1];
		imageIdleSection.w = UIElementJSON["ImageSectionIdle"][2];
		imageIdleSection.h = UIElementJSON["ImageSectionIdle"][3];
	}
	if (UIElementJSON.contains("ImageSectionHovered"))
	{
		imageHoveredSection.x = UIElementJSON["ImageSectionHovered"][0];
		imageHoveredSection.y = UIElementJSON["ImageSectionHovered"][1];
		imageHoveredSection.w = UIElementJSON["ImageSectionHovered"][2];
		imageHoveredSection.h = UIElementJSON["ImageSectionHovered"][3];
	}
	if (UIElementJSON.contains("ImageSectionSelected"))
	{
		imageSelectedSection.x = UIElementJSON["ImageSectionSelected"][0];
		imageSelectedSection.y = UIElementJSON["ImageSectionSelected"][1];
		imageSelectedSection.w = UIElementJSON["ImageSectionSelected"][2];
		imageSelectedSection.h = UIElementJSON["ImageSectionSelected"][3];
	}
	if (UIElementJSON.contains("Type")) type = (UiType)UIElementJSON["Type"];
	if (UIElementJSON.contains("State")) state = (UiState)UIElementJSON["State"];
	if (UIElementJSON.contains("Interactuable")) interactuable = UIElementJSON["Interactuable"];
	if (UIElementJSON.contains("Print")) print = UIElementJSON["Print"];
	if (UIElementJSON.contains("CountAsRealButton")) countAsRealButton = UIElementJSON["CountAsRealButton"];

	if (UIElementJSON.contains("ImagePath")) imagePath = UIElementJSON["ImagePath"];
	imageID = Resources::Load<Texture>(imagePath);

	UpdateState();
}

Rect2D ButtonImageUI::GetSectIdle() const
{
	Rect2D imageSect = { 0, 0, 0, 0 };
	if (imageID != -1)
	{
		Texture* image = Resources::GetResourceById<Texture>(imageID);
		imageSect.x = imageIdleSection.x  * image->GetSize().x;
		imageSect.y = imageIdleSection.y  * image->GetSize().y;
		imageSect.w = (imageIdleSection.w * image->GetSize().x);
		imageSect.h = (imageIdleSection.h * image->GetSize().y);
	}
	return imageSect;
}

void ButtonImageUI::SetSectSizeIdle(float x, float y, float width, float height)
{
	if (imageID != -1)
	{
		Texture* image = Resources::GetResourceById<Texture>(imageID);
		imageIdleSection.x = x        / image->GetSize().x;
		imageIdleSection.y = y        / image->GetSize().y;
		imageIdleSection.w = (width)  / image->GetSize().x;
		imageIdleSection.h = (height) / image->GetSize().y;
	}
}

Rect2D ButtonImageUI::GetSectHovered() const
{
	Rect2D imageSect = { 0, 0, 0, 0 };
	if (imageID != -1)
	{
		Texture* image = Resources::GetResourceById<Texture>(imageID);
		imageSect.x = imageHoveredSection.x  * image->GetSize().x;
		imageSect.y = imageHoveredSection.y  * image->GetSize().y;
		imageSect.w = (imageHoveredSection.w * image->GetSize().x);
		imageSect.h = (imageHoveredSection.h * image->GetSize().y);
	}
	return imageSect;
}

void ButtonImageUI::SetSectSizeHovered(float x, float y, float width, float height)
{
	if (imageID != -1)
	{
		Texture* image = Resources::GetResourceById<Texture>(imageID);
		imageHoveredSection.x = x        / image->GetSize().x;
		imageHoveredSection.y = y        / image->GetSize().y;
		imageHoveredSection.w = (width)  / image->GetSize().x;
		imageHoveredSection.h = (height) / image->GetSize().y;
	}
}

Rect2D ButtonImageUI::GetSectSelected() const
{
	Rect2D imageSect = { 0, 0, 0, 0 };
	if (imageID != -1)
	{
		Texture* image = Resources::GetResourceById<Texture>(imageID);
		imageSect.x = imageSelectedSection.x  * image->GetSize().x;
		imageSect.y = imageSelectedSection.y  * image->GetSize().y;
		imageSect.w = (imageSelectedSection.w * image->GetSize().x);
		imageSect.h = (imageSelectedSection.h * image->GetSize().y);
	}
	return imageSect;
}

void ButtonImageUI::SetSectSizeSelected(float x, float y, float width, float height)
{
	if (imageID != -1)
	{
		Texture* image = Resources::GetResourceById<Texture>(imageID);
		imageSelectedSection.x = x        / image->GetSize().x;
		imageSelectedSection.y = y        / image->GetSize().y;
		imageSelectedSection.w = (width)  / image->GetSize().x;
		imageSelectedSection.h = (height) / image->GetSize().y;
	}
}

void ButtonImageUI::UpdateState()
{
	switch (state)
	{
	case UiState::IDLE:
		currentSection = &imageIdleSection;
		break;
	case UiState::HOVERED:
		currentSection = &imageHoveredSection;
		break;
	case UiState::SELECTED:
		currentSection = &imageSelectedSection;
		break;
	case UiState::HOVEREDSELECTED:
		break;
	case UiState::DISABLED:
		break;
	case UiState::UNKNOWN:
		break;
	default:
		break;
	}
}