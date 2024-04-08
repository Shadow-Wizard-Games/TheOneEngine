#include "ButtonImageUI.h"
#include "Canvas.h"

ButtonImageUI::ButtonImageUI(std::shared_ptr<GameObject> containerGO, Rect2D rect) : ItemUI(containerGO, UiType::BUTTONIMAGE, name, false, rect)
{
	this->name = "ButtonImage";
	imagePath = "Assets/Meshes/HUD.png";
	containerGO->GetComponent<Canvas>()->AddTexture(imagePath);
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
	containerGO->GetComponent<Canvas>()->AddTexture(imagePath);
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

	canvas->GetTexture(this->imagePath)->bind();

	//image.get()->bind();

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
	containerGO->GetComponent<Canvas>()->AddTexture(imagePath);

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
	Rect2D imageSect = { 0, 0, 0, 0 };
	if (this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath) != nullptr)
	{
		imageSect.x = imageIdleSection.x * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->width;
		imageSect.y = imageIdleSection.y * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->height;
		imageSect.w = (imageIdleSection.w * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->width);
		imageSect.h = (imageIdleSection.h * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->height);
	}
	return imageSect;
}

void ButtonImageUI::SetSectSizeIdle(float x, float y, float width, float height)
{
	if (this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath) != nullptr)
	{
		imageIdleSection.x = x / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->width;
		imageIdleSection.y = y / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->height;
		imageIdleSection.w = (width) / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->width;
		imageIdleSection.h = (height) / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->height;
	}
}

Rect2D ButtonImageUI::GetSectHovered() const
{
	Rect2D imageSect = { 0, 0, 0, 0 };
	if (this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath) != nullptr)
	{
		imageSect.x = imageHoveredSection.x * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->width;
		imageSect.y = imageHoveredSection.y * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->height;
		imageSect.w = (imageHoveredSection.w * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->width);
		imageSect.h = (imageHoveredSection.h * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->height);
	}
	return imageSect;
}

void ButtonImageUI::SetSectSizeHovered(float x, float y, float width, float height)
{
	if (this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath) != nullptr)
	{
		imageHoveredSection.x = x / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->width;
		imageHoveredSection.y = y / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->height;
		imageHoveredSection.w = (width) / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->width;
		imageHoveredSection.h = (height) / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->height;
	}
}

Rect2D ButtonImageUI::GetSectSelected() const
{
	Rect2D imageSect = { 0, 0, 0, 0 };
	if (this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath) != nullptr)
	{
		imageSect.x = imageSelectedSection.x * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->width;
		imageSect.y = imageSelectedSection.y * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->height;
		imageSect.w = (imageSelectedSection.w * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->width);
		imageSect.h = (imageSelectedSection.h * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->height);
	}
	return imageSect;
}

void ButtonImageUI::SetSectSizeSelected(float x, float y, float width, float height)
{
	if (this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath) != nullptr)
	{
		imageSelectedSection.x = x / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->width;
		imageSelectedSection.y = y / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->height;
		imageSelectedSection.w = (width) / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->width;
		imageSelectedSection.h = (height) / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->height;
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