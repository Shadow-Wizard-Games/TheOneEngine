#include "App.h"

#include "Renderer3D.h"
#include "Window.h"
#include "Gui.h"
#include "PanelScene.h"
#include "SceneManager.h"

#include "..\TheOneEngine\GameObject.h"
#include "..\TheOneEngine\Component.h"
#include "..\TheOneEngine\Transform.h"
#include "..\TheOneEngine\Mesh.h"
#include "..\TheOneEngine\Camera.h"


Renderer3D::Renderer3D(App* app) : Module(app)
{
}

Renderer3D::~Renderer3D() {}

bool Renderer3D::Awake()
{
    app->engine->Awake();

    return true;
}

bool Renderer3D::Start()
{
    app->engine->Start();

    // Creating Editor Camera GO (Outside hierarchy)
    sceneCamera = std::make_shared<GameObject>("EDITOR CAMERA");
    sceneCamera.get()->AddComponent<Transform>();
    sceneCamera.get()->AddComponent<Camera>();
    sceneCamera.get()->GetComponent<Transform>()->SetPosition(vec3f(0, 3, -10));
    sceneCamera.get()->GetComponent<Camera>()->lookAt = {0, 0, 0};
    sceneCamera.get()->GetComponent<Camera>()->UpdateCamera();

	cameraParent = std::make_shared<GameObject>("CameraParent");
	cameraParent.get()->AddComponent<Transform>();
	cameraParent.get()->children.push_back(sceneCamera);
	sceneCamera.get()->parent = cameraParent;
    

    /*app->engine->audio->SetListenerTransform(
        sceneCamera.get()->GetComponent<Transform>()->getPosition().x, 
        sceneCamera.get()->GetComponent<Transform>()->getPosition().y,
        sceneCamera.get()->GetComponent<Transform>()->getPosition().z, 
        sceneCamera.get()->GetComponent<Transform>()->getForward().x, 
        sceneCamera.get()->GetComponent<Transform>()->getForward().y,
        sceneCamera.get()->GetComponent<Transform>()->getForward().z, 
        sceneCamera.get()->GetComponent<Transform>()->getUp().x,
        sceneCamera.get()->GetComponent<Transform>()->getUp().y,
        sceneCamera.get()->GetComponent<Transform>()->getUp().z);*/


    // hekbas test adding same component
    LOG(LogType::LOG_INFO, "# Testing Component Duplication");
    sceneCamera.get()->AddComponent<Camera>();


    return true;
}

bool Renderer3D::PreUpdate()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    return true;
}

bool Renderer3D::Update(double dt)
{
    CameraInput(dt);
    app->gui->panelScene->isHovered = false;

    app->engine->Update(dt);

    /*app->engine->audio->SetListenerTransform(
        sceneCamera.get()->GetComponent<Transform>()->getPosition().x,
        sceneCamera.get()->GetComponent<Transform>()->getPosition().y,
        sceneCamera.get()->GetComponent<Transform>()->getPosition().z,
        sceneCamera.get()->GetComponent<Transform>()->getForward().x,
        sceneCamera.get()->GetComponent<Transform>()->getForward().y,
        sceneCamera.get()->GetComponent<Transform>()->getForward().z,
        sceneCamera.get()->GetComponent<Transform>()->getUp().x,
        sceneCamera.get()->GetComponent<Transform>()->getUp().y,
        sceneCamera.get()->GetComponent<Transform>()->getUp().z);

    app->engine->audio->SetSpatial1Transform(
        app->sceneManager->spatialObject1->GetComponent<Transform>()->getPosition().x,
        app->sceneManager->spatialObject1->GetComponent<Transform>()->getPosition().y,
        app->sceneManager->spatialObject1->GetComponent<Transform>()->getPosition().z);
    app->engine->audio->SetSpatial2Transform(
        app->sceneManager->spatialObject2->GetComponent<Transform>()->getPosition().x,
        app->sceneManager->spatialObject2->GetComponent<Transform>()->getPosition().y,
        0);*/

    return true;
}

bool Renderer3D::PostUpdate()
{
    // Scene camera
    /*Camera* sceneCam = sceneCamera.get()->GetComponent<Camera>();
    app->engine->Render(sceneCam);*/

    // hekbas testing Mesh load/draw
    /*static auto mesh_ptrs = MeshLoader::LoadMesh("Assets/mf.fbx");
    for (auto& mesh_ptr : mesh_ptrs) mesh_ptr->draw();*/

    app->gui->Draw();

    SDL_GL_SwapWindow(app->window->window);

    return true;
}

bool Renderer3D::CleanUp()
{
    app->engine->CleanUp();
    return true;
}

void Renderer3D::CameraInput(double dt)
{
    if (!app->gui->panelScene->isHovered)
        return;

    Camera* camera = sceneCamera.get()->GetComponent<Camera>();
    Transform* transform = sceneCamera.get()->GetComponent<Transform>();

    double speed = 20 * dt;
    if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
        speed = 35 * dt;

    double mouseSensitivity = 18.0 * dt;

    if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
    {
        // Mouse camera movement
        camera->yaw = -app->input->GetMouseXMotion() * mouseSensitivity;
        camera->pitch = app->input->GetMouseYMotion() * mouseSensitivity;

        transform->Rotate(vec3f(0.0f, camera->yaw, 0.0f), HandleSpace::GLOBAL);
        transform->Rotate(vec3f(camera->pitch, 0.0f, 0.0f), HandleSpace::GLOBAL);

        //transform->RotateInspector(vec3f(0.0f, camera->yaw, 0.0f));
        //transform->RotateInspector(vec3f(camera->pitch, 0.0f, 0.0f));


        // WASD
        if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			transform->Translate(transform->GetForward() * speed);
        
        if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			transform->Translate(-transform->GetForward() * speed);
        
        if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			transform->Translate(transform->GetRight() * speed);
        
        if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			transform->Translate(-transform->GetRight() * speed);
        
    }
    else
    {
        // Zooming Camera Input
		transform->Translate(transform->GetForward() * (double)app->input->GetMouseZ());
    }

    // Orbit Object with Alt + LMB
    if (app->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
    {
        camera->yaw += -app->input->GetMouseXMotion() * mouseSensitivity;
        camera->pitch += app->input->GetMouseYMotion() * mouseSensitivity;

		transform->SetPosition(camera->lookAt);
       
        //camera->Rotate(vec3f(0.0f, 1.0f, 0.0f), camera->yaw, false);
        //camera->Rotate(vec3f(1.0f, 0.0f, 0.0f), camera->pitch, true);

        vec3f finalPos;
        finalPos = transform->GetPosition() - transform->GetForward();
        if (app->sceneManager->GetSelectedGO() != nullptr)
        {
            finalPos = app->sceneManager->GetSelectedGO().get()->GetComponent<Transform>()->GetPosition() - (transform->GetForward() * 100.0);
        }

		transform->SetPosition(finalPos);
    }

    if (app->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN && app->sceneManager->GetSelectedGO() != nullptr)
    {
        vec3f targetPos = app->sceneManager->GetSelectedGO().get()->GetComponent<Transform>()->GetPosition() - transform->GetForward();

		transform->SetPosition(targetPos * 100.0f);
    }

    camera->UpdateCamera();
}