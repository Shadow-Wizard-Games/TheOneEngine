#include "App.h"

#include "Renderer3D.h"
#include "SceneManager.h"
#include "Window.h"
#include "Gui.h"
#include "PanelScene.h"

#include "TheOneEngine\GameObject.h"
#include "TheOneEngine\Component.h"
#include "TheOneEngine\Transform.h"
#include "TheOneEngine\Mesh.h"
#include "TheOneEngine\Camera.h"
#include "TheOneEngine\EngineCore.h"

Renderer3D::Renderer3D(App* app) : Module(app) {}

Renderer3D::~Renderer3D() {}

bool Renderer3D::Awake()
{
    engine->Awake();

	return true;
}

bool Renderer3D::Start()
{
    engine->Start();

	return true;
}

bool Renderer3D::PreUpdate()
{
    return engine->PreUpdate();
}

bool Renderer3D::Update(double dt)
{
	app->gui->panelScene->isHovered = false;

    engine->Update(dt);

	return true;
}

bool Renderer3D::PostUpdate()
{
	return true;
}

bool Renderer3D::CleanUp()
{
    engine->CleanUp();
    return true;
}