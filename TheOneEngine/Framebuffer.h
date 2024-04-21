#pragma once
#include "glm/glm.hpp"

class FrameBuffer
{
public:
	FrameBuffer(int newWidth, int newHeight, bool depth);
	~FrameBuffer();

	void Bind(bool clear = true);
	void Unbind();

	void Resize(unsigned int newWidth, unsigned int newHeight);

	void Clear(glm::vec4 color = glm::vec4(0.25, 0.25, 0.25, 1));
	void ClearBuffer(int value);


	inline int getWidth() { return width; }
	inline int getHeight() { return height; }

	inline unsigned int getColorBufferTexture() { return colorAttachment; }
	inline unsigned int getDepthBufferTexture() { return depthAttachment; }

private:
	void Reset(bool depth);

private:
	unsigned int FBO = 0;
	unsigned int colorAttachment = 0;
	unsigned int depthAttachment = 0;
	unsigned int width, height;

	bool depthActive = false;
	bool initialized = false;
};
