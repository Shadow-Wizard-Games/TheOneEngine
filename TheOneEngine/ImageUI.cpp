#include "ImageUI.h"
#include "Canvas.h"
#include "Renderer2D.h"

ImageUI::ImageUI(std::shared_ptr<GameObject> containerGO, Rect2D rect) : ItemUI(containerGO, UiType::IMAGE, name, false, rect)
{
	this->name = "Img";
	imagePath = "Assets/Meshes/HUD.png";
	imageID = Resources::Load<Texture>(imagePath);
}

ImageUI::ImageUI(std::shared_ptr<GameObject> containerGO, const std::string& path, std::string name, Rect2D rect) : ItemUI(containerGO, UiType::IMAGE, name, false, rect), imagePath(path)
{
	imageID = Resources::Load<Texture>(imagePath);
}

ImageUI::ImageUI(ImageUI* ref) : ItemUI(ref)
{
	this->imagePath = ref->imagePath;
	this->imageID = ref->imageID;
	this->textureSection = ref->textureSection;
}

ImageUI::~ImageUI(){}

void ImageUI::Draw2D()
{
	auto canvas = containerGO.get()->GetComponent<Canvas>();

	if (imageID == -1) return;

	float posX = canvas->GetRect().x + GetRect().x;
	float posY = canvas->GetRect().y + GetRect().y;

	float width = (canvas->GetRect().w * imageRect.w);
	float height = (canvas->GetRect().h * imageRect.h);

	Renderer2D::DrawQuad({ posX, posY }, { width, height }, imageID, Rect2DToTexCoordsSection(textureSection));
}

json ImageUI::SaveUIElement()
{
	json uiElementJSON;

	uiElementJSON["ID"] = (unsigned int)id;
	uiElementJSON["Name"] = name.c_str();
	uiElementJSON["Rect"] = { imageRect.x, imageRect.y, imageRect.w, imageRect.h };
	uiElementJSON["ImageSection"] = { textureSection.x, textureSection.y, textureSection.w, textureSection.h };
	uiElementJSON["Type"] = (int)type;
	uiElementJSON["State"] = (int)state;
	uiElementJSON["Interactuable"] = interactuable;	


	uiElementJSON["ImagePath"] = imagePath;

	return uiElementJSON;
}

void ImageUI::LoadUIElement(const json& UIElementJSON)
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
	if (UIElementJSON.contains("ImageSection"))
	{
		textureSection.x = UIElementJSON["ImageSection"][0];
		textureSection.y = UIElementJSON["ImageSection"][1];
		textureSection.w = UIElementJSON["ImageSection"][2];
		textureSection.h = UIElementJSON["ImageSection"][3];
	}
	if (UIElementJSON.contains("Type")) type = (UiType)UIElementJSON["Type"];
	if (UIElementJSON.contains("State")) state = (UiState)UIElementJSON["State"];
	if (UIElementJSON.contains("Interactuable")) interactuable = UIElementJSON["Interactuable"];

	if (UIElementJSON.contains("ImagePath")) imagePath = UIElementJSON["ImagePath"];
	imageID = Resources::Load<Texture>(imagePath);
}

Rect2D ImageUI::GetSect() const {
	Texture* texture = Resources::GetResourceById<Texture>(imageID);
	if (imageID != -1) {
		return GetImageSection(textureSection, Rect2D{}, true, texture);
	}
	return { 0, 0, 0, 0 };
}

void ImageUI::SetSectSize(float x, float y, float width, float height) {
	Texture* texture = Resources::GetResourceById<Texture>(imageID);
	if (imageID != -1) {
		Rect2D inactive;
		SetImageSection(textureSection, inactive, true, x, y, width, height, texture);
	}
}