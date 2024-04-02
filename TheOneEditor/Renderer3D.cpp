#include "App.h"

#include "Renderer3D.h"
#include "SceneManager.h"
#include "Window.h"
#include "Gui.h"
#include "PanelScene.h"

#include "..\TheOneEngine\GameObject.h"
#include "..\TheOneEngine\Component.h"
#include "..\TheOneEngine\Transform.h"
#include "..\TheOneEngine\Mesh.h"
#include "..\TheOneEngine\Camera.h"
#include "..\TheOneEngine\EngineCore.h"

Renderer3D::Renderer3D(App* app) : Module(app)
{
}

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
    dtTemp = dt;
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

void Renderer3D::CameraInput(GameObject* cam)
{

	Camera* camera = cam->GetComponent<Camera>();
	Transform* transform = cam->GetComponent<Transform>();

	double speed = 60 * dtTemp;
	if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 90 * dtTemp;

    double mouseSensitivity = 36.0 * dtTemp;

    if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
    {
        // Yaw and Pitch
        camera->yaw = app->input->GetMouseXMotion() * mouseSensitivity;
        camera->pitch = -app->input->GetMouseYMotion() * mouseSensitivity;

        transform->Rotate(vec3f(0.0f, camera->yaw, 0.0f), HandleSpace::GLOBAL);
        transform->Rotate(vec3f(camera->pitch, 0.0f, 0.0f), HandleSpace::LOCAL);

        // WASD
        if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			transform->Translate(transform->GetForward() * speed, HandleSpace::LOCAL);
        
        if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			transform->Translate(-transform->GetForward() * speed, HandleSpace::LOCAL);
        
        if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			transform->Translate(transform->GetRight() * speed, HandleSpace::LOCAL);
        
        if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			transform->Translate(-transform->GetRight() * speed, HandleSpace::LOCAL);
        
    }
    else if (app->input->GetMouseZ() != 0) 
    {
        // Zoom
		transform->Translate(transform->GetForward() * (double)app->input->GetMouseZ());
    }

    // (MMB) Panning
    if (app->input->GetMouseButton(SDL_BUTTON_MIDDLE))
    {
        double deltaX = app->input->GetMouseXMotion();
        double deltaY = app->input->GetMouseYMotion();

        float panSpeed = 10 * dtTemp;

        transform->Translate(vec3(deltaX * panSpeed, 0, 0), HandleSpace::GLOBAL);
        transform->Translate(vec3(0, deltaY * panSpeed, 0), HandleSpace::GLOBAL);
    }

    // (F) Focus Selection
    if (app->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN && engine->N_sceneManager->GetSelectedGO() != nullptr)
    {
        transform->SetPosition(camera->lookAt);
        vec3f finalPos;
        finalPos = transform->GetPosition() - transform->GetForward();
        finalPos = engine->N_sceneManager->GetSelectedGO().get()->GetComponent<Transform>()->GetPosition() - (transform->GetForward() * 10.0);

        transform->SetPosition(finalPos);
    }

    // ALT
    // (Alt + LMB) Orbit Selection
    if (app->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
    {
        camera->yaw = app->input->GetMouseXMotion() * mouseSensitivity;
        camera->pitch = -app->input->GetMouseYMotion() * mouseSensitivity;

		transform->SetPosition(camera->lookAt);
       
        transform->Rotate(vec3f(0.0f, camera->yaw, 0.0f), HandleSpace::GLOBAL);
        transform->Rotate(vec3f(camera->pitch, 0.0f, 0.0f), HandleSpace::LOCAL);

        vec3f finalPos;
        
        if (engine->N_sceneManager->GetSelectedGO() != nullptr)
        {
            finalPos = engine->N_sceneManager->GetSelectedGO().get()->GetComponent<Transform>()->GetPosition() - (transform->GetForward() * 40.0);
        }
        else
        {
            finalPos = transform->GetPosition() - transform->GetForward();
        }

		transform->SetPosition(finalPos);
    }
}