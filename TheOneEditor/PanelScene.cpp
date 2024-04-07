#include "PanelScene.h"
#include "App.h"
#include "Gui.h"
#include "PanelInspector.h"
#include "Renderer3D.h"
#include "SceneManager.h"
#include "Window.h"
#include "imgui.h"
#include "imGuizmo.h"

#include "../TheOneEngine/EngineCore.h"
#include "../TheOneEngine/Ray.h"
#include "../TheOneEngine/Log.h"

#include <vector>

PanelScene::PanelScene(PanelType type, std::string name) : Panel(type, name), isHovered(false)
{
    drawMesh = true;
    drawWireframe = false;
    drawNormalsVerts = false;
    drawNormalsFaces = false;
    drawAABB = true;
    drawOBB = false;
    drawRaycasting = false;
    drawChecker = false;

	handleSpace = HandleSpace::LOCAL;
    handlePosition = HandlePosition::PIVOT;
    gizmoType = -1;
    gizmoMode = ImGuizmo::MODE::LOCAL;

    frameBuffer = std::make_shared<FrameBuffer>(1280, 720, true);
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
    if (viewportSize.x > 0.0f && viewportSize.y > 0.0f && // zero sized framebuffer is invalid
        (frameBuffer->getWidth() != viewportSize.x || frameBuffer->getHeight() != viewportSize.y))
    {
        frameBuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
        sceneCamera.get()->GetComponent<Camera>()->aspect = viewportSize.x / viewportSize.y;
    }

	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar;

	//ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.f, 0.f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
	ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Once);

	if (ImGui::Begin("Scene", &enabled, settingsFlags))
	{
        isHovered = ImGui::IsWindowHovered();
        isFocused = ImGui::IsWindowFocused();

        if(isFocused)
            app->renderer3D->CameraInput(sceneCamera.get());


        // SDL Window
        int SDLWindowWidth, SDLWindowHeight;
        app->window->GetSDLWindowSize(&SDLWindowWidth, &SDLWindowHeight);

        // ImGui Panel
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 availWindowSize = ImGui::GetContentRegionAvail();

        // Viewport Control ----------------------------------------------------------------
        // Aspect Ratio Size
        int width, height;
        app->gui->CalculateSizeAspectRatio(availWindowSize.x, availWindowSize.y, width, height);

        // Set glViewport coordinates
        // SDL origin at top left corner (+x >, +y v)
        // glViewport origin at bottom left corner  (+x >, +y ^)
        int x = static_cast<int>(windowPos.x);
        int y = SDLWindowHeight - windowPos.y - windowSize.y;


        // Top Bar -------------------------------------------------------------------------
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginMenuBar())
        {
            // HandlePosition
            int position = (int)handlePosition;
            ImGui::SetNextItemWidth(80);
            if (ImGui::Combo("##HandlePosition", &position, positions, 2))
            {
                handlePosition = (HandlePosition)position;
                ImGui::EndCombo();
            }

            // HandleSpace
            ImGui::SetNextItemWidth(80);
            if (ImGui::Combo("##HandleSpace", &gizmoMode, spaces, 2))
            {
                //handleSpace = (HandleSpace)space;
                LOG(LogType::LOG_INFO, "gizMode: %d", gizmoMode);
                ImGui::EndCombo();
            }

            ImGui::Dummy(ImVec2(availWindowSize.x - 360.0f, 0.0f));

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
				// Camera settings
				ImGui::TextWrapped("Scene Camera");

				Camera* camera = sceneCamera.get()->GetComponent<Camera>();

				float fov = static_cast<float>(camera->fov);
				float aspect = static_cast<float>(camera->aspect);
				float zNear = static_cast<float>(camera->zNear);
				float zFar = static_cast<float>(camera->zFar);

				ImGui::SliderFloat("FOV", &fov, 20.0, 120.0);
				ImGui::SliderFloat("Aspect", &aspect, 0.1, 10.0);
				ImGui::Text("Clipping Plane");
				ImGui::SliderFloat("Near", &zNear, 0.01, 10.0);
				ImGui::SliderFloat("Far ", &zFar, 1.0, 20000.0);

				camera->fov = fov;
				camera->aspect = aspect;
				camera->zNear = zNear;
				camera->zFar = zFar;

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Gizmo"))
            {
                ImGui::Text("Gizmo Options");
                ImGui::Text("It seems like hekbas forgot to implement ImGuizmo...");

                ImGui::EndMenu();
            }

            ImGui::PopStyleVar();
            ImGui::EndMenuBar();
        }

        engine->LogGL("PanelScene.cpp 188");

        //Draw FrameBuffer Texture
        viewportSize = { availWindowSize.x, availWindowSize.y };
        ImGui::Image((ImTextureID)frameBuffer->getColorBufferTexture(), ImVec2{ viewportSize.x, viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        engine->LogGL("PanelScene.cpp 194");


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

            ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                (ImGuizmo::OPERATION)gizmoType, (ImGuizmo::MODE)gizmoMode, glm::value_ptr(transform));

            if (ImGuizmo::IsUsing())
            {
                tc->SetTransform(tc->WorldToLocalTransform(selectedGO.get(), transform));
                tc->DecomposeTransform();

                app->gui->panelInspector->OnSelectGO(selectedGO);
            }
        }


        // Mouse Picking -------------------------------------------------------------------
        if (isHovered && app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
        {
            int sdlY = SDLWindowHeight - y - height;
            auto clickPos = glm::vec2(app->input->GetMouseX() - x, app->input->GetMouseY() - sdlY);

			Ray ray = GetScreenRay(int(clickPos.x), int(clickPos.y), sceneCamera->GetComponent<Camera>(), width, height);

            if (drawRaycasting) rays.push_back(ray);
            
            //editor->SelectObject(ray);
        }


        //ALL DRAWING MUST HAPPEN BETWEEN FB BIND/UNBIND
        {
            frameBuffer->Bind();
            engine->LogGL("Bind was Mondongo");

            frameBuffer->Clear();
            engine->LogGL("Clear was Mondongo");

            frameBuffer->ClearBuffer(-1);
            engine->LogGL("ClearBuffer was Mondongo");


            // Draw
            engine->Render(sceneCamera->GetComponent<Camera>());

            // Game cameras Frustum
            for (const auto GO : engine->N_sceneManager->GetGameObjects())
            {
                Camera* gameCam = GO.get()->GetComponent<Camera>();

                if (gameCam != nullptr && gameCam->drawFrustum)
                    engine->DrawFrustum(gameCam->frustum);
            }

            //Draw Rays
            if (drawRaycasting)
            {
                for (auto ray : rays)
                {
                    engine->DrawRay(ray);
                }
            }

            current->Draw(DrawMode::EDITOR);
            if (engine->N_sceneManager->GetSceneIsChanging())
                engine->N_sceneManager->loadingScreen->DrawUI(engine->N_sceneManager->currentScene->currentCamera, DrawMode::GAME);

            frameBuffer->Unbind();
        }

	}

    engine->LogGL("PanelScene.cpp 289");


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
