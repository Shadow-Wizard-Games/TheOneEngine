#ifndef __BUTTONIMAGEUI_H__
#define __BUTTONIMAGEUI_H__
#pragma once

#include "ItemUI.h"
#include "Resources.h"
#include <string>

class ButtonImageUI : public ItemUI
{
public:
	ButtonImageUI(std::shared_ptr<GameObject> containerGO, Rect2D rect = { 0,0,1,1 });
	ButtonImageUI(std::shared_ptr<GameObject> containerGO, const std::string& path, std::string name = "ButtonImage", Rect2D rect = { 0,0,1,1 });
	ButtonImageUI(ButtonImageUI* ref);
	~ButtonImageUI();

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

	void UpdateState() override;

	bool IsRealButton() { return this->countAsRealButton; }
	void SetIsRealButton(bool countAsRealButton) { this->countAsRealButton = countAsRealButton; }

private:
	std::string imagePath;
	ResourceId imageID = -1;

	Rect2D* currentSection;
	Rect2D imageIdleSection;
	Rect2D imageHoveredSection;
	Rect2D imageSelectedSection;

	bool countAsRealButton = true;
};

#endif // !__BUTTONIMAGEUI_H__