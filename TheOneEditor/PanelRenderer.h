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

	bool GetDrawInScene() { return drawInScene; }
	Attachment* GetSelectedAttachment() { return selectedAttachment; }

private:
	RendererTab selectedTab;

	std::vector<RenderTarget>* renderTargets;
	RenderTarget* selectedTarget;

	std::vector<FrameBuffer>* frameBuffers;
	FrameBuffer* selectedBuffer;

	std::vector<Attachment>* attachments;
	Attachment* selectedAttachment;

	bool drawInScene;

	// Render Settings
	bool renderLights;
	bool renderShadows;
	bool renderParticles;
	bool renderParticlesVL;
	bool renderCRT;
};