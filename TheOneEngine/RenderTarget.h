#pragma once

#include "Defs.h"
#include "GameObject.h"
#include "Framebuffer.h"

#include <vector>

class RenderTarget
{
public:
	RenderTarget(unsigned int id, DrawMode mode, Camera* camera, glm::vec2 viewportSize, std::vector<std::vector<Attachment>> frameBuffers);
	~RenderTarget();

	std::vector<FrameBuffer>* GetFrameBuffers();
	FrameBuffer* GetFrameBuffer(std::string name);

private:
	uint id;
	DrawMode mode;
	Camera* camera;

	glm::vec2 viewportSize;
	std::vector<FrameBuffer> frameBuffers;
};