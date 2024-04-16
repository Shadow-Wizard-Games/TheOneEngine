#ifndef __SLIDERUI_H__
#define __SLIDERUI_H__
#pragma once

#include "ItemUI.h"
#include "Texture.h"
#include <string>

enum class SliderDesign
{
	SAMEFOREACH,
	DIFFERENTFIRSTANDLAST,
	DEFAULT
};

class SliderUI : public ItemUI
{
public:
	SliderUI(std::shared_ptr<GameObject> containerGO, Rect2D rect = { 0,0,1,1 });
	SliderUI(std::shared_ptr<GameObject> containerGO, const std::string& path, std::string name = "Slider", Rect2D rect = { 0,0,1,1 });
	~SliderUI();

	void Draw2D();

	json SaveUIElement() override;
	void LoadUIElement(const json& UIElementJSON) override;

	// @Get / Set --------------------------------
	
	//idle section
	//sectionType = 0 -> normal section
	//sectionType = 1 -> first section
	//sectionType = 2 -> last section
	Rect2D GetSectIdle(int sectionType = 0) const;
	//sectionType = 0 -> normal section
	//sectionType = 1 -> first section
	//sectionType = 2 -> last section
	void SetSectSizeIdle(float x, float y, float width, float height, int sectionType = 0);
	//hovered section
	//sectionType = 0 -> normal section
	//sectionType = 1 -> first section
	//sectionType = 2 -> last section
	Rect2D GetSectHovered(int sectionType = 0) const;
	//sectionType = 0 -> normal section
	//sectionType = 1 -> first section
	//sectionType = 2 -> last section
	void SetSectSizeHovered(float x, float y, float width, float height, int sectionType = 0);
	//selected section
	//sectionType = 0 -> normal section
	//sectionType = 1 -> first section
	//sectionType = 2 -> last section
	Rect2D GetSectSelected(int sectionType = 0) const;
	//sectionType = 0 -> normal section
	//sectionType = 1 -> first section
	//sectionType = 2 -> last section
	void SetSectSizeSelected(float x, float y, float width, float height, int sectionType = 0);

	//path
	std::string GetPath()
	{
		return imagePath;
	}

	//slider design
	void SetSliderDesign(SliderDesign sliderDesign)
	{
		this->sliderDesign = sliderDesign;
	}
	SliderDesign GetSliderDesign()
	{
		return this->sliderDesign;
	}

	//max value
	void SetMaxValue(int maxValue)
	{
		maxValue < this->currentValue ? this->maxValue = this->currentValue : this->maxValue = maxValue;
	}
	int GetMaxValue()
	{
		return this->maxValue;
	}

	//current value
	void SetValue(int currentValue)
	{
		if (currentValue < this->minValue)
		{
			this->currentValue = this->minValue;
		}
		else if (currentValue > this->maxValue)
		{
			this->currentValue = this->maxValue;
		}
		else
		{
			this->currentValue = currentValue;
		}
	}
	int GetValue()
	{
		return this->currentValue;
	}

	//offset
	void SetOffset(float offset)
	{
		this->offset = offset;
	}
	float GetOffset()
	{
		return this->offset;
	}

	//additional offset first
	void SetAdditionalOffsetFirst(float offset)
	{
		this->additionalOffsetFirst = offset;
	}
	float GetAdditionalOffsetFirst()
	{
		return this->additionalOffsetFirst;
	}

	//additional offset last
	void SetAdditionalOffsetLast(float offset)
	{
		this->additionalOffsetLast = offset;
	}
	float GetAdditionalOffsetLast()
	{
		return this->additionalOffsetLast;
	}


	////
	void UpdateState() override;

private:
	std::string imagePath;
	//std::unique_ptr<Texture> image;

	//section of each slider part
	Rect2D* currentSection;
	Rect2D imageIdleSection;
	Rect2D imageHoveredSection;
	Rect2D imageSelectedSection;

	//design of the slider
	SliderDesign sliderDesign;

	//section of first and last slider part
	Rect2D* currentFirstSection;
	Rect2D imageIdleFirstSection;
	Rect2D imageHoveredFirstSection;
	Rect2D imageSelectedFirstSection;
	//
	Rect2D* currentLastSection;
	Rect2D imageIdleLastSection;
	Rect2D imageHoveredLastSection;
	Rect2D imageSelectedLastSection;
	////

	//dont modify this value, minValue must be 0
	int minValue = 0;
	int maxValue;
	int currentValue;

	float offset;
	float additionalOffsetFirst;
	float additionalOffsetLast;
};

#endif // !__SLIDERUI_H__