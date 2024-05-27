#pragma once

#include "panel.h"

#include "TheOneEngine/FrameBuffer.h"

#include <vector>

class RenderTarget;

class PanelRenderer : public Panel
{
public:
	PanelRenderer(PanelType type, std::string name);
	~PanelRenderer();

	void Start();
	bool Draw();

private:
	std::vector<RenderTarget>* renderTargets;
	RenderTarget* selectedTarget;

	std::vector<FrameBuffer>* frameBuffers;
	FrameBuffer* selectedBuffer;

	std::vector<Attachment>* attachments;
};