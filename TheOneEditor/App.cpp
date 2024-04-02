#include "App.h"

#include "Window.h"
#include "Input.h"
#include "Hardware.h"
#include "Gui.h"
#include "SceneManager.h"
#include "Renderer3D.h"

#include "PanelAbout.h"
#include "PanelConsole.h"
#include "PanelHierarchy.h"
#include "PanelInspector.h"
#include "PanelProject.h"
#include "PanelScene.h"
#include "PanelSettings.h"
#include "Timer.h"
#include "../TheOneEngine/Transform.h"
#include "../TheOneEngine/Log.h"

EngineCore* engine = NULL;

App::App(int argc, char* args[]) : argc(argc), args(args)
{
	engine = new EngineCore();
	
	window = new Window(this);
	input = new Input(this);
	hardware = new Hardware(this);
	gui = new Gui(this);
	scenemanager = new SceneManager(this);
	renderer3D = new Renderer3D(this);

	// Ordered for awake / Start / Update
	// Reverse order for CleanUp

	AddModule(window, true);
	AddModule(input, true);
	AddModule(hardware, true);
	AddModule(scenemanager, true);

	// Render last to swap buffer
	AddModule(renderer3D, true);
	AddModule(gui, true);

	state = GameState::NONE;
	time_since_start = 0.0F;
	game_time = 0.0F;
	scale_time = 1.0F;
	start_timer = new Timer();
	game_timer = new Timer();
}

App::~App()
{
	// Release modules
	modules.clear();
}

void App::AddModule(Module* module, bool activate)
{
	if (activate == true)
		module->Init();

	modules.push_back(module);
}

// Called before render is available
bool App::Awake()
{
	bool ret = false;

	targetFrameDuration = (std::chrono::duration<double>)1 / frameRate;

	//Load config from XML
	//ret = LoadConfig();
	ret = true;

	if (ret == true)
	{
		//title = configNode.child("app").child("title").child_value();

		//maxFrameDuration = configNode.child("app").child("frcap").attribute("value").as_int();

		for (const auto& item : modules)
		{
			if (item->active == false)
				continue;

			item->Awake();
		}
	}

	//LOG("---------------- Time Awake: %f/n", timer.ReadMSec());

	return ret;
}

// Called before the first frame
bool App::Start()
{
	dt = 0;

	start_timer->Start();

	for (const auto& module : modules)
	{
		if (module->active == false)
			continue;

		if (module->Start() == false)
			return false;
	}

	//LOG("----------------- Time Start(): %f", timer.ReadMSec());

	return true;
}

// Called each loop iteration
bool App::Update()
{
	bool ret = true;
	PrepareUpdate();

	/*if (input->GetWindowEvent(WE_QUIT) == true)
		ret = false;*/

	if (ret == true)
		ret = PreUpdate();

	if (ret == true)
		ret = DoUpdate();

	if (ret == true)
		ret = PostUpdate();

	FinishUpdate();

	time_since_start = start_timer->ReadSec();

	if (state == GameState::PLAY || state == GameState::PLAY_ONCE) {
		game_time = game_timer->ReadSec(scale_time);
	}

	return ret;
}

// -------------------- LOOP ITERATION --------------------
void App::PrepareUpdate()
{
	frameStart = std::chrono::steady_clock::now();
}

bool App::PreUpdate()
{
	//OPTICK_CATEGORY("PreUpdate", Optick::Category::GameLogic);
	bool ret = true;

	for (const auto& module : modules)
	{
		if (module->active == false)
			continue;

		if (module->PreUpdate() == false)
			return false;
	}

	return true;
}

bool App::DoUpdate()
{
	//OPTICK_CATEGORY("DoUpdate", Optick::Category::GameLogic);

	for (const auto& module : modules)
	{
		if (module->active == false)
			continue;

		if (module->Update(dt) == false)
			return false;
	}

	return true;
}

bool App::PostUpdate()
{
	//OPTICK_CATEGORY("PostUpdate", Optick::Category::GameLogic);

	for (const auto& module : modules)
	{
		if (module->active == false)
			continue;

		if (module->PostUpdate() == false)
			return false;
	}

	return true;
}

void App::FinishUpdate()
{
	// dt calculation
	frameEnd = std::chrono::steady_clock::now();
	auto frameDuration = std::chrono::duration_cast<std::chrono::duration<double>>(frameEnd - frameStart);

	dt = frameDuration.count();

	if (frameDuration < targetFrameDuration)
	{
		std::chrono::duration<double> sleepTime = targetFrameDuration - frameDuration;
		std::this_thread::sleep_for(sleepTime);

		dt = targetFrameDuration.count();
	}

	// fps calculation
	dtCount += dt;
	frameCount++;

	if (dtCount >= 1)
	{
		fps = frameCount;
		frameCount = 0;
		dtCount = 0;
	}

	app->gui->panelSettings->AddFpsValue(fps);
}

// -------------------- QUIT --------------------
bool App::CleanUp()
{
	if (start_timer != nullptr)
		delete start_timer;
	if (game_timer != nullptr)
		delete game_timer;

	bool ret = true;

	for (auto item = modules.rbegin(); item != modules.rend(); ++item)
	{
		Module* module = *item;
		module->CleanUp();
	}

	return ret;
}

// -------------------- Get / Set / Funtionalities --------------------
int App::GetArgc() const
{
	return argc;
}

const char* App::GetArgv(int index) const
{
	if (index < argc)
		return args[index];
	else
		return NULL;
}

// Fps control
int App::GetFrameRate() const
{
	return frameRate;
}

void App::SetFrameRate(int frameRate)
{
	this->frameRate = frameRate == 0 ? app->window->GetDisplayRefreshRate() : frameRate;
	targetFrameDuration = (std::chrono::duration<double>)1 / this->frameRate;
}

double App::GetDT() const
{
	return dt;
}

void App::SetDT(double dt)
{
	this->dt = dt;
}

// -------------------- GAME --------------------

void App::Play()
{
	static std::string actual_scene_name;
	if (state == GameState::NONE) {
		state = GameState::PLAY;

		game_time = 0.0F;
		game_timer->Start();

		LOG(LogType::LOG_INFO, "GameState changed to PLAY");
		//audioManager->audio->PlayEngine();
	}
	else if (state == GameState::PAUSE) {
		state = GameState::PLAY;
		game_timer->Resume();

		LOG(LogType::LOG_INFO, "GameState changed to PLAY");
	}
	else if (state == GameState::PLAY) {
		state = GameState::NONE;
		game_time = 0.0F;

		LOG(LogType::LOG_INFO, "GameState changed to NONE");
		//audioManager->audio->PauseEngine();
	}
}

void App::Pause()
{
	if (state == GameState::PAUSE) {
		Play();
	}
	else if (state == GameState::PLAY || state == GameState::PLAY_ONCE) {
		state = GameState::PAUSE;
		game_timer->Pause();

		LOG(LogType::LOG_INFO, "GameState changed to PAUSE");
		//audioManager->audio->PauseEngine();
	}
}

void App::PlayOnce()
{
	if (state == GameState::PAUSE) {
		game_timer->Resume();
		state = GameState::PLAY_ONCE;

		LOG(LogType::LOG_INFO, "GameState changed to PLAY_ONCE");
		//audioManager->audio->PlayEngine();
	}
	else if (state == GameState::PLAY) {
		state = GameState::PLAY_ONCE;
		LOG(LogType::LOG_INFO, "GameState changed to PLAY_ONCE");
	}
}

bool App::IsPlaying()
{
	if (state == GameState::PLAY || state == GameState::PLAY_ONCE)
		return true;
	else
		return false;
}

bool App::IsInGameState()
{
	return state != GameState::NONE;
	LOG(LogType::LOG_INFO, "GameState changed to NONE");
}

void App::Stop()
{
	game_time = 0.0F;
	state = GameState::NONE;
	LOG(LogType::LOG_INFO, "GameState changed to NONE");
	//audioManager->audio->PauseEngine();
}