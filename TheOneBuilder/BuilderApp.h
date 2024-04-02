#ifndef __BUILDER_APP_H__
#define __BUILDER_APP_H__
#pragma once

#include "../TheOneEngine/EngineCore.h"
#include "../TheOneEngine/Log.h"

#include "Timer.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>
#include <string>
#include <list>

// Forward declaration
class BuilderModule;
class BuilderWindow;
class BuilderRenderer3D;
class BuilderSceneManager;

enum class GameState {
	NONE,
	PLAY,
	PAUSE,
	PLAY_ONCE
};

class BuilderApp
{
public:

	BuilderApp(int argc, char* args[]);
	virtual ~BuilderApp();

	bool Awake();	// Called before render is available
	bool Start();	// Called before the first frame
	bool Update();	// Called each loop iteration
	bool CleanUp();	// Called before quitting

	// Add a new module to handle
	void AddModule(BuilderModule* module, bool activate);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;

	int GetFrameRate() const;
	void SetFrameRate(int refreshRate);

	double GetDT() const;
	void SetDT(double dt);

	GameState state;
	void Play();
	void Pause();
	void PlayOnce();
	bool IsPlaying();
	bool IsInGameState();
	void Stop();

private:

	// Call order for each loop iteration
	void PrepareUpdate();
	bool PreUpdate();
	bool DoUpdate();
	bool PostUpdate();
	void FinishUpdate();

	Timer* game_timer;
	Timer* start_timer;

	float time_since_start;
	float game_time;
	float scale_time;
public:

	// Modules
	BuilderWindow* window = nullptr;
	BuilderRenderer3D* renderer3D = nullptr;
	BuilderSceneManager* sceneManager = nullptr;

private:

	int argc;
	char** args;
	std::string title;
	std::string organization;

	std::list<BuilderModule*> modules;

	// Fps control
	std::chrono::duration<double> targetFrameDuration;
	std::chrono::steady_clock::time_point frameStart, frameEnd;

	int frameRate = 240;
	double dt = 0;
	double dtCount = 0;
	int frameCount = 0;
	int fps = 0;
};

extern BuilderApp* bApp;

#endif	// !__BUILDER_APP_H__