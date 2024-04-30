#pragma once

#include "ItemUI.h"
#include "Font.h"
#include <glm/glm.hpp>

class TextUI : public ItemUI
{
public:
	TextUI(std::shared_ptr<GameObject> containerGO, Rect2D rect = { 0,0,50,50 });
	TextUI(std::shared_ptr<GameObject> containerGO, const std::string& path, std::string name = "Text", Rect2D rect = { 0,0,1,1 });
	~TextUI();

	void Draw2D();

	void SetFont(const std::string& path);
	void SetColor(const glm::vec4& c);
	void SetKerning(float k);
	void SetLineSpacing(float spacing);

	json SaveUIElement() override;
	void LoadUIElement(const json& UIElementJSON) override;

	std::string GetPath() { return fontPath; }

private:
	std::string textString = "Lore ipsum";
	glm::vec4 color{ 1.0f };
	float kerning = 1.0f;
	float lineSpacing = 1.0f;

	std::string fontPath;
	std::shared_ptr<Font> fontAsset;
};