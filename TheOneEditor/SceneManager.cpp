#include "App.h"
#include "SceneManager.h"
#include "Gui.h"
#include "PanelInspector.h"
#include "../TheOneEngine/Log.h"

#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

SceneManager::SceneManager(App* app) : Module(app) {}

SceneManager::~SceneManager() {}

bool SceneManager::Awake()
{
	engine->N_sceneManager->Awake();
	return true;
}

bool SceneManager::Start()
{
	engine->N_sceneManager->currentScene = new Scene(0, "NewUntitledScene");
	engine->N_sceneManager->CreateCameraGO("mainCamera");
	engine->N_sceneManager->LoadScene("NewUntitledScene");
	engine->N_sceneManager->Start();

	engine->N_sceneManager->CreateTeapot();
	engine->monoManager->bulletGO = engine->N_sceneManager->currentScene->GetRootSceneGO()->children.back().get();
	engine->monoManager->bulletGO->AddScript("Bullet");
	engine->monoManager->bulletGO->Disable();

	N_sceneManager->CreateEmptyGO();
	N_sceneManager->GetGameObjects().front().get()->AddComponent<Camera>();
	N_sceneManager->GetGameObjects().front().get()->GetComponent<Camera>()->UpdateCamera();

	return true;
}

bool SceneManager::PreUpdate()
{
	engine->N_sceneManager->PreUpdate();

	return true;
}

bool SceneManager::Update(double dt)
{
	engine->N_sceneManager->Update(dt, app->IsPlaying());

	return true;
}

bool SceneManager::PostUpdate()
{
	engine->N_sceneManager->PostUpdate();

	return true;
}

bool SceneManager::CleanUp()
{
	engine->N_sceneManager->CleanUp();

	return true;
}