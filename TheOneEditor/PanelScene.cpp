#include "PanelScene.h"
#include "App.h"
#include "Gui.h"
#include "PanelInspector.h"
#include "Renderer3D.h"
#include "SceneManager.h"
#include "Window.h"
#include "imgui.h"
#include "imGuizmo.h"

#include "TheOneEngine/EngineCore.h"
#include "TheOneEngine/Ray.h"
#include "TheOneEngine/Log.h"
#include "TheOneEngine/Easing.h"

#include <vector>

PanelScene::PanelScene(PanelType type, std::string name) : Panel(type, name)
{
    isHovered = false;
    isFocused = false;
    cameraControl = false;

	handleSpace = HandleSpace::LOCAL;
    handlePosition = HandlePosition::PIVOT;
    gizmoType = -1;
    gizmoMode = ImGuizmo::MODE::LOCAL;

    frameBuffer = std::make_shared<FrameBuffer>(1280, 720, true);

    cameraIn = make_unique<Easing>(0.6);
    cameraOut = make_unique<Easing>(0.3);

    snapping = false;
    snapAmount = 10;

    easing = true;
    camTargetSpeed = 2;
    camSpeed = 0;
    camSpeedOut = 0;
    camKeyOut = 'n';

    drawMesh = true;
    drawWireframe = false;
    drawNormalsVerts = false;
    drawNormalsFaces = false;
    drawAABB = true;
    drawOBB = false;
    drawRaycasting = false;
    drawChecker = false;
}

PanelScene::~PanelScene() {}

void PanelScene::Start()
{
    // Creating Editor Camera GO (Outside hierarchy)
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
}

bool PanelScene::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar;

	//ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.f, 0.f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
	ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Once);

	if (ImGui::Begin("Scene", &enabled, settingsFlags))
	{
        // ImGui Window focus - camera controls
        isHovered = ImGui::IsWindowHovered();
        isFocused = ImGui::IsWindowFocused();

        // Camera Controls
        if (isHovered || cameraControl)
            CameraInput(sceneCamera.get());

        // Finish Camera Ease Out even if scene isn't hovered
        else if (camKeyOut != 'n')
        {
            // Don't move twice in the same frame
            if (app->input->GetKey(SDL_SCANCODE_W) != KEY_UP &&
                app->input->GetKey(SDL_SCANCODE_S) != KEY_UP &&
                app->input->GetKey(SDL_SCANCODE_A) != KEY_UP &&
                app->input->GetKey(SDL_SCANCODE_D) != KEY_UP)
            {
                MoveCamera();
            }
        }

        // SDL Window
        int SDLWindowWidth, SDLWindowHeight;
        app->window->GetSDLWindowSize(&SDLWindowWidth, &SDLWindowHeight);

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
                ImGui::Checkbox("Mesh", &drawMesh);
                ImGui::Checkbox("Wireframe", &drawWireframe);
                ImGui::Separator();

                ImGui::Checkbox("Vertex normals", &drawNormalsVerts);
                ImGui::Checkbox("Face normals", &drawNormalsFaces);
                ImGui::Separator();

                ImGui::Checkbox("AABB", &drawAABB);
                ImGui::Checkbox("OBB", &drawOBB);
                ImGui::Separator();

                if (ImGui::Checkbox("Ray Casting", &drawRaycasting))
                {
                    if (!drawRaycasting) rays.clear();
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
                ImGui::SliderFloat("Speed ", &camTargetSpeed, 1, 10);

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


        // Viewport resize check
        viewportSize = { availWindowSize.x, availWindowSize.y };

        if (viewportSize.x > 0.0f && viewportSize.y > 0.0f && // zero sized framebuffer is invalid
            (frameBuffer->getWidth() != viewportSize.x || frameBuffer->getHeight() != viewportSize.y))
        {
            frameBuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);

            sceneCamera.get()->GetComponent<Camera>()->aspect = viewportSize.x / viewportSize.y;
            sceneCamera.get()->GetComponent<Camera>()->UpdateCamera();
        }

        // ALL DRAWING MUST HAPPEN BETWEEN FB BIND/UNBIND ----------------------------------
        {
            frameBuffer->Bind();
            frameBuffer->Clear();

            // Set Render Environment
            engine->Render(sceneCamera->GetComponent<Camera>());
            engine->SetUniformBufferCamera(sceneCamera->GetComponent<Camera>());

            // Game cameras Frustum
            for (const auto GO : engine->N_sceneManager->GetGameObjects())
            {
                Camera* gameCam = GO.get()->GetComponent<Camera>();

                if (gameCam != nullptr && gameCam->drawFrustum)
                    engine->DrawFrustum(gameCam->frustum);
            }

            // Draw Rays
            if (drawRaycasting)
            {
                for (auto ray : rays)
                {
                    engine->DrawRay(ray);
                }
            }

            // Draw Scene
            current->Draw(DrawMode::EDITOR, sceneCamera->GetComponent<Camera>());

            // Draw Loading Screen - only on panel game
            /*if (engine->N_sceneManager->GetSceneIsChanging())
                engine->N_sceneManager->loadingScreen->DrawUI(engine->N_sceneManager->currentScene->currentCamera, DrawMode::GAME);*/

            frameBuffer->Unbind();
        }

        // Draw FrameBuffer Texture
        ImGui::Image(
            (ImTextureID)frameBuffer->getColorBufferTexture(),
            ImVec2{ viewportSize.x, viewportSize.y },
            ImVec2{ 0, 1 }, ImVec2{ 1, 0 });


        // ImGuizmo ------------------------------------------------------------------------
        // Handle Input
        if (!app->input->GetMouseButton(SDL_BUTTON_RIGHT))
        {
            if (app->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN) gizmoType = (ImGuizmo::OPERATION)-1;
            if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) gizmoType = ImGuizmo::OPERATION::TRANSLATE;
            if (app->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN) gizmoType = ImGuizmo::OPERATION::ROTATE;
            if (app->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN) gizmoType = ImGuizmo::OPERATION::SCALE;
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
            bool snappingSC = app->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT ? true : false;            
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
        if (isHovered && app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN) //hekbas need to chek if using imguizmo?
        {
            int sdlY = SDLWindowHeight - y - viewportSize.y;
            auto clickPos = glm::vec2(app->input->GetMouseX() - x, app->input->GetMouseY() - sdlY);

			Ray ray = GetScreenRay(int(clickPos.x), int(clickPos.y), sceneCamera->GetComponent<Camera>(), viewportSize.x, viewportSize.y);

            if (drawRaycasting) rays.push_back(ray);
            
            //editor->SelectObject(ray);
        }
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

void PanelScene::CameraInput(GameObject* cam)
{
    Camera* camera = cam->GetComponent<Camera>();
    Transform* transform = cam->GetComponent<Transform>();
    double dt = app->GetDT();

    double mouseSensitivity = 32.0 * dt;

    if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
    {
        cameraControl = true;

        // Yaw and Pitch
        camera->yaw = app->input->GetMouseXMotion() * mouseSensitivity;
        camera->pitch = -app->input->GetMouseYMotion() * mouseSensitivity;
        transform->Rotate(vec3f(0.0f, camera->yaw, 0.0f), HandleSpace::GLOBAL);
        transform->Rotate(vec3f(camera->pitch, 0.0f, 0.0f), HandleSpace::LOCAL);

        MoveCamera();
    }
    else if (camKeyOut != 'n')
    {
        MoveCamera();
    }
    else if (app->input->GetMouseZ() != 0)
    {
        // Zoom
        transform->Translate(transform->GetForward() * (double)app->input->GetMouseZ());
    }
    else if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_UP)
    {
        cameraControl = false;
    }

    // (MMB) Panning
    if (app->input->GetMouseButton(SDL_BUTTON_MIDDLE))
    {
        double deltaX = app->input->GetMouseXMotion();
        double deltaY = app->input->GetMouseYMotion();

        float panSpeed = 10 * dt;

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

void PanelScene::MoveCamera()
{
    // Camera Speed
    if (easing)
    {
        if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT ||
            app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT ||
            app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT ||
            app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
        {
            camKeyOut = 'n';
            camSpeed = cameraIn.get()->Ease(0, camTargetSpeed, app->GetDT(), EasingType::EASE_IN_SIN);
        }
        else if (app->input->GetKey(SDL_SCANCODE_W) == KEY_UP)
        {
            camKeyOut = 'w';
            cameraIn.get()->Reset();
            camSpeedOut = camSpeed;
        }
        else if (app->input->GetKey(SDL_SCANCODE_S) == KEY_UP)
        {
            camKeyOut = 's';
            cameraIn.get()->Reset();
            camSpeedOut = camSpeed;
        }
        else if (app->input->GetKey(SDL_SCANCODE_A) == KEY_UP)
        {
            camKeyOut = 'a';
            cameraIn.get()->Reset();
            camSpeedOut = camSpeed;
        }
        else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_UP)
        {
            camKeyOut = 'd';
            cameraIn.get()->Reset();
            camSpeedOut = camSpeed;
        }
        else if (!cameraOut.get()->GetFinished()) //hekbas maybe easing finished better
        {
            camSpeed = cameraOut.get()->Ease(camSpeedOut, 0, app->GetDT(), EasingType::EASE_OUT_SIN);

            if (cameraOut.get()->GetFinished())
            {
                cameraOut.get()->Reset();
                camKeyOut = 'n';
            }
        }
    }
    else
    {
        camSpeed = camTargetSpeed;
    }

    if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
        camSpeed *= 2;

    // Movement
    Transform* transform = sceneCamera->GetComponent<Transform>();

    if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT || camKeyOut == 'w')
        transform->Translate(transform->GetForward() * camSpeed, HandleSpace::LOCAL);

    if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT || camKeyOut == 's')
        transform->Translate(-transform->GetForward() * camSpeed, HandleSpace::LOCAL);

    if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || camKeyOut == 'a')
        transform->Translate(transform->GetRight() * camSpeed, HandleSpace::LOCAL);

    if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT || camKeyOut == 'd')
        transform->Translate(-transform->GetRight() * camSpeed, HandleSpace::LOCAL);
}