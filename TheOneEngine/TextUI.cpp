#include "TextUI.h"
#include "EngineCore.h"
#include "Renderer2D.h"

#include <glm/glm.hpp>

TextUI::TextUI(std::shared_ptr<GameObject> containerGO, Rect2D rect) : ItemUI(containerGO, UiType::TEXT, name, false, rect)
{
	SetFont("Assets/Fonts/ComicSansMS.ttf");
}

TextUI::TextUI(std::shared_ptr<GameObject> containerGO, const std::string& path, std::string name, Rect2D rect) : ItemUI(containerGO, UiType::TEXT, name, false, rect), fontPath(path)
{
	SetFont(path);
	LOG(LogType::LOG_INFO, (fontAsset->GetAtlasTexture()->GetTextureId() == 0 ? ("Failed to set new font '%s' to the TextUI") : ("Succesfully set new font '%s' to the TextUI")), path.c_str());
}

TextUI::~TextUI() {}

void TextUI::Draw2D()
{
	Renderer2D::TextParams params;
	params.Color = color;
	params.Kerning = kerning;
	params.LineSpacing = lineSpacing;

	Renderer2D::DrawString(textString, fontAsset, { imageRect.x, imageRect.y }, { imageRect.w, imageRect.h }, params);
	Renderer2D::DrawRect({ imageRect.x, imageRect.y, 0.0f }, { imageRect.w, imageRect.h }, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
}

void TextUI::SetFont(const std::string& path)
{
	if (path.empty())
		return;

	fontPath = path;
	fontAsset = std::make_shared<Font>(path);
}

void TextUI::SetColor(const glm::vec4& c)
{
	color = c;
}

void TextUI::SetKerning(float k)
{
	kerning = k;
}

void TextUI::SetLineSpacing(float spacing)
{
	lineSpacing = spacing;
}

json TextUI::SaveUIElement()
{
	json uiFontJSON;

	uiFontJSON["ID"] = (unsigned int)id;
	uiFontJSON["Name"] = name.c_str();
	uiFontJSON["Rect"] = { imageRect.x, imageRect.y, imageRect.w, imageRect.h };
	uiFontJSON["Type"] = (int)type;
	uiFontJSON["State"] = (int)state;
	uiFontJSON["Interactuable"] = interactuable;
	uiFontJSON["FontPath"] = fontPath;
	uiFontJSON["Color"] = { color.r, color.g, color.b, color.a };
	uiFontJSON["Kerning"] = kerning;
	uiFontJSON["LineSpacing"] = lineSpacing;

	return uiFontJSON;
}

void TextUI::LoadUIElement(const json& UIElementJSON)
{
	if (UIElementJSON.contains("Name")) name = UIElementJSON["Name"];
	if (UIElementJSON.contains("ID")) id = (unsigned int)UIElementJSON["ID"];
	if (UIElementJSON.contains("Rect"))
	{
		imageRect.x = UIElementJSON["Rect"][0];
		imageRect.y = UIElementJSON["Rect"][1];
		imageRect.w = UIElementJSON["Rect"][2];
		imageRect.h = UIElementJSON["Rect"][3];
	}
	if (UIElementJSON.contains("Type")) type = (UiType)UIElementJSON["Type"];
	if (UIElementJSON.contains("State")) state = (UiState)UIElementJSON["State"];
	if (UIElementJSON.contains("Interactuable")) interactuable = UIElementJSON["Interactuable"];

	if (UIElementJSON.contains("FontPath")) fontPath = UIElementJSON["FontPath"];
	if (UIElementJSON.contains("Color"))
	{
		for (int i = 0; i < 4; i++) {
			color[i] = UIElementJSON["Color"][i];
		}

	}
	if (UIElementJSON.contains("Kerning")) kerning = UIElementJSON["Kerning"];
	if (UIElementJSON.contains("LineSpacing")) lineSpacing = UIElementJSON["LineSpacing"];

	SetFont(fontPath);
}