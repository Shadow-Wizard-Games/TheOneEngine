#include "BuilderRenderer3D.h"
#include "BuilderApp.h"

#include "BuilderRenderer3D.h"
#include "BuilderWindow.h"

#include "../TheOneEngine/GameObject.h"
#include "../TheOneEngine/Component.h"
#include "../TheOneEngine/Transform.h"
#include "../TheOneEngine/Mesh.h"
#include "../TheOneEngine/Camera.h"
#include "../TheOneEngine/EngineCore.h"

BuilderRenderer3D::BuilderRenderer3D(BuilderApp* app) : BuilderModule(app) {}

BuilderRenderer3D::~BuilderRenderer3D() {}

bool BuilderRenderer3D::Awake()
{
    engine->Awake();

    return true;
}

bool BuilderRenderer3D::Start()
{
    engine->Start();

    sceneCamera = std::make_shared<GameObject>("EDITOR CAMERA");
    sceneCamera.get()->AddComponent<Transform>();
    sceneCamera.get()->GetComponent<Transform>()->SetPosition(vec3f(0, 3, -10));
    sceneCamera.get()->AddComponent<Camera>();
    sceneCamera.get()->GetComponent<Camera>()->UpdateCamera();

    cameraParent = std::make_shared<GameObject>("CameraParent");
    cameraParent.get()->AddComponent<Transform>();
    cameraParent.get()->children.push_back(sceneCamera);
    sceneCamera.get()->parent = cameraParent;

    return true;
}

bool BuilderRenderer3D::PreUpdate()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    return engine->PreUpdate();
}

bool BuilderRenderer3D::Update(double dt)
{
    engine->Update(dt);
    
    return true;
}

bool BuilderRenderer3D::PostUpdate()
{
    SDL_GL_SwapWindow(app->window->window);
    
    return true;
}

bool BuilderRenderer3D::CleanUp()
{
    engine->CleanUp();

    return true;
}
