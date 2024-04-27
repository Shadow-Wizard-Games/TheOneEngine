#ifndef __TEXTURE_H__
#define __TEXTURE_H__
#pragma once

#include "Defs.h"
#include "Component.h"

#include <string>
#include <memory>

class Texture
{
private:
	uint32_t textureID;
	glm::ivec2 imageSize;
	int textureChannels;

	bool Init(const std::string& path);
	bool InitDDS(const std::string& path);

public:
	Texture();
	Texture(const std::string& path);
	~Texture();

	void Bind(uint32_t slot = 0) const;

	void SetData(void* data, uint32_t size);

	void Delete();

	bool CreateCheckerTexture();

	uint32_t GetTextureId() const { return textureID; }
	int GetChannels() { return textureChannels; }

	int GetWidth() { return imageSize.x; }
	int GetHeight() { return imageSize.y; }
	glm::ivec2 GetSize() { return imageSize; }

	std::string path;

	bool operator==(const Texture& other) const
	{
		return textureID == other.GetTextureId();
	}

	// Image utils
	static void raw_to_dds_file(const char* filename, const unsigned char* pData, int width, int height, int bpp);
	static unsigned char* raw_to_dds_mem(const unsigned char* data, int width, int height, int bpp, size_t* outSize);
	static void free_dds_mem(unsigned char* data);
};

#endif // !__TEXTURE_H__