#include "App.h"

#include "Gui.h"
#include "Window.h"
#include "Hardware.h"
#include "SceneManager.h"

#include "Panel.h"
#include "PanelAbout.h"
#include "PanelConsole.h"
#include "PanelHierarchy.h"
#include "PanelInspector.h"
#include "PanelProject.h"
#include "PanelScene.h"
#include "PanelGame.h"
#include "PanelAnimation.h"
#include "PanelRenderer.h"
#include "PanelSettings.h"
#include "PanelBuild.h"

#include "TheOneEngine/Log.h"
#include "TheOneEngine/Light.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include "imGuizmo.h"

#include <filesystem>
#include <windows.h>
#include <shellapi.h>
#include <Time.h>

namespace fs = std::filesystem;

Gui::Gui(App* app) :
	Module(app),
	panelAbout(nullptr),
	panelConsole(nullptr),
	panelHierarchy(nullptr),
	panelInspector(nullptr),
	panelProject(nullptr),
	panelScene(nullptr),
	panelGame(nullptr),
	panelAnimation(nullptr),
	panelRenderer(nullptr),
	panelSettings(nullptr),
	panelBuild(nullptr)
{
	editColor = { "null", { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } };
}

Gui::~Gui()
{
	for (auto panel : panels)
		delete panel;
}

bool Gui::Awake()
{
	LOG(LogType::LOG_INFO, "# Initializing GUI Panels...");

	bool ret = true;

	panelAbout = new PanelAbout(PanelType::ABOUT, "About");
	ret *= IsInitialized(panelAbout);
	panels.push_back(panelAbout);

	panelConsole = new PanelConsole(PanelType::CONSOLE, "Console");
	panels.push_back(panelConsole);
	ret *= IsInitialized(panelConsole);

	panelHierarchy = new PanelHierarchy(PanelType::HIERARCHY, "Hierarchy");
	panels.push_back(panelHierarchy);
	ret *= IsInitialized(panelHierarchy);

	panelInspector = new PanelInspector(PanelType::INSPECTOR, "Inspector");
	panels.push_back(panelInspector);
	ret *= IsInitialized(panelInspector);

	panelProject = new PanelProject(PanelType::PROJECT, "Project");
	panels.push_back(panelProject);
	ret *= IsInitialized(panelProject);

	panelScene = new PanelScene(PanelType::SCENE, "Scene");
	panels.push_back(panelScene);
	ret *= IsInitialized(panelScene);

	panelGame = new PanelGame(PanelType::GAME, "Game");
	panels.push_back(panelGame);
	ret *= IsInitialized(panelGame);

	panelAnimation = new PanelAnimation(PanelType::ANIMATION, "Animation");
	panels.push_back(panelAnimation);
	ret *= IsInitialized(panelAnimation);

	panelRenderer = new PanelRenderer(PanelType::RENDERER, "Renderer");
	panels.push_back(panelRenderer);
	ret *= IsInitialized(panelRenderer);

	panelSettings = new PanelSettings(PanelType::SETTINGS, "Settings");
	panels.push_back(panelSettings);
	ret *= IsInitialized(panelSettings);
	
	panelBuild = new PanelBuild(PanelType::BUILD, "Build");
	panels.push_back(panelBuild);
	ret *= IsInitialized(panelBuild);

	return ret;
}

bool Gui::Start()
{
	LOG(LogType::LOG_INFO, "# Initializing ImGui/ImPlot...");

	IMGUI_CHECKVERSION();
	LOG(LogType::LOG_OK, "-ImGui version: %s", IMGUI_VERSION);

	ImGuiContext* contextui = ImGui::CreateContext();
	ImPlotContext* contextplot = ImPlot::CreateContext();

	if (!contextui)
		LOG(LogType::LOG_ERROR, "-Missing Dear ImGui context. Refer to examples app!");
	LOG(LogType::LOG_OK, "-Creating ImGui Context");

	if (!contextplot)
		LOG(LogType::LOG_ERROR, "-Creating Implot context");
	LOG(LogType::LOG_OK, "-Creating ImPlot Context");

	// Input/Output
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	if (!&io)
		LOG(LogType::LOG_ERROR, "-Enabling I/O");
	LOG(LogType::LOG_OK, "-Enabling I/O");

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(app->window->window, app->window->glContext);
	ImGui_ImplOpenGL3_Init("#version 450");

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Custom Style
	#pragma region IMGUI_STYLE

	LOG(LogType::LOG_OK, "-Setting ImGui Custom Style");
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = ImVec4(0.88f, 0.88f, 0.88f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
	style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
	style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.60f);
	style.GrabRounding = style.FrameRounding = 2.3f;
	style.FramePadding = { 4, 4 };
	style.WindowMenuButtonPosition = -1;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	#pragma endregion IMGUI_STYLE

	// Enable Panels
	bool active = true;
	app->gui->panelInspector->SetState(active);
	app->gui->panelProject->SetState(active);
	app->gui->panelConsole->SetState(active);
	app->gui->panelHierarchy->SetState(active);
	app->gui->panelScene->SetState(active);
	app->gui->panelGame->SetState(active);
	app->gui->panelAnimation->SetState(active);

	// Iterate Panels & Start
	for (const auto& panel : panels)
		panel->Start();

	return true;
}

bool Gui::PreUpdate()
{
	bool ret = true;

    // Clears GUI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(app->window->window);
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    // Dockspace
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
        MainWindowDockspace();

	return ret;
}

bool Gui::Update(double dt)
{
	bool ret = true;

    // Creates the Main Menu Bar
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ret = MainMenuFile();
            ImGui::EndMenu();
        }

		if (ImGui::BeginMenu("Edit"))
		{
			MainMenuEdit();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Assets"))
		{
			MainMenuAssets();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("GameObject"))
		{
			MainMenuGameObject();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Component"))
		{
			MainMenuComponent();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			MainMenuWindow();
			ImGui::EndMenu();
		}

        if (ImGui::BeginMenu("Help"))
        {
            MainMenuHelp();
            ImGui::EndMenu();
        }

		// Play/Pause/Stop
		ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x / 2 - 290, 0.0f));
		if (ImGui::Button(" > "))  app->Play();
		if (ImGui::Button(" || ")) app->Pause();
		if (ImGui::Button(" [] "))  app->Stop();

		ImGui::EndMainMenuBar();
	}

	// Iterate Panels & Draw
	for (const auto& panel : panels)
	{
		if (panel->GetState())
			panel->Draw();
	}

	if (showImGuiDemo)
		ImGui::ShowDemoWindow();

	if (openSceneFileWindow)
		OpenSceneFileWindow();

	if (openColorPicker)
	{
		ImGui::OpenPopup("ColorPicker");
		openColorPicker = false;		
	}

	ColorPicker();

	if (overwritePopup)
	{
		ImGui::OpenPopup("Overwrite or Skip");
		overwritePopup = false;
	}

	OverwriteAsset(assetsDir);

    return ret;
}

bool Gui::PostUpdate()
{
	bool ret = true;

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)app->window->GetWidth(), (float)app->window->GetHeight());

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	//  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}

	SDL_GL_SwapWindow(app->window->window);

	return ret;
}

bool Gui::CleanUp()
{
	LOG(LogType::LOG_INFO, "Cleaning up IMGUI");
	bool ret = true;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	ImPlot::DestroyContext();

	for (const auto& panel : panels)
	{
		if (panel->GetState())
			panel->CleanUp();
	}

	return ret;
}


bool Gui::IsInitialized(Panel* panel)
{
	if (!panel)
	{
		LOG(LogType::LOG_ERROR, "-%s", panel->GetName().c_str());
		return false;
	}
	LOG(LogType::LOG_OK, "-%s", panel->GetName().c_str());
	return true;
}

std::list<Panel*> Gui::GetPanels()
{
	return panels;
}

void Gui::HandleInput(SDL_Event* event)
{
	ImGui_ImplSDL2_ProcessEvent(event);
}


// Utils ------------------------------------------------------

void Gui::OpenURL(const char* url) const
{
	ShellExecuteA(0, 0, url, 0, 0, SW_SHOW);
}

void Gui::PlotChart(const char* label, const std::vector<int>& data, ImPlotFlags plotFlags, ImPlotAxisFlags axisFlags)
{
	if (ImPlot::BeginPlot(label, nullptr, nullptr, ImVec2(-1, -1), plotFlags, axisFlags))
	{
		ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
		ImPlot::SetupAxesLimits(0, data.size(), 0, 250, ImGuiCond_Once);
		ImPlot::PlotShaded("FPS Area", data.data(), data.size());
		ImPlot::PlotLine(label, data.data(), data.size());
		ImPlot::PopStyleVar();
		ImPlot::EndPlot();
	}
}

void Gui::AssetContainer(const char* label)
{
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
	ImGui::InputText("##label", (char*)label, 64, ImGuiInputTextFlags_ReadOnly);
	ImGui::PopStyleVar();
}


// Main Dock Space ----------------------------------------------

void Gui::MainWindowDockspace()
{
	// Flags
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	// When using ImGuiDockNodeFlags_PassthruCentralNode,
	// DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Resize
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	// Style
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	static bool p_open = true;

	ImGui::Begin("DockSpace", &p_open, window_flags);
	
	ImGui::PopStyleVar(3);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowMinSize = ImVec2(150, 150);

	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("ToeDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	ImGui::End();
}

// Menu Bar ------
bool Gui::MainMenuFile()
{
	bool ret = true;

	if (ImGui::MenuItem("New", "Ctrl+N", false, false))
	{
		//app->scenemanager->N_sceneManager->CreateNewScene(1, "NewScene");
	}
	if (ImGui::MenuItem("Open", "Ctrl+O", false))
	{
		openSceneFileWindow = true;
	}

	ImGui::Separator();

	if (ImGui::MenuItem("Save", "Ctrl+S", false))
	{
		engine->N_sceneManager->SaveScene("Assets\\Scenes", engine->N_sceneManager->currentScene->GetSceneName());
	}
	if (ImGui::MenuItem("Save As..", 0, false, false)) {}

	ImGui::Separator();

	if (ImGui::MenuItem("Build", 0, false)) 
	{
		app->gui->panelBuild->SetState(true);
	}

	ImGui::Separator();

	if (ImGui::MenuItem("Exit"))
		ret = false;

	return ret;
}

void Gui::MainMenuEdit()
{
	if (ImGui::MenuItem("Undo", "Ctrl+Z", false, false)) {}
	if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false)) {}

	ImGui::Separator();

	if (ImGui::MenuItem("Cut", "Ctrl+X", false, false)) {}
	if (ImGui::MenuItem("Copy", "Ctrl+C", false, false)) {}
	if (ImGui::MenuItem("Paste", "Ctrl+V", false, false)) {}
}

void Gui::MainMenuAssets()
{
	if (ImGui::BeginMenu("Create"))
	{
		if (ImGui::MenuItem("Folder", 0, false, false)) {}

		ImGui::Separator();

		if (ImGui::MenuItem("Script", 0, false, false)) {}
		if (ImGui::MenuItem("Shader", 0, false, false)) {}

		ImGui::EndMenu();
	}
}

void Gui::MainMenuGameObject()
{
	if (ImGui::MenuItem("Create Empty", "Ctrl+Shift+N")) { engine->N_sceneManager->CreateEmptyGO(); }

	if (ImGui::BeginMenu("3D Object", "Ctrl+Shift+N"))
	{
		if (ImGui::MenuItem("Cube")) { engine->N_sceneManager->CreateMeshGO("Assets/Meshes/SM_Cube.fbx"); }
		if (ImGui::MenuItem("Sphere", 0, false, false)) {}
		if (ImGui::MenuItem("Less than 12?")) { engine->N_sceneManager->CreateMF(); }
		if (ImGui::MenuItem("Box Low Poly")) { engine->N_sceneManager->CreateMeshGO("Assets/Meshes/SM_Box_LowPoly.fbx"); }

		if (ImGui::BeginMenu("Characters", "Ctrl+Shift+N"))
		{
			if (ImGui::MenuItem("Main Character")) { engine->N_sceneManager->CreateMeshGO("Assets/Meshes/SK_MainCharacter.fbx"); }
			if (ImGui::MenuItem("FaceHugger (Arnau)")) { engine->N_sceneManager->CreateMeshGO("Assets/Meshes/SK_Facehugger.fbx"); }
			if (ImGui::MenuItem("Queen")) { engine->N_sceneManager->CreateMeshGO("Assets/Meshes/SK_Queen.fbx"); }
			if (ImGui::MenuItem("Anarchist")) { engine->N_sceneManager->CreateMeshGO("Assets/Meshes/SK_Anarchist.fbx"); }
			if (ImGui::MenuItem("ChestbuRster")) { engine->N_sceneManager->CreateMeshGO("Assets/Meshes/SM_Chestburster.fbx"); }
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}

	if (ImGui::MenuItem("Camera")) { engine->N_sceneManager->CreateCameraGO("newCamera"); }

	//Alex: this is just for debug
	if (ImGui::MenuItem("Canvas")) { engine->N_sceneManager->CreateCanvasGO("newCanvas"); }

	if (ImGui::BeginMenu("Light"))
	{
		if (ImGui::MenuItem("Directional Light")) { engine->N_sceneManager->CreateLightGO(LightType::Directional); }
		if (ImGui::MenuItem("Point Light")) { engine->N_sceneManager->CreateLightGO(LightType::Point); }
		if (ImGui::MenuItem("Spot Light")) { engine->N_sceneManager->CreateLightGO(LightType::Spot); }
		if (ImGui::MenuItem("Area Light", 0, false, false)) {}
		ImGui::EndMenu();
	}
}

void Gui::MainMenuComponent()
{
	if (ImGui::MenuItem("Mesh Renderer", 0, false, false)) {}
}

void Gui::MainMenuWindow()
{
	for (auto panel : app->gui->GetPanels())
	{
		if (panel->GetType() == PanelType::ABOUT || panel->GetType() == PanelType::BUILD)
			continue;

		if (ImGui::MenuItem(panel->GetName().c_str(), NULL, panel->GetState()))
			panel->SwitchState();
	}
}

void Gui::MainMenuHelp()
{
	if (ImGui::MenuItem("About TheOneEngine"))
	{
		app->gui->panelAbout->SwitchState();
	}

	ImGui::Separator();

	if (ImGui::MenuItem("Documentation"))
	{
		OpenURL("https://github.com/Shadow-Wizard-Games/TheOneEngine");
	}

	if (ImGui::MenuItem("ImGui Demo"))
	{
		showImGuiDemo = !showImGuiDemo;
	}
}

// Popups
void Gui::OpenSceneFileWindow()
{
	if (ImGui::Begin("Open File", &openSceneFileWindow))
	{
		static char nameSceneBuffer[50];

		ImGui::InputText("File Name", nameSceneBuffer, IM_ARRAYSIZE(nameSceneBuffer));

		/*std::string filename = "Assets/Scenes/Scene 1.toe";
		app->scenemanager->N_sceneManager->LoadScene(filename);*/
		std::string nameScene = nameSceneBuffer;
		std::string file = "Assets/Scenes/" + nameScene + ".toe";

		if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && nameSceneBuffer != "")
		{
			if (engine->N_sceneManager->currentScene->IsDirty())
			{
				ImGui::OpenPopup("SaveBeforeLoad");
			}
			else
			{
				engine->N_sceneManager->LoadScene(nameSceneBuffer);
				openSceneFileWindow = false;
			}
		}

		if (ImGui::BeginPopup("SaveBeforeLoad"))
		{
			ImGui::Text("You have unsaved changes in this scene. Are you sure?");

			if (ImGui::Button("Yes", { 100, 20 })) {
				engine->N_sceneManager->LoadScene(nameSceneBuffer);
				openSceneFileWindow = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("No", { 100, 20 })) {
				openSceneFileWindow = false;
			}

			ImGui::EndPopup();
		}

		ImGui::End();
	}
}

void Gui::ColorPicker()
{
	// Generate a default palette. The palette will persist and can be edited.
	static bool saved_palette_init = true;
	static ImVec4 saved_palette[32] = {};
	if (saved_palette_init)
	{
		for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
		{
			ImGui::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f,
				saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
			saved_palette[n].w = 1.0f; // Alpha
		}
		saved_palette_init = false;
	}

	if (ImGui::BeginPopup("ColorPicker"))
	{
		ImGui::Text("Color");
		ImGui::Separator(); ImGui::Dummy({ 0, 4 });

		// Previous/New Color
		if (ImGui::ColorButton("##previous", editColor.previousColor, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreview, ImVec2(50, 20)))
			editColor.color = editColor.previousColor;
		ImGui::SameLine();
		ImGui::ColorButton("##current", editColor.color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreview, ImVec2(50, 20));
		ImGui::Dummy({ 0, 4 });

		// Color Picker + Formats
		ImGui::ColorPicker4("##picker", (float*)&editColor.color, panelSettings->GetColorFlags() | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaBar);

		// Palette
		ImGui::Dummy({ 0, 4 }); ImGui::Separator(); ImGui::Dummy({ 0, 4 });
		ImGui::Text("Palette");
		for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
		{
			ImGui::PushID(n);
			if ((n % 8) != 0)
				ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);

			ImGuiColorEditFlags palette_button_flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip;
			if (ImGui::ColorButton("##palette", saved_palette[n], palette_button_flags, ImVec2(20, 20)))
				editColor.color = ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, editColor.color.w); // Preserve alpha!

			// Allow user to drop colors into each palette entry. Note that ColorButton() is already a
			// drag source by default, unless specifying the ImGuiColorEditFlags_NoDragDrop flag.
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
					memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 3);
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
					memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 4);
				ImGui::EndDragDropTarget();
			}

			ImGui::PopID();
		}

		ImGui::EndPopup();
	}
	else
	{
		editColor.id = "null";
	}
}

void Gui::OverwriteAsset(std::string path)
{
	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, { 0.5, 0.5 });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 10, 10 });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5);

	if (ImGui::BeginPopupModal("Overwrite or Skip", nullptr, ImGuiWindowFlags_NoResize))
	{
		ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit;
		if (ImGui::BeginTable("##TableWarning", 2, tableFlags))
		{
			ImGui::TableSetupColumn("Image", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("Dialog", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoHide);
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Image((ImTextureID)app->gui->panelProject->GetIconTextures()[FileType::SCENE], { 64, 64 });
			ImGui::SameLine();
			ImGui::Dummy({ 8, 0 });

			ImGui::TableSetColumnIndex(1);
			ImGui::TextWrapped("Assets already has a file named:");

			ImGui::TextWrapped(path.c_str());
			ImGui::Dummy({ 0, 8 });
			ImGui::TextWrapped("Are you sure you want to overwrite it?");
			ImGui::Dummy({ 0, 8 });

			if (ImGui::Button("Overwrite", { 90, 20 }))
			{
				engine->N_sceneManager->CreateMeshGO(assetsDir);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Skip", { 90, 20 }))
			{
				engine->N_sceneManager->CreateExistingMeshGO(assetsDir);
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndTable();
		}
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar(2);
}