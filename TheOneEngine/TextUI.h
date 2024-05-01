#pragma once

#include "ItemUI.h"
#include "Font.h"
#include <glm/glm.hpp>

class TextUI : public ItemUI
{
public:
	TextUI(std::shared_ptr<GameObject> containerGO, Rect2D rect = { 0,0,0.1,0.1 });
	TextUI(std::shared_ptr<GameObject> containerGO, const std::string& path, std::string name = "Text", Rect2D rect = { 0,0,0.1,0.1 });
	~TextUI();

	void Draw2D();

	void SetText(const std::string& newText);
	void SetFont(const std::string& path);
	void SetColor(const glm::vec4& c);
	void SetKerning(float k);
	void SetLineSpacing(float spacing);

	json SaveUIElement() override;
	void LoadUIElement(const json& UIElementJSON) override;

	std::string GetPath() { return fontPath; }

private:
	std::string textString = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Id velit ut tortor pretium viverra. Duis at tellus at urna condimentum. Aenean pharetra magna ac placerat. Dolor morbi non arcu risus quis varius quam quisque. Quam viverra orci sagittis eu volutpat odio facilisis. Ut porttitor leo a diam sollicitudin tempor id eu. Netus et malesuada fames ac turpis egestas integer. Adipiscing diam donec adipiscing tristique risus nec feugiat in fermentum. Cras tincidunt lobortis feugiat vivamus at augue eget. Et netus et malesuada fames ac turpis egestas.";
	glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	float kerning = 0.0f;
	float lineSpacing = 0.0f;

	std::string fontPath;
	std::shared_ptr<Font> fontAsset;
};