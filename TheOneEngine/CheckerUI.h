#ifndef __CHECKEREUI_H__
#define __CHECKEREUI_H__
#pragma once

#include "ItemUI.h"
#include "Texture.h"
#include <string>

class CheckerUI : public ItemUI
{
public:
	CheckerUI(std::shared_ptr<GameObject> containerGO, Rect2D rect = { 0,0,1,1 });
	CheckerUI(std::shared_ptr<GameObject> containerGO, const std::string& path, std::string name = "Checker", Rect2D rect = { 0,0,1,1 });
	~CheckerUI();

	void Draw2D();

	json SaveUIElement() override;
	void LoadUIElement(const json& UIElementJSON) override;

	// @Get / Set --------------------------------
	//idle section
	Rect2D GetSectIdle() const;
	void SetSectSizeIdle(float x, float y, float width, float height);
	//hovered section
	Rect2D GetSectHovered() const;
	void SetSectSizeHovered(float x, float y, float width, float height);
	//selected section
	Rect2D GetSectSelected() const;
	void SetSectSizeSelected(float x, float y, float width, float height);

	std::string GetPath()
	{
		return imagePath;
	}

	void SetChecker(bool state)
	{
		this->checkerActive = state;
		UpdateState();
	}

	bool GetChecker()
	{
		return this->checkerActive;
	}

	void UpdateState() override;

private:
	std::string imagePath;
	//std::unique_ptr<Texture> image;

	Rect2D* currentSection;

	//checker in on
	Rect2D imageIdleSection;
	Rect2D imageHoveredSection;
	Rect2D imageSelectedSection;
	//checker in off
	Rect2D imageIdleSectionOff;
	Rect2D imageHoveredSectionOff;
	Rect2D imageSelectedSectionOff;

	bool checkerActive;
};

#endif // !__CHECKERUI_H__