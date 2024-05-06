#include "PanelGame.h"
#include "App.h"
#include "Gui.h"
#include "Renderer3D.h"
#include "SceneManager.h"
#include "Window.h"
#include "imgui.h"

#include "../TheOneEngine/EngineCore.h"

PanelGame::PanelGame(PanelType type, std::string name) : Panel(type, name)
{
	currentScene = nullptr;
	gameCamera = nullptr;
	frameBuffer = std::make_shared<FrameBuffer>(1280, 720, true);
	viewportSize = { 0.0f, 0.0f };
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
	if (gameCameras.front()) gameCamera = gameCameras.front()->GetComponent<Camera>();
	
	// Find primary camera
	/*for (const auto& cam : gameCameras)
	{
		Camera* gameCam = cam->GetComponent<Camera>();

		if (gameCam && !gameCam->primaryCam) continue;
		gameCamera = gameCam;
	}*/
}

bool PanelGame::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
	ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Once);

	if (ImGui::Begin("Game", &enabled, settingsFlags))
	{
		if (!gameCamera)
		{
			std::vector<GameObject*> gameCameras = GetGameCameras();
			if (gameCameras.front()) gameCamera = gameCameras.front()->GetComponent<Camera>();
		}

		isHovered = ImGui::IsWindowHovered();
		isFocused = ImGui::IsWindowFocused();

		ImVec2 availWindowSize = ImGui::GetContentRegionAvail();

		// Top Bar -----------------------------------------------------------------
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


		// Viewport resize check
		ImVec2 size;		
		app->gui->ApplyAspectRatio(availWindowSize.x, availWindowSize.y, &size.x, &size.y, aspect);
		ImVec2 offset = { (availWindowSize.x - size.x) * 0.5f, (availWindowSize.y - size.y) * 0.5f };
		viewportSize = { size.x, size.y };

		if (viewportSize.x > 0.0f && viewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(frameBuffer->getWidth() != viewportSize.x || frameBuffer->getHeight() != viewportSize.y))
		{
			frameBuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);

			if (gameCamera)
			{
				gameCamera->aspect = viewportSize.x / viewportSize.y;
				gameCamera->UpdateCamera();
			}
		}

		// ALL DRAWING MUST HAPPEN BETWEEN FB BIND/UNBIND -------------------------------------------------
		{
			frameBuffer->Bind();
			frameBuffer->Clear();
			frameBuffer->ClearBuffer(-1);

			// Set Render Environment
			engine->SetRenderEnvironment(gameCamera);
			engine->SetUniformBufferCamera(gameCamera);

			currentScene->Draw(DrawMode::GAME);

			N_SceneManager* engineSM = engine->N_sceneManager;
			if (engineSM->GetSceneIsChanging())
				engineSM->loadingScreen->DrawUI(engineSM->currentScene->currentCamera, DrawMode::GAME);

			frameBuffer->Unbind();
		}

		// Draw FrameBuffer Texture
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
		ImGui::Dummy(offset);
		if (offset.x) ImGui::SameLine();

		ImGui::Image(
			(ImTextureID)frameBuffer->getColorBufferTexture(),
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