#include "CheckerUI.h"
#include "Canvas.h"
#include "Renderer2D.h"

CheckerUI::CheckerUI(std::shared_ptr<GameObject> containerGO, Rect2D rect) : ItemUI(containerGO, UiType::CHECKER, name, false, rect)
{
	this->name = "Checker";
	imagePath = "Assets/Meshes/HUD.png";
	imageID = Resources::Load<Texture>(imagePath);
	UpdateState();
	checkerActive = false;
}

CheckerUI::CheckerUI(std::shared_ptr<GameObject> containerGO, const std::string& path, std::string name, Rect2D rect) : ItemUI(containerGO, UiType::CHECKER, name, false, rect), imagePath(path)
{
	imageID = Resources::Load<Texture>(imagePath);
	UpdateState();
	checkerActive = false;
}

CheckerUI::CheckerUI(CheckerUI* ref) : ItemUI(ref)
{
	this->imagePath = ref->imagePath;
	this->imageID = ref->imageID;

	this->imageIdleSection = ref->imageIdleSection;
	this->imageHoveredSection = ref->imageHoveredSection;
	this->imageSelectedSection = ref->imageSelectedSection;

	this->imageIdleSectionOff = ref->imageIdleSectionOff;
	this->imageHoveredSectionOff = ref->imageHoveredSectionOff;
	this->imageSelectedSectionOff = ref->imageSelectedSectionOff;

	this->currentSection = ref->currentSection;

	this->checkerActive = ref->checkerActive;
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

	Renderer2D::DrawQuad(BatchType::UI, { posX, posY }, { width, height }, imageID, Rect2DToTexCoordsSection(*currentSection));
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
	uiElementJSON["Print"] = print;
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
	if (UIElementJSON.contains("Print")) print = UIElementJSON["Print"];
	if (UIElementJSON.contains("CheckerActive")) checkerActive = UIElementJSON["CheckerActive"];

	if (UIElementJSON.contains("ImagePath")) imagePath = UIElementJSON["ImagePath"];
	imageID = Resources::Load<Texture>(imagePath);

	UpdateState();
}

Rect2D CheckerUI::GetSectIdle() const {
	Texture* texture = Resources::GetResourceById<Texture>(imageID);
	if (imageID != -1) {
		return GetImageSection(imageIdleSection, imageIdleSectionOff, checkerActive, texture);
	}
	return { 0, 0, 0, 0 };
}

void CheckerUI::SetSectSizeIdle(float x, float y, float width, float height) {
	Texture* texture = Resources::GetResourceById<Texture>(imageID);
	if (imageID != -1) {
		SetImageSection(imageIdleSection, imageIdleSectionOff, checkerActive, x, y, width, height, texture);
	}
}

Rect2D CheckerUI::GetSectHovered() const {
	Texture* texture = Resources::GetResourceById<Texture>(imageID);
	if (imageID != -1) {
		return GetImageSection(imageHoveredSection, imageHoveredSectionOff, checkerActive, texture);
	}
	return { 0, 0, 0, 0 };
}

void CheckerUI::SetSectSizeHovered(float x, float y, float width, float height) {
	Texture* texture = Resources::GetResourceById<Texture>(imageID);
	if (imageID != -1) {
		SetImageSection(imageHoveredSection, imageHoveredSectionOff, checkerActive, x, y, width, height, texture);
	}
}

Rect2D CheckerUI::GetSectSelected() const {
	Texture* texture = Resources::GetResourceById<Texture>(imageID);
	if (imageID != -1) {
		return GetImageSection(imageSelectedSection, imageSelectedSectionOff, checkerActive, texture);
	}
	return { 0, 0, 0, 0 };
}

void CheckerUI::SetSectSizeSelected(float x, float y, float width, float height) {
	Texture* texture = Resources::GetResourceById<Texture>(imageID);
	if (imageID != -1) {
		SetImageSection(imageSelectedSection, imageSelectedSectionOff, checkerActive, x, y, width, height, texture);
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