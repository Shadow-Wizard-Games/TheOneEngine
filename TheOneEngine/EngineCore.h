#ifndef __ENGINE_CORE_H__
#define __ENGINE_CORE_H__
#pragma once

#include "Defs.h"
#include "Camera.h"
#include "Transform.h"
#include "Mesh.h"
#include "Texture.h"
#include "Ray.h"
#include "Log.h"
#include "CollisionSolver.h"
#include "UniformBuffer.h"

#include "MonoManager.h"
#include "InputManager.h"
#include "AudioManager.h"
#include "EasingManager.h"

#include "ozz/base/memory/allocator.h"

#include <chrono>
#include <memory>
#include <string>

class N_SceneManager;
class Window;


// Volatile memory buffer that can be used within function scope.
// Minimum alignment is 16 bytes.
class ScratchBuffer
{
public:
	ScratchBuffer() : buffer_(nullptr), size_(0) {}

	~ScratchBuffer() {
		ozz::memory::default_allocator()->Deallocate(buffer_);
	}

	void* Resize(size_t _size) {
		if (_size > size_) {
			size_ = _size;
			ozz::memory::default_allocator()->Deallocate(buffer_);
			buffer_ = ozz::memory::default_allocator()->Allocate(_size, 16);
		}
		return buffer_;
	}

private:
	void* buffer_;
	size_t size_;
};


class EngineCore
{
public:

	EngineCore();
	~EngineCore();

	void Awake();
	void Start();

	bool PreUpdate();
	void Update(double dt);

	void CleanUp();

	bool GetVSync();
	bool SetVSync(bool vsync);

	std::vector<LogInfo> GetLogs();
	void AddLog(LogType type, const char* entry);
	void CleanLogs();

	void SetEditorCamera(Camera* cam);

public:
	
	double dt = 0;
	bool vsync = false;

	Window* window = nullptr;
	InputManager* inputManager = nullptr;
	MonoManager* monoManager = nullptr;
	N_SceneManager* N_sceneManager = nullptr;
	CollisionSolver* collisionSolver = nullptr;
	EasingManager* easingManager = nullptr;


	ScratchBuffer scratch_buffer_;

	std::string lightingProcessPath;

private:

	// Logs
	LogInfo logInfo;
	std::vector<LogInfo> logs;

	Camera* editorCamReference;
};

extern EngineCore* engine;
extern AudioManager* audioManager;
#endif // !__ENGINE_CORE_H__