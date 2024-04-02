#include "BuilderApp.h"

#include "BuilderWindow.h"
#include "BuilderRenderer3D.h"
#include "BuilderSceneManager.h"
#include "Timer.h"
#include "../TheOneEngine/Log.h"

EngineCore* engine = NULL;

BuilderApp::BuilderApp(int argc, char* args[]) : argc(argc), args(args)
{
	engine = new EngineCore();

	window = new BuilderWindow(this);
	renderer3D = new BuilderRenderer3D(this);
	sceneManager = new BuilderSceneManager(this);
	
	// Ordered for awake / Start / Update
	// Reverse order for CleanUp
	AddModule(window, true);
	AddModule(sceneManager, true);
	
	// Render last to swap buffer
	AddModule(renderer3D, true);
	
	state = GameState::NONE;
	time_since_start = 0.0F;
	game_time = 0.0F;
	scale_time = 1.0F;
	start_timer = new Timer();
	game_timer = new Timer();
}

BuilderApp::~BuilderApp()
{
	// Release modules
	modules.clear();
}

void BuilderApp::AddModule(BuilderModule* module, bool activate)
{
	if (activate == true)
		module->Init();

	modules.push_back(module);
}

// Called before render is available
bool BuilderApp::Awake()
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
bool BuilderApp::Start()
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

	state = GameState::PLAY;

	return true;
}

// Called each loop iteration
bool BuilderApp::Update()
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
void BuilderApp::PrepareUpdate()
{
	frameStart = std::chrono::steady_clock::now();
}

bool BuilderApp::PreUpdate()
{
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

bool BuilderApp::DoUpdate()
{
	for (const auto& module : modules)
	{
		if (module->active == false)
			continue;

		if (module->Update(dt) == false)
			return false;
	}

	return true;
}

bool BuilderApp::PostUpdate()
{
	for (const auto& module : modules)
	{
		if (module->active == false)
			continue;

		if (module->PostUpdate() == false)
			return false;
	}

	return true;
}

void BuilderApp::FinishUpdate()
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

}

// -------------------- QUIT --------------------
bool BuilderApp::CleanUp()
{
	if (start_timer != nullptr)
		delete start_timer;
	if (game_timer != nullptr)
		delete game_timer;

	bool ret = true;

	for (auto item = modules.rbegin(); item != modules.rend(); ++item)
	{
		BuilderModule* module = *item;
		module->CleanUp();
	}

	return ret;
}

// -------------------- Get / Set / Funtionalities --------------------
int BuilderApp::GetArgc() const
{
	return argc;
}

const char* BuilderApp::GetArgv(int index) const
{
	if (index < argc)
		return args[index];
	else
		return NULL;
}

// Fps control
int BuilderApp::GetFrameRate() const
{
	return frameRate;
}

void BuilderApp::SetFrameRate(int frameRate)
{
	this->frameRate = frameRate == 0 ? bApp->window->GetDisplayRefreshRate() : frameRate;
	targetFrameDuration = (std::chrono::duration<double>)1 / this->frameRate;
}

double BuilderApp::GetDT() const
{
	return dt;
}

void BuilderApp::SetDT(double dt)
{
	this->dt = dt;
}

// -------------------- GAME --------------------
void BuilderApp::Play()
{
	static std::string actual_scene_name;
	if (state == GameState::NONE) {

		state = GameState::PLAY;

		game_time = 0.0F;
		game_timer->Start();

		LOG(LogType::LOG_INFO, "GameState changed to PLAY");
		//engine->audio->PlayEngine();
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
		//engine->audio->PauseEngine();

	}
}

void BuilderApp::Pause()
{
	if (state == GameState::PAUSE) {
		Play();
	}
	else if (state == GameState::PLAY || state == GameState::PLAY_ONCE) {
		state = GameState::PAUSE;
		game_timer->Pause();

		LOG(LogType::LOG_INFO, "GameState changed to PAUSE");
		//engine->audio->PauseEngine();

	}
}

void BuilderApp::PlayOnce()
{
	if (state == GameState::PAUSE) {
		game_timer->Resume();
		state = GameState::PLAY_ONCE;

		LOG(LogType::LOG_INFO, "GameState changed to PLAY_ONCE");
		//engine->audio->PlayEngine();

	}
	else if (state == GameState::PLAY) {
		state = GameState::PLAY_ONCE;
		LOG(LogType::LOG_INFO, "GameState changed to PLAY_ONCE");

	}
}

bool BuilderApp::IsPlaying()
{
	if (state == GameState::PLAY || state == GameState::PLAY_ONCE)
		return true;
	else
		return false;
}

bool BuilderApp::IsInGameState()
{
	return state != GameState::NONE;
	LOG(LogType::LOG_INFO, "GameState changed to NONE");
}

void BuilderApp::Stop()
{
	game_time = 0.0F;
	state = GameState::NONE;
	LOG(LogType::LOG_INFO, "GameState changed to NONE");
	//engine->audio->PauseEngine();
}