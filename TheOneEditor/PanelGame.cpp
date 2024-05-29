#include "PanelGame.h"
#include "App.h"
#include "Gui.h"
#include "SceneManager.h"
#include "Window.h"
#include "imgui.h"

#include "TheOneEngine/EngineCore.h"
#include "TheOneEngine/Renderer.h"
#include "TheOneEngine/Renderer3D.h"
#include "TheOneEngine/RenderTarget.h"

PanelGame::PanelGame(PanelType type, std::string name) : Panel(type, name)
{
	currentScene = nullptr;
	gameCamera = nullptr;
	viewportSize = { 0.0f, 0.0f };
	renderTarget = -1;
	isHovered = false;
	isFocused = false;
	aspect = Aspect::A_16x9;
}

PanelGame::~PanelGame() {}

void PanelGame::Start()
{
	currentScene = engine->N_sceneManager->currentScene;

	// Set Game Camera
	std::vector<GameObject*> gameCameras = GetGameCameras();

	gameCamera = gameCameras.empty() ?
		engine->N_sceneManager->currentScene->currentCamera : gameCameras.front()->GetComponent<Camera>();
	
	// Create Render Target
	std::vector<Attachment> gBuffAttachments = {
		{ Attachment::Type::RGBA8, "color", "gBuffer", 0 },
		{ Attachment::Type::RGB16F, "position", "gBuffer", 0 },
		{ Attachment::Type::RGB16F, "normal", "gBuffer", 0 },
		{ Attachment::Type::DEPTH_STENCIL, "depth", "gBuffer", 0 }
	};
	std::vector<Attachment> postBuffAttachments = {
		{ Attachment::Type::RGBA8, "color", "postBuffer", 0 },
		{ Attachment::Type::DEPTH_STENCIL, "depth", "postBuffer", 0 }
	};
	std::vector<Attachment> uiBuffAttachments = {
		{ Attachment::Type::RGBA8, "color", "uiBuffer", 0 }
	};

	std::vector<std::vector<Attachment>> gameBuffers{ gBuffAttachments, postBuffAttachments, uiBuffAttachments };

	viewportSize = { 680, 360 };
	renderTarget = Renderer::AddRenderTarget("Panel Game", DrawMode::GAME, gameCamera, viewportSize, gameBuffers);
}

bool PanelGame::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
	ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Once);

	if (ImGui::Begin("Game", &enabled, settingsFlags))
	{
		if (!gameCamera && !GetGameCameras().empty())
		{
			gameCamera = GetGameCameras().front()->GetComponent<Camera>();
			if (renderTarget) Renderer::GetRenderTarget(renderTarget)->SetCamera(gameCamera);				
		}

		isHovered = ImGui::IsWindowHovered();
		isFocused = ImGui::IsWindowFocused();

		ImVec2 availWindowSize = ImGui::GetContentRegionAvail();

		// Top Bar -------------------------------------------------------------------------
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 10, 10 });
		if (ImGui::BeginMenuBar())
		{
			// Select Camera
			std::string camName = gameCamera ? gameCamera->GetContainerGO().get()->GetName() : "None";
			ImGui::SetNextItemWidth(105);
			if (ImGui::BeginCombo("##Camera", camName.c_str()))
			{
				std::vector<GameObject*> gameCameras = GetGameCameras();

				for (auto cameraGO : gameCameras)
				{
					bool selected = false;
					if (ImGui::Selectable(cameraGO->GetName().c_str(), &selected))
						gameCamera = cameraGO->GetComponent<Camera>();			
				}
				ImGui::EndCombo();
			}

			// Select Aspect Ratio
			int aspect = (int)this->aspect;
			ImGui::SetNextItemWidth(105);
			if (ImGui::Combo("##Aspect", &aspect, aspects, 3))
			{
				this->aspect = (Aspect)aspect;
			}

			ImGui::Dummy(ImVec2(MAX(availWindowSize.x - 308.0f, 0), 0.0f));

			// Render Settings
			if (ImGui::BeginMenu("Render"))
			{

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		ImGui::PopStyleVar();


		// Render --------------------------------------------------------------------------
		// Viewport resize check
		ImVec2 size;		
		app->gui->ApplyAspectRatio(availWindowSize.x, availWindowSize.y, &size.x, &size.y, aspect);
		ImVec2 offset = { (availWindowSize.x - size.x) * 0.5f, (availWindowSize.y - size.y) * 0.5f };
		viewportSize = { size.x, size.y };

		if (viewportSize.x > 0.0f && viewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(Renderer::GetFrameBuffer(renderTarget, "gBuffer")->GetWidth() != viewportSize.x ||
				Renderer::GetFrameBuffer(renderTarget, "gBuffer")->GetHeight() != viewportSize.y))
		{
			Renderer::GetFrameBuffer(renderTarget, "gBuffer")->Resize(viewportSize.x, viewportSize.y);
			Renderer::GetFrameBuffer(renderTarget, "postBuffer")->Resize(viewportSize.x, viewportSize.y);

			if (gameCamera)
			{
				gameCamera->aspect = viewportSize.x / viewportSize.y;
				gameCamera->UpdateCamera();
			}
		}

		// Draw FrameBuffer Texture
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
		ImGui::Dummy(offset);
		if (offset.x) ImGui::SameLine();

		ImGui::Image(
			(ImTextureID)Renderer::GetFrameBuffer(renderTarget, "postBuffer")->GetAttachmentTexture("color"),
			ImVec2{ viewportSize.x, viewportSize.y },
			ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		ImGui::PopStyleVar();
    }
	ImGui::End();

	ImGui::PopStyleVar();

	return true;
}

void PanelGame::OnSceneChange()
{
	gameCamera = nullptr;
}

std::vector<GameObject*> PanelGame::GetGameCameras()
{
	std::vector<GameObject*> gameCameras;
	for (const auto GO : engine->N_sceneManager->GetGameObjects())
	{
		if (GO->HasCameraComponent()) { gameCameras.push_back(GO.get()); }
	}

	return gameCameras;
}