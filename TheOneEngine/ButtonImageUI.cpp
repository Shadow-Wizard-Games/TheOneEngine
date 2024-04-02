#include "ButtonImageUI.h"
#include "Canvas.h"

ButtonImageUI::ButtonImageUI(std::shared_ptr<GameObject> containerGO, Rect2D rect) : ItemUI(containerGO, UiType::BUTTONIMAGE, name, false, rect)
{
	this->name = "ButtonImage";
	imagePath = "Assets/Meshes/HUD.png";
	image = std::make_unique<Texture>(imagePath, containerGO);
	switch (this->state)
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

ButtonImageUI::ButtonImageUI(std::shared_ptr<GameObject> containerGO, const std::string& path, std::string name, Rect2D rect) : ItemUI(containerGO, UiType::BUTTONIMAGE, name, false, rect), imagePath(path)
{
	image = std::make_unique<Texture>(imagePath);
	switch (this->state)
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

ButtonImageUI::~ButtonImageUI() {}

void ButtonImageUI::Draw2D()
{
	UpdateState();

	auto canvas = containerGO.get()->GetComponent<Canvas>();

	float posX = canvas->GetRect().x + GetRect().x;
	float posY = canvas->GetRect().y + GetRect().y;

	float width = (canvas->GetRect().w * imageRect.w);
	float height = (canvas->GetRect().h * imageRect.h);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	image.get()->bind();

	glBegin(GL_QUADS);

	glTexCoord2f(currentSection->x, currentSection->y + currentSection->h);  // Top-left corner of the texture
	glVertex2f(posX - width / 2, posY + height / 2);

	glTexCoord2f(currentSection->x + currentSection->w, currentSection->y + currentSection->h);  // Top-right corner of the texture
	glVertex2f(posX + width / 2, posY + height / 2);

	glTexCoord2f(currentSection->x + currentSection->w, currentSection->y);  // Bottom-right corner of the texture
	glVertex2f(posX + width / 2, posY - height / 2);

	glTexCoord2f(currentSection->x, currentSection->y);  // Bottom-left corner of the texture
	glVertex2f(posX - width / 2, posY - height / 2);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
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

	if (UIElementJSON.contains("ImagePath")) imagePath = UIElementJSON["ImagePath"];
	image = std::make_unique<Texture>(imagePath);

	switch (this->state)
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

Rect2D ButtonImageUI::GetSectIdle() const
{
	Rect2D imageSect;
	imageSect.x = imageIdleSection.x * image.get()->width;
	imageSect.y = imageIdleSection.y * image.get()->height;
	imageSect.w = (imageIdleSection.w * image.get()->width);
	imageSect.h = (imageIdleSection.h * image.get()->height);

	return imageSect;
}

void ButtonImageUI::SetSectSizeIdle(float x, float y, float width, float height)
{
	imageIdleSection.x = x / image.get()->width;
	imageIdleSection.y = y / image.get()->height;
	imageIdleSection.w = (width) / image.get()->width;
	imageIdleSection.h = (height) / image.get()->height;
}

Rect2D ButtonImageUI::GetSectHovered() const
{
	Rect2D imageSect;
	imageSect.x = imageHoveredSection.x * image.get()->width;
	imageSect.y = imageHoveredSection.y * image.get()->height;
	imageSect.w = (imageHoveredSection.w * image.get()->width);
	imageSect.h = (imageHoveredSection.h * image.get()->height);

	return imageSect;
}

void ButtonImageUI::SetSectSizeHovered(float x, float y, float width, float height)
{
	imageHoveredSection.x = x / image.get()->width;
	imageHoveredSection.y = y / image.get()->height;
	imageHoveredSection.w = (width) / image.get()->width;
	imageHoveredSection.h = (height) / image.get()->height;
}

Rect2D ButtonImageUI::GetSectSelected() const
{
	Rect2D imageSect;
	imageSect.x = imageSelectedSection.x * image.get()->width;
	imageSect.y = imageSelectedSection.y * image.get()->height;
	imageSect.w = (imageSelectedSection.w * image.get()->width);
	imageSect.h = (imageSelectedSection.h * image.get()->height);

	return imageSect;
}

void ButtonImageUI::SetSectSizeSelected(float x, float y, float width, float height)
{
	imageSelectedSection.x = x / image.get()->width;
	imageSelectedSection.y = y / image.get()->height;
	imageSelectedSection.w = (width) / image.get()->width;
	imageSelectedSection.h = (height) / image.get()->height;
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