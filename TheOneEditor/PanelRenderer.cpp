#include "PanelRenderer.h"
#include "App.h"
#include "Gui.h"

#include "TheOneEngine/Log.h"
#include "TheOneEngine/Renderer.h"
#include "TheOneEngine/Renderer3D.h"
#include "TheOneEngine/Renderer3D.h"
#include "TheOneEngine/RenderTarget.h"

#include "imgui.h"
#include <vector>

PanelRenderer::PanelRenderer(PanelType type, std::string name)
	: Panel(type, name),
	renderTargets(nullptr),
	selectedTarget(nullptr),
	frameBuffers(nullptr),
	selectedBuffer(nullptr),
	attachments(nullptr),
	selectedTab(RendererTab::Settings),
	renderLights(true),
	renderShadows(true),
	renderParticles(true),
	renderCRT(true)
{}

PanelRenderer::~PanelRenderer() {}

void PanelRenderer::Start()
{
	// Set Default Render Target
	renderTargets = Renderer::GetRenderTargets();
	selectedTarget = &renderTargets->front();

	frameBuffers = Renderer::GetFrameBuffers(selectedTarget->GetId());
	selectedBuffer = &frameBuffers->front();

	attachments = selectedBuffer->GetAllAttachments();
}

bool PanelRenderer::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoFocusOnAppearing;

	//ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.f, 0.f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));
	ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Once);

	if (ImGui::Begin("Renderer", &enabled, settingsFlags))
	{
        // Top Bar -------------------------------------------------------------------------
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 10, 10 });
        if (ImGui::BeginMenuBar())
        {
			if (ImGui::MenuItem("Settings"))
			{
				selectedTab = RendererTab::Settings;
			}

			if (ImGui::MenuItem("FrameBuffer"))
			{
				selectedTab = RendererTab::FrameBuffers;
			}

            ImGui::EndMenuBar();
        }
        ImGui::PopStyleVar();

		switch (selectedTab)
		{
			case RendererTab::Settings: SettingsTab(); break;
			case RendererTab::FrameBuffers: FrameBuffersTab(); break;
		}
		
	}
	ImGui::End();
	ImGui::PopStyleVar();

	return true;
}

void PanelRenderer::SettingsTab()
{
	if (ImGui::Checkbox("Lights", &renderLights))
		Renderer::Settings()->lights.SetState(renderLights);

	if (ImGui::Checkbox("Shadows", &renderShadows))
		Renderer::Settings()->shadows.SetState(renderShadows);

	if (ImGui::Checkbox("Particles", &renderParticles))
		Renderer::Settings()->particles.SetState(renderParticles);

	if (ImGui::Checkbox("CRT", &renderCRT))
		Renderer::Settings()->crt.SetState(renderCRT);
}

void PanelRenderer::FrameBuffersTab()
{
	// Select Target
	std::string targetName = selectedTarget ? selectedTarget->GetName() : "None";
	ImGui::SetNextItemWidth(105);
	if (ImGui::BeginCombo("##Target", targetName.c_str()))
	{
		renderTargets = Renderer::GetRenderTargets();

		for (auto& target : *renderTargets)
		{
			bool selected = false;
			if (ImGui::Selectable(target.GetName().c_str(), &selected))
			{
				selectedTarget = &target;
				selectedBuffer = selectedTarget->GetFrameBuffer(selectedBuffer->GetName());
				attachments = selectedBuffer->GetAllAttachments();
			}
		}
		ImGui::EndCombo();
	}

	ImGui::SameLine();

	// Select FrameBuffer
	std::string bufferName = selectedBuffer ? selectedBuffer->GetName() : "None";
	ImGui::SetNextItemWidth(105);
	if (ImGui::BeginCombo("##Buffer", bufferName.c_str()))
	{
		frameBuffers = Renderer::GetFrameBuffers(selectedTarget->GetId());

		for (auto& buffer : *frameBuffers)
		{
			bool selected = false;
			if (ImGui::Selectable(buffer.GetName().c_str(), &selected))
			{
				selectedBuffer = &buffer;
				attachments = selectedBuffer->GetAllAttachments();
			}
		}
		ImGui::EndCombo();
	}

	if (!selectedTarget || !selectedBuffer)
		return;

	// Draw
	ImVec2 availWindowSize = ImGui::GetContentRegionAvail();
	ImVec2 size;
	app->gui->ApplyAspectRatio(availWindowSize.x, (availWindowSize.y - 10) / attachments->size(), &size.x, &size.y, Aspect::A_FREE);

	for (auto& attachment : *attachments)
		ImGui::Image((ImTextureID)attachment.textureId, size, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
}