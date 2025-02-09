#include "PanelScene.h"
#include "App.h"
#include "Gui.h"
#include "PanelInspector.h"
#include "SceneManager.h"
#include "imgui.h"
#include "imGuizmo.h"

#include "TheOneEngine/EngineCore.h"
#include "TheOneEngine/Window.h"
#include "TheOneEngine/Renderer.h"
#include "TheOneEngine/Renderer3D.h"
#include "TheOneEngine/RenderTarget.h"
#include "TheOneEngine/Ray.h"
#include "TheOneEngine/Log.h"
#include "TheOneEngine/Easing.h"

#include <vector>

PanelScene::PanelScene(PanelType type, std::string name) : Panel(type, name)
{
    isHovered = false;
    isFocused = false;

    current = nullptr;

    sceneCamera = nullptr;
    cameraParent = nullptr;
    camControlMode = CamControlMode::DISABLED;

    camEaseInX = engine->easingManager->AddEasing(2);
    camEaseInY = engine->easingManager->AddEasing(2);
    camEaseOutX = engine->easingManager->AddEasing(0.2);
    camEaseOutY = engine->easingManager->AddEasing(0.2);

    renderTarget = -1;
    viewportSize = { 0.0f, 0.0f };

    gizmoType = -1;
    gizmoMode = ImGuizmo::MODE::LOCAL;

    handleSpace = HandleSpace::LOCAL;
    handlePosition = HandlePosition::PIVOT;

    easing = true;
    camSpeedMult = 4;

    renderLights = true;
    renderShadows = true;

    drawMesh = true;
    drawWireframe = false;
    drawNormalsVerts = false;
    drawNormalsFaces = false;
    drawAABB = true;
    drawOBB = false;
    drawChecker = false;

    snapping = false;
    snapAmount = 10;
}

PanelScene::~PanelScene() {}

void PanelScene::Start()
{
    // Create Editor Camera GO (Outside hierarchy)
    sceneCamera = std::make_shared<GameObject>("EDITOR CAMERA");
    sceneCamera.get()->AddComponent<Transform>();
    sceneCamera.get()->GetComponent<Transform>()->SetPosition(vec3f(0, 3, -10));
    sceneCamera.get()->AddComponent<Camera>();
    sceneCamera.get()->GetComponent<Camera>()->UpdateCamera();

    cameraParent = std::make_shared<GameObject>("CameraParent");
    cameraParent.get()->AddComponent<Transform>();
    cameraParent.get()->children.push_back(sceneCamera);
    sceneCamera.get()->parent = cameraParent;

    current = engine->N_sceneManager->currentScene;
    engine->SetEditorCamera(sceneCamera->GetComponent<Camera>());

    // Create Render Target
    std::vector<Attachment> gBuffAttachments = {
        { Attachment::Type::RGBA8, "color", "gBuffer", 0 },
        { Attachment::Type::RGB16F, "position", "gBuffer", 0 },
        { Attachment::Type::RGB16F, "normal", "gBuffer", 0 },
        { Attachment::Type::DEPTH_STENCIL, "depth", "gBuffer", 0 }
    };
    std::vector<Attachment> postBuffAttachments = {
        { Attachment::Type::RGBA8, "color", "postBuffer", 0 },
        { Attachment::Type::DEPTH_STENCIL, "depth", "postBuffer", 0 }
    };
    std::vector<Attachment> uiBuffAttachments = {
        { Attachment::Type::RGBA8, "color", "uiBuffer", 0 }
    };

    std::vector<std::vector<Attachment>> sceneBuffers{ gBuffAttachments, postBuffAttachments, uiBuffAttachments };

    viewportSize = { 640, 360 };
    renderTarget = Renderer::AddRenderTarget("Panel Scene", DrawMode::EDITOR, sceneCamera.get()->GetComponent<Camera>(), viewportSize, sceneBuffers);
}

bool PanelScene::Draw()
{
    isHovered = false;
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar;

	//ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.f, 0.f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
	ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Once);

	if (ImGui::Begin("Scene", &enabled, settingsFlags))
	{
        // ImGui Window status
        isHovered = ImGui::IsWindowHovered();
        isFocused = ImGui::IsWindowFocused();

        // Camera Control     
        CameraMode();
        CameraMovement(sceneCamera.get());

        // SDL Window
        int SDLWindowWidth, SDLWindowHeight;
        engine->window->GetSDLWindowSize(&SDLWindowWidth, &SDLWindowHeight);

        // ImGui Panel
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 availWindowSize = ImGui::GetContentRegionAvail();

        // Viewport Control ----------------------------------------------------------------
        // Aspect Ratio Size - only in Panel Game
        //int width, height;
        //app->gui->CalculateSizeAspectRatio(availWindowSize.x, availWindowSize.y, width, height, aspect);

        // Set glViewport coordinates
        // SDL origin at top left corner (+x >, +y v)
        // glViewport origin at bottom left corner  (+x >, +y ^)
        int x = static_cast<int>(windowPos.x);
        int y = SDLWindowHeight - windowPos.y - windowSize.y;


        // Top Bar -------------------------------------------------------------------------
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 10, 10 });
        if (ImGui::BeginMenuBar())
        {
            // HandlePosition
            int position = (int)handlePosition;
            ImGui::SetNextItemWidth(80);
            if (ImGui::Combo("##HandlePosition", &position, positions, 2))
            {
                handlePosition = (HandlePosition)position;
            }

            // HandleSpace
            ImGui::SetNextItemWidth(80);
            if (ImGui::Combo("##HandleSpace", &gizmoMode, spaces, 2))
            {
                //handleSpace = (HandleSpace)space;
                LOG(LogType::LOG_INFO, "gizMode: %d", gizmoMode);
            }

            ImGui::Dummy(ImVec2(MAX(availWindowSize.x - 360.0f, 0), 0.0f));

            if (ImGui::BeginMenu("Render"))
            {
                if (ImGui::Checkbox("Lights", &renderLights))
                    Renderer::SetRenderLights(renderLights);

                if (ImGui::Checkbox("Shadows", &renderShadows))
                    Renderer::SetRenderShadows(renderShadows);
                
                ImGui::Separator();

                ImGui::Checkbox("Mesh", &drawMesh);
                ImGui::Checkbox("Wireframe", &drawWireframe);
                ImGui::Separator();

                ImGui::Checkbox("Vertex normals", &drawNormalsVerts);
                ImGui::Checkbox("Face normals", &drawNormalsFaces);
                ImGui::Separator();

                ImGui::Checkbox("AABB", &drawAABB);
                ImGui::Checkbox("OBB", &drawOBB);
                ImGui::Separator();

                bool drawRayCasting = Renderer::GetDrawRaycasting();

                if (ImGui::Checkbox("Ray Casting", &drawRayCasting))
                {
                    Renderer::SetDrawRaycasting(drawRayCasting);
                    if (!Renderer::GetDrawRaycasting()) Renderer::ClearRays();
                }
                

                ImGui::EndMenu();
            }

			if (ImGui::BeginMenu("Camera"))
			{
				Camera* camera = sceneCamera.get()->GetComponent<Camera>();

                bool ortho = camera->cameraType == CameraType::ORTHOGRAPHIC ? true : false;
				float fov = static_cast<float>(camera->fov);
				float aspect = static_cast<float>(camera->aspect);
				float zNear = static_cast<float>(camera->zNear);
				float zFar = static_cast<float>(camera->zFar);

				ImGui::Text("Scene Camera");
                if (ImGui::Checkbox("Orthographic", &ortho))
                {
                    camera->cameraType = ortho ? CameraType::ORTHOGRAPHIC : CameraType::PERSPECTIVE;
                    camera->UpdateCamera();
                }

				if (ImGui::SliderFloat("FOV", &fov, 4.0, 120.0))
                    sceneCamera.get()->GetComponent<Camera>()->UpdateCamera();

                if (ImGui::SliderFloat("Aspect", &aspect, 0.1, 10.0))
                    sceneCamera.get()->GetComponent<Camera>()->UpdateCamera();
                ImGui::Dummy({ 0, 4 });

				ImGui::Text("Clipping Plane");
                if (ImGui::SliderFloat("Near", &zNear, 0.01, 10.0))
                    sceneCamera.get()->GetComponent<Camera>()->UpdateCamera();

                if (ImGui::SliderFloat("Far ", &zFar, 1.0, 20000.0))
                    sceneCamera.get()->GetComponent<Camera>()->UpdateCamera();
                ImGui::Dummy({ 0, 4 });

                ImGui::Text("Navigation");
                ImGui::Checkbox("Easing", &easing);
                ImGui::SliderFloat("Speed ", &camSpeedMult, 1, 10);

				camera->fov = fov;
				camera->aspect = aspect;
				camera->zNear = zNear;
				camera->zFar = zFar;

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Gizmo"))
            {
                ImGui::Checkbox("Snap", &snapping);
                ImGui::SliderInt("Amount", &snapAmount, 1, 100);

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
        ImGui::PopStyleVar();


        // Render --------------------------------------------------------------------------
        
        Renderer::GetRenderTarget(renderTarget)->SetActive(true);

        // Viewport resize check
        viewportSize = { availWindowSize.x, availWindowSize.y };
        
        if (viewportSize.x > 0.0f && viewportSize.y > 0.0f && 
            (Renderer::GetFrameBuffer(renderTarget, "gBuffer")->GetWidth() != viewportSize.x || 
                Renderer::GetFrameBuffer(renderTarget, "gBuffer")->GetHeight() != viewportSize.y))
        {
            Renderer::GetFrameBuffer(renderTarget, "gBuffer")->Resize(viewportSize.x, viewportSize.y);
            Renderer::GetFrameBuffer(renderTarget, "postBuffer")->Resize(viewportSize.x, viewportSize.y);
            sceneCamera.get()->GetComponent<Camera>()->aspect = viewportSize.x / viewportSize.y;
            sceneCamera.get()->GetComponent<Camera>()->UpdateCamera();
        }

        ImTextureID textureID = Renderer::GetRenderLights() ?
            (ImTextureID)Renderer::GetFrameBuffer(renderTarget, "postBuffer")->GetAttachmentTexture("color") :
            (ImTextureID)Renderer::GetFrameBuffer(renderTarget, "gBuffer")->GetAttachmentTexture("color");
        
        ImGui::Image(textureID, ImVec2{ viewportSize.x, viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });


        // ImGuizmo ------------------------------------------------------------------------
        // Handle Input
        if (!engine->inputManager->GetMouseButton(SDL_BUTTON_RIGHT))
        {
            if (engine->inputManager->GetKey(SDL_SCANCODE_Q) == KEY_DOWN) gizmoType = (ImGuizmo::OPERATION)-1;
            if (engine->inputManager->GetKey(SDL_SCANCODE_W) == KEY_DOWN) gizmoType = ImGuizmo::OPERATION::TRANSLATE;
            if (engine->inputManager->GetKey(SDL_SCANCODE_E) == KEY_DOWN) gizmoType = ImGuizmo::OPERATION::ROTATE;
            if (engine->inputManager->GetKey(SDL_SCANCODE_R) == KEY_DOWN) gizmoType = ImGuizmo::OPERATION::SCALE;
        }       

        // Gizmo
        shared_ptr<GameObject> selectedGO = engine->N_sceneManager->GetSelectedGO();

        if (selectedGO && gizmoType != -1)
        {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            int viewportTopLeftY = windowPos.y + (windowSize.y - availWindowSize.y);
            ImGuizmo::SetRect(windowPos.x, viewportTopLeftY, viewportSize.x, viewportSize.y);

            //Camera
            const glm::mat4& cameraProjection = sceneCamera->GetComponent<Camera>()->projectionMatrix;
            glm::mat4 cameraView = sceneCamera->GetComponent<Camera>()->viewMatrix;

            //Entity Transform
            Transform* tc = selectedGO->GetComponent<Transform>();
            glm::mat4 transform = tc->CalculateWorldTransform();

            // Snap
            bool snappingSC = engine->inputManager->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT ? true : false;            
            vec3f snapSize = vec3f(snapAmount, snapAmount, snapAmount);

            ImGuizmo::Manipulate(
                glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                (ImGuizmo::OPERATION)gizmoType, (ImGuizmo::MODE)gizmoMode,
                glm::value_ptr(transform), NULL, snapping || snappingSC ? &snapSize.x : NULL);

            if (ImGuizmo::IsUsing())
            {
                tc->SetTransform(tc->WorldToLocalTransform(selectedGO.get(), transform));
                tc->DecomposeTransform();

                app->gui->panelInspector->OnSelectGO(selectedGO);
            }
        }


        // Mouse Picking -------------------------------------------------------------------
        if (isHovered && engine->inputManager->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN) //hekbas need to chek if using imguizmo?
        {
            int sdlY = SDLWindowHeight - y - viewportSize.y;
            auto clickPos = glm::vec2(engine->inputManager->GetMouseX() - x, engine->inputManager->GetMouseY() - sdlY);

			Ray ray = GetScreenRay(int(clickPos.x), int(clickPos.y), sceneCamera->GetComponent<Camera>(), viewportSize.x, viewportSize.y);

            if (Renderer::GetDrawRaycasting()) Renderer::AddRay(ray);
            
            //editor->SelectObject(ray);
        }
	}
    else
    {
        Renderer::GetRenderTarget(renderTarget)->SetActive(false);
    }
	ImGui::End();
	ImGui::PopStyleVar();

	return true;
}

Ray PanelScene::GetScreenRay(int x, int y, Camera* camera, int width, int height)
{
	if (!camera)
	{
		LOG(LogType::LOG_ERROR, "Invalid camera, can't create Ray");
		return Ray();
	}

	//Normalised Device Coordinates [-1, 1]
	float rayX = (2.0f * x) / width - 1.0f;
	float rayY = -(2.0f * y) / height + 1.0f;

	return camera->ComputeCameraRay(rayX, rayY);
}

void PanelScene::CameraMode()
{
    bool disable = false;

    switch (camControlMode)
    {
        case CamControlMode::ENABLED:
        {
            if (engine->inputManager->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_UP)
                disable = true;

            if (engine->inputManager->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_DOWN)
                camControlMode = CamControlMode::PANNING;

            if (engine->inputManager->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT &&
                engine->inputManager->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
                camControlMode = CamControlMode::ORBIT;
        }
        break;

        case CamControlMode::PANNING:
        {
            if (engine->inputManager->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_UP)
                camControlMode = CamControlMode::DISABLED;

            if (engine->inputManager->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN)
                camControlMode = CamControlMode::ENABLED;

            if (engine->inputManager->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT &&
                engine->inputManager->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
                camControlMode = CamControlMode::ORBIT;
        }
        break;

        case CamControlMode::ORBIT:
        {
            if (engine->inputManager->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
                camControlMode = CamControlMode::DISABLED;

            if (engine->inputManager->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_DOWN)
                camControlMode = CamControlMode::PANNING;

            if (engine->inputManager->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN)
                camControlMode = CamControlMode::ENABLED;
        }
        break;

        case CamControlMode::DISABLED:
        {
            if (isHovered)
            {
                if (engine->inputManager->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN)
                    camControlMode = CamControlMode::ENABLED;

                if (engine->inputManager->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_DOWN)
                    camControlMode = CamControlMode::PANNING;

                if (engine->inputManager->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT &&
                    engine->inputManager->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
                    camControlMode = CamControlMode::ORBIT;
            }
        }
        break;
    }

    if (disable)
    {
        while (!inputStack.empty()) inputStack.pop();
        camControlMode = CamControlMode::DISABLED;
    }
}

void PanelScene::CameraMovement(GameObject* cam)
{
    Camera* camera = cam->GetComponent<Camera>();
    Transform* transform = cam->GetComponent<Transform>();
    transform->Update(engine->dt);
    double dt = MIN(app->GetDT(), 0.016);
    double mouseSensitivity = 28.0;

    switch (camControlMode)
    {
        // (RMB)
        case CamControlMode::ENABLED:
        {
            engine->window->InfiniteScroll();

            camera->yaw = engine->inputManager->GetMouseXMotion() * mouseSensitivity * dt;
            camera->pitch = -engine->inputManager->GetMouseYMotion() * mouseSensitivity * dt;
            transform->Rotate(vec3f(0.0f, camera->yaw, 0.0f), HandleSpace::GLOBAL);
            transform->Rotate(vec3f(camera->pitch, 0.0f, 0.0f), HandleSpace::LOCAL);

            HandleInput(SDL_SCANCODE_A);
            HandleInput(SDL_SCANCODE_D);
            HandleInput(SDL_SCANCODE_W);
            HandleInput(SDL_SCANCODE_S);

            SetTargetSpeed();
        }
        break;

        // (MMB)
        case CamControlMode::PANNING:
        {
            engine->window->InfiniteScroll();

            double deltaX = engine->inputManager->GetMouseXMotion();
            double deltaY = engine->inputManager->GetMouseYMotion();
            float panSpeed = 10 * dt;
            transform->Translate(vec3(deltaX * panSpeed, 0, 0), HandleSpace::GLOBAL);
            transform->Translate(vec3(0, deltaY * panSpeed, 0), HandleSpace::GLOBAL);
        }
        break;

        // (Alt + LMB)
        case CamControlMode::ORBIT:
        {
            engine->window->InfiniteScroll();

            camera->yaw = engine->inputManager->GetMouseXMotion() * mouseSensitivity * dt;
            camera->pitch = -engine->inputManager->GetMouseYMotion() * mouseSensitivity * dt;

            transform->SetPosition(camera->lookAt);

            transform->Rotate(vec3f(0.0f, camera->yaw, 0.0f), HandleSpace::GLOBAL);
            transform->Rotate(vec3f(camera->pitch, 0.0f, 0.0f), HandleSpace::LOCAL);

            vec3f finalPos;

            auto selectedGO = engine->N_sceneManager->GetSelectedGO();
            if (selectedGO)
                finalPos = selectedGO->GetComponent<Transform>()->GetPosition() - (transform->GetForward() * 40.0);
            else
                finalPos = transform->GetPosition() - transform->GetForward();

            transform->SetPosition(finalPos);
        }
        break;

        case CamControlMode::DISABLED:
        {
            camTargetSpeed.x = 0;
            camTargetSpeed.y = 0;
        }
        break;
    }

    // Camera Speed Easing
    if (camTargetSpeed.x)
    {
        engine->easingManager->GetEasing(camEaseInX).Play();
        engine->easingManager->GetEasing(camEaseOutX).Reset();

        camCurrentSpeed.x = engine->easingManager->GetEasing(camEaseInX).Ease(
            camInitSpeed.x, camTargetSpeed.x, dt, EasingType::EASE_IN_SIN, false);
    }
    else if (camCurrentSpeed.x)
    {
        engine->easingManager->GetEasing(camEaseOutX).Play();
        engine->easingManager->GetEasing(camEaseInX).Reset();

        camCurrentSpeed.x = engine->easingManager->GetEasing(camEaseOutX).Ease(
            camInitSpeed.x, camTargetSpeed.x, dt, EasingType::EASE_OUT_SIN, false);
    }

    if (camTargetSpeed.y)
    {
        engine->easingManager->GetEasing(camEaseInY).Play();
        engine->easingManager->GetEasing(camEaseOutY).Reset();

        camCurrentSpeed.y = engine->easingManager->GetEasing(camEaseInY).Ease(
            camInitSpeed.y, camTargetSpeed.y, dt, EasingType::EASE_IN_SIN, false);
    }
    else if (camCurrentSpeed.y)
    {
        engine->easingManager->GetEasing(camEaseOutY).Play();
        engine->easingManager->GetEasing(camEaseInY).Reset();

        camCurrentSpeed.y = engine->easingManager->GetEasing(camEaseOutY).Ease(
            camInitSpeed.y, camTargetSpeed.y, dt, EasingType::EASE_OUT_SIN, false);
    }

    double shift = engine->inputManager->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT ? 2 : 1;
    double speedX = camCurrentSpeed.x * camSpeedMult * shift * dt;
    double speedY = camCurrentSpeed.y * camSpeedMult * shift * dt;

    // Move
    transform->Translate(speedX * transform->GetRight(), HandleSpace::LOCAL);
    transform->Translate(speedY * transform->GetForward(), HandleSpace::LOCAL);

    // (Wheel) Zoom
    if (isHovered && engine->inputManager->GetMouseZ() != 0)
        transform->Translate(transform->GetForward() * (double)engine->inputManager->GetMouseZ() * 4.0, HandleSpace::LOCAL);

    // (F) Focus Selection
    if (engine->inputManager->GetKey(SDL_SCANCODE_F) == KEY_DOWN && engine->N_sceneManager->GetSelectedGO())
    {
        transform->SetPosition(camera->lookAt);
        vec3f finalPos;
        finalPos = transform->GetPosition() - transform->GetForward();
        finalPos = engine->N_sceneManager->GetSelectedGO().get()->GetComponent<Transform>()->GetPosition() - (transform->GetForward() * 10.0);

        transform->SetPosition(finalPos);
    }
}

void PanelScene::HandleInput(SDL_Scancode key)
{
    if (engine->inputManager->GetKey(key) == KEY_DOWN)
    {
        inputStack.push(key);
    }
    else if (engine->inputManager->GetKey(key) == KEY_UP)
    {
        std::stack<SDL_Scancode> tempStack;

        while (!inputStack.empty())
        {
            if (inputStack.top() != key)
                tempStack.push(inputStack.top());
            inputStack.pop();
        }
        while (!tempStack.empty())
        {
            inputStack.push(tempStack.top());
            tempStack.pop();
        }
    }

    if (engine->inputManager->GetKey(key) == KEY_UP ||
        engine->inputManager->GetKey(key) == KEY_DOWN)
    {
        switch (key)
        {
            case SDL_SCANCODE_A: camInitSpeed.x = camCurrentSpeed.x; break;
            case SDL_SCANCODE_D: camInitSpeed.x = camCurrentSpeed.x; break;
            case SDL_SCANCODE_W: camInitSpeed.y = camCurrentSpeed.y; break;
            case SDL_SCANCODE_S: camInitSpeed.y = camCurrentSpeed.y; break;
            default: break;
        }
    }
}

void PanelScene::SetTargetSpeed()
{
    std::stack<SDL_Scancode> tempStack = inputStack;
    std::stack<SDL_Scancode> reversedStack; 

    // Reverse the order of elements
    while (!tempStack.empty())
    {
        reversedStack.push(tempStack.top());
        tempStack.pop();
    }

    bool zeroX = true;
    bool zeroY = true;

    // Compute speed
    while (!reversedStack.empty())
    {
        switch (reversedStack.top())
        {
            case SDL_SCANCODE_A:
                camTargetSpeed.x =  50;
                zeroX = false;
                break;

            case SDL_SCANCODE_D:
                camTargetSpeed.x = -50;
                zeroX = false;
                break;

            case SDL_SCANCODE_W:
                camTargetSpeed.y =  50;
                zeroY = false;
                break;

            case SDL_SCANCODE_S:
                camTargetSpeed.y = -50;
                zeroY = false;
                break;

            default: break;
        }

        reversedStack.pop();
    }

    if (zeroX) camTargetSpeed.x = 0;
    if (zeroY) camTargetSpeed.y = 0;
}