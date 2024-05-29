#include "BuilderApp.h"
#include "BuilderSceneManager.h"

#include "TheOneEngine/GameObject.h"
#include "TheOneEngine/Renderer.h"
#include "TheOneEngine/Framebuffer.h"

#include <vector>

BuilderSceneManager::BuilderSceneManager(BuilderApp* app) : BuilderModule(app) {}

BuilderSceneManager::~BuilderSceneManager() {}

bool BuilderSceneManager::Awake()
{
	engine->N_sceneManager->Awake();

	return true;
}

bool BuilderSceneManager::Start()
{
	engine->N_sceneManager->currentScene = new Scene(0, "NewUntitledScene");
	cameraGO = engine->N_sceneManager->CreateCameraGO("mainCamera");
	engine->N_sceneManager->LoadScene("MainMenu");
	engine->N_sceneManager->Start();

	// pew pew pew :3
	Resources::LoadFromLibrary<Model>("Library/Meshes/SM_Cube/pCube1.mesh");

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

	std::vector<std::vector<Attachment>> buildBuffers{ gBuffAttachments, postBuffAttachments, uiBuffAttachments };

	viewportSize = { 680, 360 };
	renderTarget = Renderer::AddRenderTarget("Build", DrawMode::EDITOR, cameraGO->GetComponent<Camera>(), viewportSize, buildBuffers);

	return true;
}

bool BuilderSceneManager::PreUpdate()
{
	engine->N_sceneManager->PreUpdate();

	return true;
}

bool BuilderSceneManager::Update(double dt)
{
	engine->N_sceneManager->Update(dt, app->IsPlaying());

	return true;
}

bool BuilderSceneManager::PostUpdate()
{
	//engine->SetRenderEnvironment(engine->N_sceneManager->currentScene->currentCamera);
	
	//hekbas: add here engine->DebugDraw

	//engine->Render(engine->N_sceneManager->currentScene->currentCamera);
	//engine->N_sceneManager->currentScene->Draw(DrawMode::GAME, engine->N_sceneManager->currentScene->currentCamera);

	return true;
}

bool BuilderSceneManager::CleanUp()
{
	engine->N_sceneManager->CleanUp();

	return true;
}
