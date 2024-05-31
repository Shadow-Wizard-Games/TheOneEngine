#include "App.h"
#include "SceneManager.h"
#include "Gui.h"
#include "PanelInspector.h"

#include "TheOneEngine/InputManager.h"
#include "TheOneEngine/Log.h"

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
	//engine->N_sceneManager->LoadScene("AA_RenderTest");

	engine->N_sceneManager->Start();

	return true;
}

bool SceneManager::PreUpdate()
{
	if (engine->N_sceneManager->GetSceneIsChanging())
	{
		for (auto panel : app->gui->GetPanels())
			panel->OnSceneChange();
	}
	
	engine->N_sceneManager->PreUpdate();

	return true;
}

bool SceneManager::Update(double dt)
{
	engine->N_sceneManager->Update(dt, app->IsPlaying());

	if ((engine->inputManager->GetKey(SDL_SCANCODE_S) == KEY_DOWN &&
		engine->inputManager->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT) &&
		engine->N_sceneManager->currentScene->IsDirty())
	{
		engine->N_sceneManager->SaveScene("Assets\\Scenes", engine->N_sceneManager->currentScene->GetSceneName());
	}

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