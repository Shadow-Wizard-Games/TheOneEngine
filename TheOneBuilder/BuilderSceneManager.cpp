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
	engine->N_sceneManager->LoadScene("Level1");
	engine->N_sceneManager->Start();

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
	engine->Render(engine->N_sceneManager->currentScene->currentCamera);
	engine->N_sceneManager->currentScene->Draw();
	if (engine->N_sceneManager->GetSceneIsChanging())
		engine->N_sceneManager->loadingScreen->DrawUI(engine->N_sceneManager->currentScene->currentCamera, DrawMode::GAME);

	return true;
}

bool BuilderSceneManager::CleanUp()
{
	engine->N_sceneManager->CleanUp();

	return true;
}
