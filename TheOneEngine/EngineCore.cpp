#include "EngineCore.h"
#include "Log.h"
#include "Defs.h"
#include "N_SceneManager.h"
#include "Collider2D.h"
#include "Renderer.h"
#include "Renderer2D.h"
#include "Renderer3D.h"
#include "Window.h"

#include <GL\glew.h>
#include <glm\ext\matrix_transform.hpp>
#include <IL\il.h>
#include <memory>

AudioManager* audioManager = NULL;

EngineCore::EngineCore()
{
    window = new Window();
    inputManager = new InputManager();
    audioManager = new AudioManager();
    monoManager = new MonoManager();
    collisionSolver = new CollisionSolver();
    N_sceneManager = new N_SceneManager();
    easingManager = new EasingManager();
}

EngineCore::~EngineCore() {}

void EngineCore::Awake()
{
    LOG(LogType::LOG_OK, "Initializing DevIL");
    ilInit();
    window->Awake();
    inputManager->Init();
    audioManager->Awake();
    monoManager->InitMono();
    Renderer::Init();
}

void EngineCore::Start()
{
    window->Start();
}

bool EngineCore::PreUpdate()
{
    if (!inputManager->PreUpdate()) { return false; }
    if (!collisionSolver->PreUpdate()) { return false; }
    return true;
}

void EngineCore::Update(double dt)
{
    audioManager->Update(dt);
    if(N_sceneManager->GetSceneIsPlaying())
        collisionSolver->Update(dt);
    //easingManager->Update(dt);
    Renderer::Update();

    this->dt = dt;
    upTime += dt;
}

void EngineCore::CleanUp()
{
    Renderer::Shutdown();

    audioManager->CleanUp();
    delete audioManager;
    audioManager = nullptr;
    
    monoManager->ShutDownMono();
    delete monoManager;
    monoManager = nullptr;

    inputManager->CleanUp();
    delete inputManager;
    inputManager = nullptr;

    collisionSolver->CleanUp();
    delete collisionSolver;
    collisionSolver = nullptr;
}

bool EngineCore::GetVSync()
{
    return vsync;
}

// hekbas - parameter not used?
bool EngineCore::SetVSync(bool vsync)
{
    if (this->vsync)
    {
        if (SDL_GL_SetSwapInterval(1) == -1)
        {
            LOG(LogType::LOG_ERROR, "Unable to turn on V-Sync: %s", SDL_GetError());
            return false;
        }
    }
    else
    {
        if (SDL_GL_SetSwapInterval(0) == -1)
        {
            LOG(LogType::LOG_ERROR, "Unable to turn off V-Sync: %s", SDL_GetError());
            return false;
        }
    }

    return true;
}

std::vector<LogInfo> EngineCore::GetLogs()
{
    return logs;
}

void EngineCore::AddLog(LogType type, const char* entry)
{
    if (logs.size() > MAX_LOGS_CONSOLE)
        logs.erase(logs.begin());

    std::string toAdd = entry;
    LogInfo info = { type, toAdd };

    logs.push_back(info);
}

void EngineCore::CleanLogs()
{
    logs.clear();
}

void EngineCore::SetEditorCamera(Camera* cam)
{
    if(cam) editorCamReference = cam;
}