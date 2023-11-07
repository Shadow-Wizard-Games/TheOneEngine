#include "App.h"

#include "Window.h"
#include "Gui.h"
#include "Renderer3D.h"


Renderer3D::Renderer3D(App* app) : Module(app) {}

Renderer3D::~Renderer3D() {}

bool Renderer3D::Awake()
{
    app->engine->Awake();

    return true;
}

bool Renderer3D::Start()
{
    app->engine->Start();
    return true;
}

bool Renderer3D::PreUpdate()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    return true;
}

bool Renderer3D::Update(double dt)
{
    app->engine->Update(dt);

    return true;
}

bool Renderer3D::PostUpdate()
{ 
    app->engine->Render(EngineCore::RenderModes::DEBUG);

    // hekbas testing Mesh load/draw
    static auto mesh_ptrs = AssetMesh::loadFromFile("Assets/mf.fbx");
    for (auto& mesh_ptr : mesh_ptrs) mesh_ptr->draw();

    app->gui->Draw();

    SDL_GL_SwapWindow(app->window->window);

    return true;
}

bool Renderer3D::CleanUp()
{

    return true;
}