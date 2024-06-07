#include "BuilderApp.h"
#include "BuilderSceneManager.h"

#include "TheOneEngine/GameObject.h"
#include "TheOneEngine/Renderer.h"
#include "TheOneEngine/RenderTarget.h"
#include "TheOneEngine/Framebuffer.h"

#include <vector>

BuilderSceneManager::BuilderSceneManager(BuilderApp* app)
	: BuilderModule(app),
	renderTarget(0),
	viewportSize(640, 360),
	changeRenderTargetCamera(true)
{}

BuilderSceneManager::~BuilderSceneManager() {}

bool BuilderSceneManager::Awake()
{
	bool ret = true;
	ret = engine->N_sceneManager->Awake();

	return ret;
}

bool BuilderSceneManager::Start()
{
	// Scene
	engine->N_sceneManager->currentScene = new Scene(0, "NewUntitledScene");
	//cameraGO = engine->N_sceneManager->CreateCameraGO("mainCamera");
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
		{ Attachment::Type::RGBA8, "color_ui", "uiBuffer", 0 },
		{ Attachment::Type::RGBA8, "color_crt", "uiBuffer", 0 }
	};

	std::vector<std::vector<Attachment>> buildBuffers{ gBuffAttachments, postBuffAttachments, uiBuffAttachments };

	viewportSize = { 640, 360 };
	renderTarget = Renderer::AddRenderTarget("Build", DrawMode::BUILD, engine->N_sceneManager->currentScene->currentCamera, viewportSize, buildBuffers, true);

	return true;
}

// Scene is changed here!!!
bool BuilderSceneManager::PreUpdate()
{
	engine->N_sceneManager->PreUpdate();

	if (changeRenderTargetCamera)
	{
		Renderer::GetRenderTarget(renderTarget)->SetCamera(engine->N_sceneManager->currentScene->currentCamera);
		changeRenderTargetCamera = false;
	}

	return true;
}

// Scene change is SET here!!!
bool BuilderSceneManager::Update(double dt)
{
	engine->N_sceneManager->Update(dt, app->IsPlaying());

	if (engine->N_sceneManager->GetSceneIsChanging())
		changeRenderTargetCamera = true;

	return true;
}

bool BuilderSceneManager::PostUpdate()
{

	return true;
}

bool BuilderSceneManager::CleanUp()
{
	engine->N_sceneManager->CleanUp();

	return true;
}
