#ifndef __BUTTONIMAGEUI_H__
#define __BUTTONIMAGEUI_H__
#pragma once

#include "ItemUI.h"
#include "Texture.h"
#include <string>

class ButtonImageUI : public ItemUI
{
public:
	ButtonImageUI(std::shared_ptr<GameObject> containerGO, Rect2D rect = { 0,0,1,1 });
	ButtonImageUI(std::shared_ptr<GameObject> containerGO, const std::string& path, std::string name = "ButtonImage", Rect2D rect = { 0,0,1,1 });
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

private:
	std::string imagePath;
	std::unique_ptr<Texture> image;

	Rect2D* currentSection;
	Rect2D imageIdleSection;
	Rect2D imageHoveredSection;
	Rect2D imageSelectedSection;
};

#endif // !__BUTTONIMAGEUI_H__