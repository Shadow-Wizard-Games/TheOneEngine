#pragma once
#include "glm/glm.hpp"

class FrameBuffer
{
public:
	FrameBuffer(int newWidth, int newHeight, bool depth = true, bool normal = false, bool position = false);
	~FrameBuffer();

	void Bind(bool clear = true);
	void Unbind();

	void Resize(unsigned int newWidth, unsigned int newHeight);

	void Clear(glm::vec4 color = glm::vec4(0.22, 0.22, 0.22, 1.0));
	void ClearBuffer(int value);


	inline int getWidth() { return width; }
	inline int getHeight() { return height; }

	inline unsigned int getColorBufferTexture() { return colorAttachment; }
	inline unsigned int getPositionBufferTexture() { return positionAttachment; }
	inline unsigned int getNormalBufferTexture() { return normalAttachment; }
	inline unsigned int getDepthBufferTexture() { return depthAttachment; }

private:
	void Reset(bool depth, bool normal, bool position);

private:
	unsigned int FBO = 0;
	unsigned int colorAttachment = 0;
	unsigned int positionAttachment = 0;
	unsigned int normalAttachment = 0;
	unsigned int depthAttachment = 0;
	unsigned int width, height;

	bool depthActive = false;
	bool positionActive = false;
	bool normalActive = false;
	bool initialized = false;
};
