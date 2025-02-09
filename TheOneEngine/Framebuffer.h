#pragma once

#include "glm/glm.hpp"
#include "GL/glew.h"

#include <vector>
#include <string>

#define MAX_ATTACHMENTS 8

struct Attachment
{
	enum class Type { RGBA8, RGB16F, DEPTH, DEPTH_STENCIL };

	Type type;
	std::string name;
	std::string frameBufferName;
	GLuint textureId;
};

enum class ClearBit : unsigned int
{
	Color = GL_COLOR_BUFFER_BIT,
	Depth = GL_DEPTH_BUFFER_BIT,
	Stencil = GL_STENCIL_BUFFER_BIT,
	All = Color | Depth | Stencil
};

class FrameBuffer
{
public:
	FrameBuffer(std::string name, int width, int height, std::vector<Attachment> attachments);
	~FrameBuffer();

	void Bind(bool clear = false);
	void Unbind();

	void Resize(unsigned int newWidth, unsigned int newHeight);
	void TextureParameters();

	void Clear(ClearBit flag = ClearBit::All, glm::vec4 color = { 0.22, 0.22, 0.22, 1.0 });

	inline std::string GetName() { return name; }

	inline int GetWidth() { return width; }
	inline int GetHeight() { return height; }

	unsigned int GetBuffer() const { return FBO; }

	unsigned int GetAttachmentTexture(const std::string& name) const;
	std::vector<Attachment>* GetAllAttachments() { return &attachments; }

private:
	void GenerateFrameBuffer();

private:
	unsigned int FBO = 0;
	std::string name;

	unsigned int width, height;

	std::vector<Attachment> attachments;
};

inline ClearBit operator|(ClearBit lhs, ClearBit rhs)
{
	return static_cast<ClearBit>(
		static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}

inline ClearBit& operator|=(ClearBit& lhs, ClearBit rhs)
{
	lhs = lhs | rhs;
	return lhs;
}