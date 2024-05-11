#pragma once

#include "glm/glm.hpp"
#include "GL/glew.h"

#include <vector>
#include <string>

#define MAX_ATTACHMENTS 8

struct Attachment
{
	enum class Type { RGBA8, RGB16F, DEPTH };

	Type type;
	std::string name;
	GLuint textureId;
};

class FrameBuffer
{
public:
	FrameBuffer(int newWidth, int newHeight, std::vector<Attachment> attachments);
	~FrameBuffer();

	void Bind(bool clear = true);
	void Unbind();

	void Resize(unsigned int newWidth, unsigned int newHeight);
	void TextureParameters();

	void Clear(glm::vec4 color = glm::vec4(0.22, 0.22, 0.22, 1.0));

	inline int GetWidth() { return width; }
	inline int GetHeight() { return height; }

	unsigned int GetAttachmentTexture(const std::string& name) const;

private:
	void GenerateFrameBuffer();

private:
	std::vector<Attachment> attachments;

	unsigned int FBO = 0;
	unsigned int width, height;
};
