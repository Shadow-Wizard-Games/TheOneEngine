#pragma once

#include "panel.h"

#include "TheOneEngine/FrameBuffer.h"

#include <vector>

class RenderTarget;

enum class RendererTab
{
	Settings,
	FrameBuffers
};

class PanelRenderer : public Panel
{
public:
	PanelRenderer(PanelType type, std::string name);
	~PanelRenderer();

	void Start();
	bool Draw();

	void SettingsTab();
	void FrameBuffersTab();

private:
	std::vector<RenderTarget>* renderTargets;
	RenderTarget* selectedTarget;

	std::vector<FrameBuffer>* frameBuffers;
	FrameBuffer* selectedBuffer;

	std::vector<Attachment>* attachments;

	RendererTab selectedTab;

	// Render Settings
	bool renderLights;
	bool renderShadows;
	bool renderParticles;
	bool renderParticlesVL;
	bool renderCRT;
};