#pragma once

#include "Defs.h"
#include "FrameBuffer.h"
#include "GameObject.h"

#include <vector>
#include <string>

class RenderTarget
{
public:
	RenderTarget(unsigned int id, DrawMode mode, Camera* camera, glm::vec2 viewportSize, std::vector<std::vector<Attachment>> frameBuffers);
	~RenderTarget();

	std::vector<FrameBuffer>* GetFrameBuffers();
	FrameBuffer* GetFrameBuffer(std::string name);
	Camera* GetCamera() { return camera; }

private:
	unsigned int id;
	DrawMode mode;
	Camera* camera;

	glm::vec2 viewportSize;
	std::vector<FrameBuffer> frameBuffers;
};