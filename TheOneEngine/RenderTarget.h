#pragma once

#include "Defs.h"
#include "FrameBuffer.h"
#include "GameObject.h"

#include <vector>
#include <string>

class RenderTarget
{
public:
	RenderTarget(unsigned int id, std::string name, DrawMode mode, Camera* camera, glm::vec2 viewportSize, std::vector<std::vector<Attachment>> frameBuffers);
	~RenderTarget();

	std::vector<FrameBuffer>* GetFrameBuffers();
	FrameBuffer* GetFrameBuffer(std::string name);

	unsigned int GetId() { return id; }
	std::string GetName() { return name; }
	DrawMode GetMode() { return mode; }
	Camera* GetCamera() { return camera; }

private:
	unsigned int id;
	std::string name;
	DrawMode mode;
	Camera* camera;

	glm::vec2 viewportSize;
	std::vector<FrameBuffer> frameBuffers;
};