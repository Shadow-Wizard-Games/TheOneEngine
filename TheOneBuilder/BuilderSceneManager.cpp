#include "BuilderApp.h"
#include "BuilderSceneManager.h"
#include "../TheOneEngine/Camera.h"

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
	engine->N_sceneManager->CreateCameraGO("mainCamera");
	engine->N_sceneManager->LoadScene("MainMenu");
	engine->N_sceneManager->Start();

	// pew pew pew :3
	Resources::LoadFromLibrary<Model>("Library/Meshes/SM_Cube/pCube1.mesh");

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
	engine->SetRenderEnvironment(engine->N_sceneManager->currentScene->currentCamera);
	
	//hekbas: add here engine->DebugDraw

	//engine->Render(engine->N_sceneManager->currentScene->currentCamera);
	engine->N_sceneManager->currentScene->Draw(DrawMode::GAME, engine->N_sceneManager->currentScene->currentCamera);

	return true;
}

bool BuilderSceneManager::CleanUp()
{
	engine->N_sceneManager->CleanUp();

	return true;
}
