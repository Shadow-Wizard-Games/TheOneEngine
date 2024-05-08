#pragma once

#include <filesystem>
#include "Texture.h"

struct MSDFData;

class Font
{
public:
	Font(const std::filesystem::path& font);
	~Font();

	const MSDFData* GetMSDFData() const { return m_Data; }
	std::shared_ptr<Texture> GetAtlasTexture() const { return m_AtlasTexture; }

	static std::shared_ptr<Font> GetDefault();
private:
	MSDFData* m_Data;
	std::shared_ptr<Texture> m_AtlasTexture;
};
