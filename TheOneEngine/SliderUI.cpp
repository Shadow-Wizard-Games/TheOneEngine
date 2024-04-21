#include "SliderUI.h"
#include "Canvas.h"

SliderUI::SliderUI(std::shared_ptr<GameObject> containerGO, Rect2D rect) : ItemUI(containerGO, UiType::SLIDER, name, false, rect)
{
	this->name = "Slider";
	imagePath = "Assets/Meshes/HUD.png";
	containerGO->GetComponent<Canvas>()->AddTexture(imagePath);
	this->sliderDesign = SliderDesign::SAMEFOREACH;
	UpdateState();
	currentValue = 0;
	maxValue = 0;
	offset = 0.0f;
	additionalOffsetFirst = 0.0f;
	additionalOffsetLast = 0.0f;
}

SliderUI::SliderUI(std::shared_ptr<GameObject> containerGO, const std::string& path, std::string name, Rect2D rect) : ItemUI(containerGO, UiType::SLIDER, name, false, rect), imagePath(path)
{
	containerGO->GetComponent<Canvas>()->AddTexture(imagePath);
	this->sliderDesign = SliderDesign::SAMEFOREACH;
	UpdateState();
	currentValue = 0;
	maxValue = 0;
	offset = 0.0f;
	additionalOffsetFirst = 0.0f;
	additionalOffsetLast = 0.0f;
}

SliderUI::~SliderUI() {}

void SliderUI::Draw2D()
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

	for (size_t i = minValue; i < maxValue; i++)
	{
		glBegin(GL_QUADS);
		if (i < currentValue)
		{
			if (sliderDesign == SliderDesign::DIFFERENTFIRSTANDLAST && i == minValue)
			{
				glTexCoord2f(currentFirstSection->x, currentFirstSection->y + currentFirstSection->h);  // Top-left corner of the texture
				glVertex2f(posX - width / 2, posY + height / 2);

				glTexCoord2f(currentFirstSection->x + currentFirstSection->w, currentFirstSection->y + currentFirstSection->h);  // Top-right corner of the texture
				glVertex2f(posX + width / 2, posY + height / 2);

				glTexCoord2f(currentFirstSection->x + currentFirstSection->w, currentFirstSection->y);  // Bottom-right corner of the texture
				glVertex2f(posX + width / 2, posY - height / 2);

				glTexCoord2f(currentFirstSection->x, currentFirstSection->y);  // Bottom-left corner of the texture
				glVertex2f(posX - width / 2, posY - height / 2);
			}
			else if (sliderDesign == SliderDesign::DIFFERENTFIRSTANDLAST && i == maxValue - 1)
			{
				glTexCoord2f(currentLastSection->x, currentLastSection->y + currentLastSection->h);  // Top-left corner of the texture
				glVertex2f(posX - width / 2 + currentFirstSection->w + currentSection->w * (i - 1) + offset * (i)+additionalOffsetLast, posY + height / 2);

				glTexCoord2f(currentLastSection->x + currentLastSection->w, currentLastSection->y + currentLastSection->h);  // Top-right corner of the texture
				glVertex2f(posX + width / 2 + currentFirstSection->w + currentSection->w * (i - 1) + offset * (i)+additionalOffsetLast, posY + height / 2);

				glTexCoord2f(currentLastSection->x + currentLastSection->w, currentLastSection->y);  // Bottom-right corner of the texture
				glVertex2f(posX + width / 2 + currentFirstSection->w + currentSection->w * (i - 1) + offset * (i)+additionalOffsetLast, posY - height / 2);

				glTexCoord2f(currentLastSection->x, currentLastSection->y);  // Bottom-left corner of the texture
				glVertex2f(posX - width / 2 + currentFirstSection->w + currentSection->w * (i - 1) + offset * (i)+additionalOffsetLast, posY - height / 2);
			}
			else if (sliderDesign == SliderDesign::DIFFERENTFIRSTANDLAST)
			{
				glTexCoord2f(currentSection->x, currentSection->y + currentSection->h);  // Top-left corner of the texture
				glVertex2f(posX - width / 2 + currentFirstSection->w + currentSection->w * (i - 1) + offset * (i)+additionalOffsetFirst, posY + height / 2);

				glTexCoord2f(currentSection->x + currentSection->w, currentSection->y + currentSection->h);  // Top-right corner of the texture
				glVertex2f(posX + width / 2 + currentFirstSection->w + currentSection->w * (i - 1) + offset * (i)+additionalOffsetFirst, posY + height / 2);

				glTexCoord2f(currentSection->x + currentSection->w, currentSection->y);  // Bottom-right corner of the texture
				glVertex2f(posX + width / 2 + currentFirstSection->w + currentSection->w * (i - 1) + offset * (i)+additionalOffsetFirst, posY - height / 2);

				glTexCoord2f(currentSection->x, currentSection->y);  // Bottom-left corner of the texture
				glVertex2f(posX - width / 2 + currentFirstSection->w + currentSection->w * (i - 1) + offset * (i)+additionalOffsetFirst, posY - height / 2);
			}
			else if (sliderDesign == SliderDesign::SAMEFOREACH)
			{
				glTexCoord2f(currentSection->x, currentSection->y + currentSection->h);  // Top-left corner of the texture
				glVertex2f(posX - width / 2 + currentSection->w * i + offset * (i), posY + height / 2);

				glTexCoord2f(currentSection->x + currentSection->w, currentSection->y + currentSection->h);  // Top-right corner of the texture
				glVertex2f(posX + width / 2 + currentSection->w * i + offset * (i), posY + height / 2);

				glTexCoord2f(currentSection->x + currentSection->w, currentSection->y);  // Bottom-right corner of the texture
				glVertex2f(posX + width / 2 + currentSection->w * i + offset * (i), posY - height / 2);

				glTexCoord2f(currentSection->x, currentSection->y);  // Bottom-left corner of the texture
				glVertex2f(posX - width / 2 + currentSection->w * i + offset * (i), posY - height / 2);
			}
		}
		else
		{
			if (sliderDesign == SliderDesign::DIFFERENTFIRSTANDLAST && i == minValue)
			{
				glTexCoord2f(currentFirstSectionOff->x, currentFirstSectionOff->y + currentFirstSectionOff->h);  // Top-left corner of the texture
				glVertex2f(posX - width / 2, posY + height / 2);

				glTexCoord2f(currentFirstSectionOff->x + currentFirstSectionOff->w, currentFirstSectionOff->y + currentFirstSectionOff->h);  // Top-right corner of the texture
				glVertex2f(posX + width / 2, posY + height / 2);

				glTexCoord2f(currentFirstSectionOff->x + currentFirstSectionOff->w, currentFirstSectionOff->y);  // Bottom-right corner of the texture
				glVertex2f(posX + width / 2, posY - height / 2);

				glTexCoord2f(currentFirstSectionOff->x, currentFirstSectionOff->y);  // Bottom-left corner of the texture
				glVertex2f(posX - width / 2, posY - height / 2);
			}
			else if (sliderDesign == SliderDesign::DIFFERENTFIRSTANDLAST && i == maxValue - 1)
			{
				glTexCoord2f(currentLastSectionOff->x, currentLastSectionOff->y + currentLastSectionOff->h);  // Top-left corner of the texture
				glVertex2f(posX - width / 2 + currentFirstSectionOff->w + currentSectionOff->w * (i - 1) + offset * (i)+additionalOffsetLast, posY + height / 2);

				glTexCoord2f(currentLastSectionOff->x + currentLastSectionOff->w, currentLastSectionOff->y + currentLastSectionOff->h);  // Top-right corner of the texture
				glVertex2f(posX + width / 2 + currentFirstSectionOff->w + currentSectionOff->w * (i - 1) + offset * (i)+additionalOffsetLast, posY + height / 2);

				glTexCoord2f(currentLastSectionOff->x + currentLastSectionOff->w, currentLastSectionOff->y);  // Bottom-right corner of the texture
				glVertex2f(posX + width / 2 + currentFirstSectionOff->w + currentSectionOff->w * (i - 1) + offset * (i)+additionalOffsetLast, posY - height / 2);

				glTexCoord2f(currentLastSectionOff->x, currentLastSectionOff->y);  // Bottom-left corner of the texture
				glVertex2f(posX - width / 2 + currentFirstSectionOff->w + currentSectionOff->w * (i - 1) + offset * (i)+additionalOffsetLast, posY - height / 2);
			}
			else if (sliderDesign == SliderDesign::DIFFERENTFIRSTANDLAST)
			{
				glTexCoord2f(currentSectionOff->x, currentSectionOff->y + currentSectionOff->h);  // Top-left corner of the texture
				glVertex2f(posX - width / 2 + currentFirstSectionOff->w + currentSectionOff->w * (i - 1) + offset * (i)+additionalOffsetFirst, posY + height / 2);

				glTexCoord2f(currentSectionOff->x + currentSectionOff->w, currentSectionOff->y + currentSectionOff->h);  // Top-right corner of the texture
				glVertex2f(posX + width / 2 + currentFirstSectionOff->w + currentSectionOff->w * (i - 1) + offset * (i)+additionalOffsetFirst, posY + height / 2);

				glTexCoord2f(currentSectionOff->x + currentSectionOff->w, currentSectionOff->y);  // Bottom-right corner of the texture
				glVertex2f(posX + width / 2 + currentFirstSectionOff->w + currentSectionOff->w * (i - 1) + offset * (i)+additionalOffsetFirst, posY - height / 2);

				glTexCoord2f(currentSectionOff->x, currentSectionOff->y);  // Bottom-left corner of the texture
				glVertex2f(posX - width / 2 + currentFirstSectionOff->w + currentSectionOff->w * (i - 1) + offset * (i)+additionalOffsetFirst, posY - height / 2);
			}
			else if (sliderDesign == SliderDesign::SAMEFOREACH)
			{
				glTexCoord2f(currentSectionOff->x, currentSectionOff->y + currentSectionOff->h);  // Top-left corner of the texture
				glVertex2f(posX - width / 2 + currentSectionOff->w * i + offset * (i), posY + height / 2);

				glTexCoord2f(currentSectionOff->x + currentSectionOff->w, currentSectionOff->y + currentSectionOff->h);  // Top-right corner of the texture
				glVertex2f(posX + width / 2 + currentSectionOff->w * i + offset * (i), posY + height / 2);

				glTexCoord2f(currentSectionOff->x + currentSectionOff->w, currentSectionOff->y);  // Bottom-right corner of the texture
				glVertex2f(posX + width / 2 + currentSectionOff->w * i + offset * (i), posY - height / 2);

				glTexCoord2f(currentSectionOff->x, currentSectionOff->y);  // Bottom-left corner of the texture
				glVertex2f(posX - width / 2 + currentSectionOff->w * i + offset * (i), posY - height / 2);
			}
		}
		glEnd();
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
}

json SliderUI::SaveUIElement()
{
	json uiElementJSON;

	uiElementJSON["ID"] = (unsigned int)id;
	uiElementJSON["Name"] = name.c_str();
	uiElementJSON["Rect"] = { imageRect.x, imageRect.y, imageRect.w, imageRect.h };
	uiElementJSON["ImageSectionIdle"] = { imageIdleSection.x, imageIdleSection.y, imageIdleSection.w, imageIdleSection.h };
	uiElementJSON["ImageSectionHovered"] = { imageHoveredSection.x, imageHoveredSection.y, imageHoveredSection.w, imageHoveredSection.h };
	uiElementJSON["ImageSectionSelected"] = { imageSelectedSection.x, imageSelectedSection.y, imageSelectedSection.w, imageSelectedSection.h };
	uiElementJSON["ImageFirstSectionIdle"] = { imageIdleFirstSection.x, imageIdleFirstSection.y, imageIdleFirstSection.w, imageIdleFirstSection.h };
	uiElementJSON["ImageFirstSectionHovered"] = { imageHoveredFirstSection.x, imageHoveredFirstSection.y, imageHoveredFirstSection.w, imageHoveredFirstSection.h };
	uiElementJSON["ImageFirstSectionSelected"] = { imageSelectedFirstSection.x, imageSelectedFirstSection.y, imageSelectedFirstSection.w, imageSelectedFirstSection.h };
	uiElementJSON["ImageLastSectionIdle"] = { imageIdleLastSection.x, imageIdleLastSection.y, imageIdleLastSection.w, imageIdleLastSection.h };
	uiElementJSON["ImageLastSectionHovered"] = { imageHoveredLastSection.x, imageHoveredLastSection.y, imageHoveredLastSection.w, imageHoveredLastSection.h };
	uiElementJSON["ImageLastSectionSelected"] = { imageSelectedLastSection.x, imageSelectedLastSection.y, imageSelectedLastSection.w, imageSelectedLastSection.h };
	uiElementJSON["ImageSectionIdleOff"] = { imageIdleSectionOff.x, imageIdleSectionOff.y, imageIdleSectionOff.w, imageIdleSectionOff.h };
	uiElementJSON["ImageSectionHoveredOff"] = { imageHoveredSectionOff.x, imageHoveredSectionOff.y, imageHoveredSectionOff.w, imageHoveredSectionOff.h };
	uiElementJSON["ImageSectionSelectedOff"] = { imageSelectedSectionOff.x, imageSelectedSectionOff.y, imageSelectedSectionOff.w, imageSelectedSectionOff.h };
	uiElementJSON["ImageFirstSectionIdleOff"] = { imageIdleFirstSectionOff.x, imageIdleFirstSectionOff.y, imageIdleFirstSectionOff.w, imageIdleFirstSectionOff.h };
	uiElementJSON["ImageFirstSectionHoveredOff"] = { imageHoveredFirstSectionOff.x, imageHoveredFirstSectionOff.y, imageHoveredFirstSectionOff.w, imageHoveredFirstSectionOff.h };
	uiElementJSON["ImageFirstSectionSelectedOff"] = { imageSelectedFirstSectionOff.x, imageSelectedFirstSectionOff.y, imageSelectedFirstSectionOff.w, imageSelectedFirstSectionOff.h };
	uiElementJSON["ImageLastSectionIdleOff"] = { imageIdleLastSectionOff.x, imageIdleLastSectionOff.y, imageIdleLastSectionOff.w, imageIdleLastSectionOff.h };
	uiElementJSON["ImageLastSectionHoveredOff"] = { imageHoveredLastSectionOff.x, imageHoveredLastSectionOff.y, imageHoveredLastSectionOff.w, imageHoveredLastSectionOff.h };
	uiElementJSON["ImageLastSectionSelectedOff"] = { imageSelectedLastSectionOff.x, imageSelectedLastSectionOff.y, imageSelectedLastSectionOff.w, imageSelectedLastSectionOff.h };
	uiElementJSON["Type"] = (int)type;
	uiElementJSON["State"] = (int)state;
	uiElementJSON["SliderDesign"] = (int)sliderDesign;
	uiElementJSON["Value"] = currentValue;
	uiElementJSON["MinValue"] = minValue;
	uiElementJSON["MaxValue"] = maxValue;
	uiElementJSON["Offset"] = offset;
	uiElementJSON["AdditionalOffsetFirst"] = additionalOffsetFirst;
	uiElementJSON["AdditionalOffsetLast"] = additionalOffsetLast;
	uiElementJSON["Interactuable"] = interactuable;

	uiElementJSON["ImagePath"] = imagePath;

	return uiElementJSON;
}

void SliderUI::LoadUIElement(const json& UIElementJSON)
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
	if (UIElementJSON.contains("ImageFirstSectionIdle"))
	{
		imageIdleFirstSection.x = UIElementJSON["ImageFirstSectionIdle"][0];
		imageIdleFirstSection.y = UIElementJSON["ImageFirstSectionIdle"][1];
		imageIdleFirstSection.w = UIElementJSON["ImageFirstSectionIdle"][2];
		imageIdleFirstSection.h = UIElementJSON["ImageFirstSectionIdle"][3];
	}
	if (UIElementJSON.contains("ImageFirstSectionHovered"))
	{
		imageHoveredFirstSection.x = UIElementJSON["ImageFirstSectionHovered"][0];
		imageHoveredFirstSection.y = UIElementJSON["ImageFirstSectionHovered"][1];
		imageHoveredFirstSection.w = UIElementJSON["ImageFirstSectionHovered"][2];
		imageHoveredFirstSection.h = UIElementJSON["ImageFirstSectionHovered"][3];
	}
	if (UIElementJSON.contains("ImageFirstSectionSelected"))
	{
		imageSelectedFirstSection.x = UIElementJSON["ImageFirstSectionSelected"][0];
		imageSelectedFirstSection.y = UIElementJSON["ImageFirstSectionSelected"][1];
		imageSelectedFirstSection.w = UIElementJSON["ImageFirstSectionSelected"][2];
		imageSelectedFirstSection.h = UIElementJSON["ImageFirstSectionSelected"][3];
	}
	if (UIElementJSON.contains("ImageLastSectionIdle"))
	{
		imageIdleLastSection.x = UIElementJSON["ImageLastSectionIdle"][0];
		imageIdleLastSection.y = UIElementJSON["ImageLastSectionIdle"][1];
		imageIdleLastSection.w = UIElementJSON["ImageLastSectionIdle"][2];
		imageIdleLastSection.h = UIElementJSON["ImageLastSectionIdle"][3];
	}
	if (UIElementJSON.contains("ImageLastSectionHovered"))
	{
		imageHoveredLastSection.x = UIElementJSON["ImageLastSectionHovered"][0];
		imageHoveredLastSection.y = UIElementJSON["ImageLastSectionHovered"][1];
		imageHoveredLastSection.w = UIElementJSON["ImageLastSectionHovered"][2];
		imageHoveredLastSection.h = UIElementJSON["ImageLastSectionHovered"][3];
	}
	if (UIElementJSON.contains("ImageLastSectionSelected"))
	{
		imageSelectedLastSection.x = UIElementJSON["ImageLastSectionSelected"][0];
		imageSelectedLastSection.y = UIElementJSON["ImageLastSectionSelected"][1];
		imageSelectedLastSection.w = UIElementJSON["ImageLastSectionSelected"][2];
		imageSelectedLastSection.h = UIElementJSON["ImageLastSectionSelected"][3];
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
	if (UIElementJSON.contains("ImageFirstSectionIdleOff"))
	{
		imageIdleFirstSectionOff.x = UIElementJSON["ImageFirstSectionIdleOff"][0];
		imageIdleFirstSectionOff.y = UIElementJSON["ImageFirstSectionIdleOff"][1];
		imageIdleFirstSectionOff.w = UIElementJSON["ImageFirstSectionIdleOff"][2];
		imageIdleFirstSectionOff.h = UIElementJSON["ImageFirstSectionIdleOff"][3];
	}
	if (UIElementJSON.contains("ImageFirstSectionHoveredOff"))
	{
		imageHoveredFirstSectionOff.x = UIElementJSON["ImageFirstSectionHoveredOff"][0];
		imageHoveredFirstSectionOff.y = UIElementJSON["ImageFirstSectionHoveredOff"][1];
		imageHoveredFirstSectionOff.w = UIElementJSON["ImageFirstSectionHoveredOff"][2];
		imageHoveredFirstSectionOff.h = UIElementJSON["ImageFirstSectionHoveredOff"][3];
	}
	if (UIElementJSON.contains("ImageFirstSectionSelectedOff"))
	{
		imageSelectedFirstSectionOff.x = UIElementJSON["ImageFirstSectionSelectedOff"][0];
		imageSelectedFirstSectionOff.y = UIElementJSON["ImageFirstSectionSelectedOff"][1];
		imageSelectedFirstSectionOff.w = UIElementJSON["ImageFirstSectionSelectedOff"][2];
		imageSelectedFirstSectionOff.h = UIElementJSON["ImageFirstSectionSelectedOff"][3];
	}
	if (UIElementJSON.contains("ImageLastSectionIdleOff"))
	{
		imageIdleLastSectionOff.x = UIElementJSON["ImageLastSectionIdleOff"][0];
		imageIdleLastSectionOff.y = UIElementJSON["ImageLastSectionIdleOff"][1];
		imageIdleLastSectionOff.w = UIElementJSON["ImageLastSectionIdleOff"][2];
		imageIdleLastSectionOff.h = UIElementJSON["ImageLastSectionIdleOff"][3];
	}
	if (UIElementJSON.contains("ImageLastSectionHoveredOff"))
	{
		imageHoveredLastSectionOff.x = UIElementJSON["ImageLastSectionHoveredOff"][0];
		imageHoveredLastSectionOff.y = UIElementJSON["ImageLastSectionHoveredOff"][1];
		imageHoveredLastSectionOff.w = UIElementJSON["ImageLastSectionHoveredOff"][2];
		imageHoveredLastSectionOff.h = UIElementJSON["ImageLastSectionHoveredOff"][3];
	}
	if (UIElementJSON.contains("ImageLastSectionSelectedOff"))
	{
		imageSelectedLastSectionOff.x = UIElementJSON["ImageLastSectionSelectedOff"][0];
		imageSelectedLastSectionOff.y = UIElementJSON["ImageLastSectionSelectedOff"][1];
		imageSelectedLastSectionOff.w = UIElementJSON["ImageLastSectionSelectedOff"][2];
		imageSelectedLastSectionOff.h = UIElementJSON["ImageLastSectionSelectedOff"][3];
	}
	if (UIElementJSON.contains("Type")) type = (UiType)UIElementJSON["Type"];
	if (UIElementJSON.contains("State")) state = (UiState)UIElementJSON["State"];
	if (UIElementJSON.contains("SliderDesign")) sliderDesign = (SliderDesign)UIElementJSON["SliderDesign"];
	if (UIElementJSON.contains("Value")) this->currentValue = UIElementJSON["Value"];
	if (UIElementJSON.contains("MinValue")) this->minValue = UIElementJSON["MinValue"];
	if (UIElementJSON.contains("MaxValue")) this->maxValue = UIElementJSON["MaxValue"];
	if (UIElementJSON.contains("Offset")) this->offset = UIElementJSON["Offset"];
	if (UIElementJSON.contains("AdditionalOffsetFirst")) this->additionalOffsetFirst = UIElementJSON["AdditionalOffsetFirst"];
	if (UIElementJSON.contains("AdditionalOffsetLast")) this->additionalOffsetLast = UIElementJSON["AdditionalOffsetLast"];
	if (UIElementJSON.contains("Interactuable")) interactuable = UIElementJSON["Interactuable"];

	if (UIElementJSON.contains("ImagePath")) imagePath = UIElementJSON["ImagePath"];
	containerGO->GetComponent<Canvas>()->AddTexture(imagePath);

	UpdateState();
}

Rect2D SliderUI::GetSectIdle(int sectionType, bool activePart) const
{
	Rect2D imageSect = { 0, 0, 0, 0 };
	if (this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath) != nullptr)
	{
		if (sectionType == 0)
		{
			imageSect.x = imageIdleSection.x * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.y = imageIdleSection.y * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageSect.w = imageIdleSection.w * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.h = imageIdleSection.h * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
		else if (sectionType == 1)
		{
			imageSect.x = imageIdleFirstSection.x * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.y = imageIdleFirstSection.y * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageSect.w = imageIdleFirstSection.w * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.h = imageIdleFirstSection.h * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
		else if (sectionType == 2)
		{
			imageSect.x = imageIdleLastSection.x * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.y = imageIdleLastSection.y * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageSect.w = imageIdleLastSection.w * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.h = imageIdleLastSection.h * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
	}
	return imageSect;
}

void SliderUI::SetSectSizeIdle(float x, float y, float width, float height, int sectionType, bool activePart)
{
	if (this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath) != nullptr)
	{
		if (sectionType == 0)
		{
			imageIdleSection.x = x / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageIdleSection.y = y / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageIdleSection.w = width / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageIdleSection.h = height / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
		else if (sectionType == 1)
		{
			imageIdleFirstSection.x = x / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageIdleFirstSection.y = y / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageIdleFirstSection.w = width / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageIdleFirstSection.h = height / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
		else if (sectionType == 2)
		{
			imageIdleLastSection.x = x / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageIdleLastSection.y = y / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageIdleLastSection.w = width / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageIdleLastSection.h = height / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
	}
}

Rect2D SliderUI::GetSectHovered(int sectionType, bool activePart) const
{
	Rect2D imageSect = { 0, 0, 0, 0 };
	if (this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath) != nullptr)
	{
		if (sectionType == 0)
		{
			imageSect.x = imageHoveredSection.x * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.y = imageHoveredSection.y * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageSect.w = imageHoveredSection.w * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.h = imageHoveredSection.h * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
		else if (sectionType == 1)
		{
			imageSect.x = imageHoveredFirstSection.x * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.y = imageHoveredFirstSection.y * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageSect.w = imageHoveredFirstSection.w * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.h = imageHoveredFirstSection.h * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
		else if (sectionType == 2)
		{
			imageSect.x = imageHoveredLastSection.x * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.y = imageHoveredLastSection.y * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageSect.w = imageHoveredLastSection.w * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.h = imageHoveredLastSection.h * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
	}
	return imageSect;
}

void SliderUI::SetSectSizeHovered(float x, float y, float width, float height, int sectionType, bool activePart)
{
	if (this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath) != nullptr)
	{
		if (sectionType == 0)
		{
			imageHoveredSection.x = x / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageHoveredSection.y = y / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageHoveredSection.w = width / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageHoveredSection.h = height / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
		else if (sectionType == 1)
		{
			imageHoveredFirstSection.x = x / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageHoveredFirstSection.y = y / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageHoveredFirstSection.w = width / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageHoveredFirstSection.h = height / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
		else if (sectionType == 2)
		{
			imageHoveredLastSection.x = x / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageHoveredLastSection.y = y / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageHoveredLastSection.w = width / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageHoveredLastSection.h = height / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
	}
}

Rect2D SliderUI::GetSectSelected(int sectionType, bool activePart) const
{
	Rect2D imageSect = { 0, 0, 0, 0 };
	if (this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath) != nullptr)
	{
		if (sectionType == 0)
		{
			imageSect.x = imageSelectedSection.x * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.y = imageSelectedSection.y * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageSect.w = imageSelectedSection.w * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.h = imageSelectedSection.h * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
		else if (sectionType == 1)
		{
			imageSect.x = imageSelectedFirstSection.x * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.y = imageSelectedFirstSection.y * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageSect.w = imageSelectedFirstSection.w * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.h = imageSelectedFirstSection.h * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
		else if (sectionType == 2)
		{
			imageSect.x = imageSelectedLastSection.x * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.y = imageSelectedLastSection.y * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageSect.w = imageSelectedLastSection.w * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSect.h = imageSelectedLastSection.h * this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
	}
	return imageSect;
}

void SliderUI::SetSectSizeSelected(float x, float y, float width, float height, int sectionType, bool activePart)
{
	if (this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath) != nullptr)
	{
		if (sectionType == 0)
		{
			imageSelectedSection.x = x / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSelectedSection.y = y / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageSelectedSection.w = width / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSelectedSection.h = height / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
		else if (sectionType == 1)
		{
			imageSelectedFirstSection.x = x / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSelectedFirstSection.y = y / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageSelectedFirstSection.w = width / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSelectedFirstSection.h = height / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
		else if (sectionType == 2)
		{
			imageSelectedLastSection.x = x / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSelectedLastSection.y = y / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
			imageSelectedLastSection.w = width / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().x;
			imageSelectedLastSection.h = height / this->containerGO->GetComponent<Canvas>()->GetTexture(this->imagePath)->GetSize().y;
		}
	}
}

void SliderUI::UpdateState()
{
	switch (this->state)
	{
	case UiState::IDLE:
		currentSection = &imageIdleSection;
		currentFirstSection = &imageIdleFirstSection;
		currentLastSection = &imageIdleLastSection;

		currentSectionOff = &imageIdleSectionOff;
		currentFirstSectionOff = &imageIdleFirstSectionOff;
		currentLastSectionOff = &imageIdleLastSectionOff;
		break;
	case UiState::HOVERED:
		currentSection = &imageHoveredSection;
		currentFirstSection = &imageHoveredFirstSection;
		currentLastSection = &imageHoveredLastSection;

		currentSectionOff = &imageHoveredSectionOff;
		currentFirstSectionOff = &imageHoveredFirstSectionOff;
		currentLastSectionOff = &imageHoveredLastSectionOff;
		break;
	case UiState::SELECTED:
		currentSection = &imageSelectedSection;
		currentFirstSection = &imageSelectedFirstSection;
		currentLastSection = &imageSelectedLastSection;

		currentSectionOff = &imageSelectedSectionOff;
		currentFirstSectionOff = &imageSelectedFirstSectionOff;
		currentLastSectionOff = &imageSelectedLastSectionOff;
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