#include "CheckerUI.h"
#include "Canvas.h"

CheckerUI::CheckerUI(std::shared_ptr<GameObject> containerGO, Rect2D rect) : ItemUI(containerGO, UiType::CHECKER, name, false, rect)
{
	this->name = "Checker";
	imagePath = "Assets/Meshes/HUD.png";
	containerGO->GetComponent<Canvas>()->AddTexture(imagePath);
	UpdateState();
	checkerActive = false;
}

CheckerUI::CheckerUI(std::shared_ptr<GameObject> containerGO, const std::string& path, std::string name, Rect2D rect) : ItemUI(containerGO, UiType::CHECKER, name, false, rect), imagePath(path)
{
	containerGO->GetComponent<Canvas>()->AddTexture(imagePath);
	UpdateState();
	checkerActive = false;
}

CheckerUI::~CheckerUI() {}

void CheckerUI::Draw2D()
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

	canvas->GetTexture(this->imagePath)->Bind();

	//image.get()->Bind();

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

json CheckerUI::SaveUIElement()
{
	json uiElementJSON;

	uiElementJSON["ID"] = (unsigned int)id;
	uiElementJSON["Name"] = name.c_str();
	uiElementJSON["Rect"] = { imageRect.x, imageRect.y, imageRect.w, imageRect.h };
	uiElementJSON["ImageSectionIdle"] = { imageIdleSection.x, imageIdleSection.y, imageIdleSection.w, imageIdleSection.h };
	uiElementJSON["ImageSectionHovered"] = { imageHoveredSection.x, imageHoveredSection.y, imageHoveredSection.w, imageHoveredSection.h };
	uiElementJSON["ImageSectionSelected"] = { imageSelectedSection.x, imageSelectedSection.y, imageSelectedSection.w, imageSelectedSection.h };
	uiElementJSON["ImageSectionIdleOff"] = { imageIdleSectionOff.x, imageIdleSectionOff.y, imageIdleSectionOff.w, imageIdleSectionOff.h };
	uiElementJSON["ImageSectionHoveredOff"] = { imageHoveredSectionOff.x, imageHoveredSectionOff.y, imageHoveredSectionOff.w, imageHoveredSectionOff.h };
	uiElementJSON["ImageSectionSelectedOff"] = { imageSelectedSectionOff.x, imageSelectedSectionOff.y, imageSelectedSectionOff.w, imageSelectedSectionOff.h };
	uiElementJSON["Type"] = (int)type;
	uiElementJSON["State"] = (int)state;
	uiElementJSON["Interactuable"] = interactuable;
	uiElementJSON["CheckerActive"] = checkerActive;

	uiElementJSON["ImagePath"] = imagePath;

	return uiElementJSON;
}

void CheckerUI::LoadUIElement(const json& UIElementJSON)
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
	if (UIElementJSON.contains("ImageSectionIdleOff"))
	{
		imageIdleSectionOff.x = UIElementJSON["ImageSectionIdleOff"][0];
		imageIdleSectionOff.y = UIElementJSON["ImageSectionIdleOff"][1];
		imageIdleSectionOff.w = UIElementJSON["ImageSectionIdleOff"][2];
		imageIdleSectionOff.h = UIElementJSON["ImageSectionIdleOff"][3];
	}
	if (UIElementJSON.contains("ImageSectionHoveredOff"))
	{
		imageHoveredSectionOff.x = UIElementJSON["ImageSectionHoveredOff"][0];
		imageHoveredSectionOff.y = UIElementJSON["ImageSectionHoveredOff"][1];
		imageHoveredSectionOff.w = UIElementJSON["ImageSectionHoveredOff"][2];
		imageHoveredSectionOff.h = UIElementJSON["ImageSectionHoveredOff"][3];
	}
	if (UIElementJSON.contains("ImageSectionSelectedOff"))
	{
		imageSelectedSectionOff.x = UIElementJSON["ImageSectionSelectedOff"][0];
		imageSelectedSectionOff.y = UIElementJSON["ImageSectionSelectedOff"][1];
		imageSelectedSectionOff.w = UIElementJSON["ImageSectionSelectedOff"][2];
		imageSelectedSectionOff.h = UIElementJSON["ImageSectionSelectedOff"][3];
	}
	if (UIElementJSON.contains("Type")) type = (UiType)UIElementJSON["Type"];
	if (UIElementJSON.contains("State")) state = (UiState)UIElementJSON["State"];
	if (UIElementJSON.contains("Interactuable")) interactuable = UIElementJSON["Interactuable"];
	if (UIElementJSON.contains("CheckerActive")) checkerActive = UIElementJSON["CheckerActive"];

	if (UIElementJSON.contains("ImagePath")) imagePath = UIElementJSON["ImagePath"];
	containerGO->GetComponent<Canvas>()->AddTexture(imagePath);

	UpdateState();
}

Rect2D CheckerUI::GetSectIdle() const
{
	Rect2D imageSect = { 0, 0, 0, 0 };
	if (this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath) != nullptr)
	{
		if (checkerActive)
		{
			imageSect.x = imageIdleSection.x * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.y = imageIdleSection.y * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageSect.w = imageIdleSection.w * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.h = imageIdleSection.h * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
		else
		{
			imageSect.x = imageIdleSectionOff.x * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.y = imageIdleSectionOff.y * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageSect.w = imageIdleSectionOff.w * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.h = imageIdleSectionOff.h * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
	}
	return imageSect;
}

void CheckerUI::SetSectSizeIdle(float x, float y, float width, float height)
{
	if (this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath) != nullptr)
	{
		if (checkerActive)
		{
			imageIdleSection.x = x / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageIdleSection.y = y / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageIdleSection.w = width / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageIdleSection.h = height / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
		else
		{
			imageIdleSectionOff.x = x / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageIdleSectionOff.y = y / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageIdleSectionOff.w = width / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageIdleSectionOff.h = height / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
	}
}

Rect2D CheckerUI::GetSectHovered() const
{
	Rect2D imageSect = { 0, 0, 0, 0 };
	if (this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath) != nullptr)
	{
		if (checkerActive)
		{
			imageSect.x = imageHoveredSection.x * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.y = imageHoveredSection.y * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageSect.w = imageHoveredSection.w * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.h = imageHoveredSection.h * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
		else
		{
			imageSect.x = imageHoveredSectionOff.x * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.y = imageHoveredSectionOff.y * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageSect.w = imageHoveredSectionOff.w * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.h = imageHoveredSectionOff.h * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
	}
	return imageSect;
}

void CheckerUI::SetSectSizeHovered(float x, float y, float width, float height)
{
	if (this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath) != nullptr)
	{
		if (checkerActive)
		{
			imageHoveredSection.x = x / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageHoveredSection.y = y / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageHoveredSection.w = width / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageHoveredSection.h = height / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
		else
		{
			imageHoveredSectionOff.x = x / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageHoveredSectionOff.y = y / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageHoveredSectionOff.w = width / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageHoveredSectionOff.h = height / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
	}
}

Rect2D CheckerUI::GetSectSelected() const
{
	Rect2D imageSect = { 0, 0, 0, 0 };
	if (this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath) != nullptr)
	{
		if (checkerActive)
		{
			imageSect.x = imageSelectedSection.x * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.y = imageSelectedSection.y * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageSect.w = imageSelectedSection.w * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.h = imageSelectedSection.h * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
		else
		{
			imageSect.x = imageSelectedSectionOff.x * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.y = imageSelectedSectionOff.y * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageSect.w = imageSelectedSectionOff.w * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.h = imageSelectedSectionOff.h * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
	}
	return imageSect;
}

void CheckerUI::SetSectSizeSelected(float x, float y, float width, float height)
{
	if (this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath) != nullptr)
	{
		if (checkerActive)
		{
			imageSelectedSection.x = x / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSelectedSection.y = y / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageSelectedSection.w = width / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSelectedSection.h = height / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
		else
		{
			imageSelectedSectionOff.x = x / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSelectedSectionOff.y = y / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageSelectedSectionOff.w = width / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSelectedSectionOff.h = height / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
	}
}

void CheckerUI::UpdateState()
{
	switch (state)
	{
	case UiState::IDLE:
		currentSection = (checkerActive ? &imageIdleSection : &imageIdleSectionOff);
		break;
	case UiState::HOVERED:
		currentSection = (checkerActive ? &imageHoveredSection : &imageHoveredSectionOff);
		break;
	case UiState::SELECTED:
		currentSection = (checkerActive ? &imageSelectedSection : &imageSelectedSectionOff);
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