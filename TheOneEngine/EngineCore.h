#ifndef __ENGINE_CORE_H__
#define __ENGINE_CORE_H__
#pragma once


// hekbas: Include here all headers needed in Editor
// Include in Editor when needed: #include "../TheOneEngine/EngineCore.h"
#include "Defs.h"
#include "Camera.h"
#include "Transform.h"
#include "Mesh.h"
#include "Texture.h"
#include "Ray.h"
#include "Log.h"
#include "CollisionSolver.h"

#include "MonoManager.h"
#include "InputManager.h"
#include "AudioManager.h"

#include <chrono>
#include <memory>
#include <string>

class N_SceneManager;

class EngineCore
{
public:

	EngineCore();

	void Awake();
	void Start();

	bool PreUpdate();
	void Update(double dt);

	void Render(Camera* camera);

	void LogGL(string id);

	void CleanUp();

	void DrawAxis();
	void DrawGrid(int grid_size, int grid_step);
	void DrawFrustum(const Frustum& frustum);
	void DrawRay(const Ray& ray);

	bool GetVSync();
	bool SetVSync(bool vsync);

	std::vector<LogInfo> GetLogs();
	void AddLog(LogType type, const char* entry);
	void CleanLogs();

	void SetEditorCamera(Camera* cam);

public:
	
	double dt = 0;

	bool vsync = false;

	CollisionSolver* collisionSolver = nullptr;

	MonoManager* monoManager = nullptr;
	InputManager* inputManager = nullptr;
	N_SceneManager* N_sceneManager = nullptr;

private:

	// Logs
	LogInfo logInfo;
	std::vector<LogInfo> logs;
	Camera* editorCamReference;

};

extern EngineCore* engine;
extern AudioManager* audioManager;
#endif // !__ENGINE_CORE_H__