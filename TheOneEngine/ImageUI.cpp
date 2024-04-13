#include "ImageUI.h"
#include "Canvas.h"

ImageUI::ImageUI(std::shared_ptr<GameObject> containerGO, Rect2D rect) : ItemUI(containerGO, UiType::IMAGE, name, false, rect)
{
	this->name = "Image";
	imagePath = "Assets/Meshes/HUD.png";
	containerGO->GetComponent<Canvas>()->AddTexture(imagePath);
}

ImageUI::ImageUI(std::shared_ptr<GameObject> containerGO, const std::string& path, std::string name, Rect2D rect) : ItemUI(containerGO, UiType::IMAGE, name, false, rect), imagePath(path)
{
	containerGO->GetComponent<Canvas>()->AddTexture(imagePath);
}

ImageUI::~ImageUI(){}

void ImageUI::Draw2D()
{
	auto canvas = containerGO.get()->GetComponent<Canvas>();

	if (canvas->GetTexture(this->imagePath) == nullptr) return;

	float posX = canvas->GetRect().x + GetRect().x;
	float posY = canvas->GetRect().y + GetRect().y;

	float width = (canvas->GetRect().w * imageRect.w);
	float height = (canvas->GetRect().h * imageRect.h);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClear(GL_DEPTH_BUFFER_BIT);

	canvas->GetTexture(this->imagePath)->bind();

	//image.get()->bind();

	glBegin(GL_QUADS);

	glTexCoord2f(textureSection.x, textureSection.y + textureSection.h);  // Top-left corner of the texture
	glVertex2f(posX - width / 2, posY + height / 2);
	
	glTexCoord2f(textureSection.x + textureSection.w, textureSection.y + textureSection.h);  // Top-right corner of the texture
	glVertex2f(posX + width / 2, posY + height / 2);

	glTexCoord2f(textureSection.x + textureSection.w, textureSection.y);  // Bottom-right corner of the texture
	glVertex2f(posX + width / 2, posY - height / 2);

	glTexCoord2f(textureSection.x, textureSection.y);  // Bottom-left corner of the texture
	glVertex2f(posX - width / 2, posY - height / 2);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_DEPTH_TEST);

	glDisable(GL_BLEND);

	glDisable(GL_TEXTURE_2D);
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
	containerGO->GetComponent<Canvas>()->AddTexture(imagePath);
}

Rect2D ImageUI::GetSect() const
{
	Rect2D imageSect = { 0, 0, 0, 0 };
	if (this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath) != nullptr)
	{
		imageSect.x = textureSection.x * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->width;
		imageSect.y = textureSection.y * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->height;
		imageSect.w = (textureSection.w * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->width);
		imageSect.h = (textureSection.h * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->height);
	}

	return imageSect;
}

void ImageUI::SetSectSize(float x, float y, float width, float height)
{
	if (this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath) != nullptr)
	{
		textureSection.x = x / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->width;
		textureSection.y = y / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->height;
		textureSection.w = (width) / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->width;
		textureSection.h = (height) / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->height;
	}
}