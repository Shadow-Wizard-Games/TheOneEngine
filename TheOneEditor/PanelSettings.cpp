#include "PanelSettings.h"
#include "App.h"
#include "Gui.h"
#include "Window.h"
#include "Input.h"
#include "Hardware.h"

#include "imgui.h"
#include "implot.h"
#include "implot_internal.h"


PanelSettings::PanelSettings(PanelType type, std::string name) : Panel(type, name), fpsHistory(MAX_HISTORY_SIZE), delayHistory(MAX_HISTORY_SIZE) {}

PanelSettings::~PanelSettings() {}


void PanelSettings::AddFpsValue(int fps)
{
	fpsHistory.push_back(fps);

	if (fpsHistory.size() > MAX_HISTORY_SIZE)
	{
		fpsHistory.erase(fpsHistory.begin());
	}
}


bool PanelSettings::Draw()
{
	ImGuiWindowFlags settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	ImVec2 mainViewportPos = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(ImVec2(mainViewportPos.x, mainViewportPos.y), ImGuiCond_Appearing, ImVec2(0.5, 0.8));

	if (ImGui::Begin("Settings", &enabled, settingsFlags))
	{
		// LEFT - Buttons
		ImVec2 leftChildSize = ImVec2(ImGui::GetWindowSize().x * 0.2, ImGui::GetWindowSize().y);
		ImVec2 buttonSize = ImVec2(ImGui::GetWindowSize().x * 0.2, 20);
	
		if (ImGui::BeginChild("", leftChildSize, true))
		{
			if (ImGui::Button("Performance", buttonSize))
				selected = SelectedSetting::PERFORMANCE;

			if (ImGui::Button("Window", buttonSize))
				selected = SelectedSetting::WINDOW;

			if (ImGui::Button("Input", buttonSize))
				selected = SelectedSetting::INPUT;

			if (ImGui::Button("Renderer", buttonSize))
				selected = SelectedSetting::RENDERER;

			if (ImGui::Button("Hardware", buttonSize))
				selected = SelectedSetting::HARDWARE;

			if (ImGui::Button("Software", buttonSize))
				selected = SelectedSetting::SOFTWARE;

			ImGui::EndChild();
		}

		ImGui::SameLine();

		// RIGHT - Display selected
		settingsFlags &= ~(ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		if (ImGui::BeginChild("Selected", ImVec2(ImGui::GetWindowSize().x * 0.8f, ImGui::GetWindowSize().y), false, settingsFlags))
		{
			switch (selected)
			{
				case SelectedSetting::PERFORMANCE:	Performance();	break;
				case SelectedSetting::WINDOW:		Window();		break;
				case SelectedSetting::INPUT:		Input();		break;
				case SelectedSetting::RENDERER:		Renderer();		break;
				case SelectedSetting::HARDWARE:		Hardware();		break;
				case SelectedSetting::SOFTWARE:		Software();		break;
				default: Performance(); break;
			}

			ImGui::EndChild();
		}

		ImGui::End();
	}

	ImGui::PopStyleVar();

	return true;
}


// Settings -----------------------------------------
void PanelSettings::Performance()
{
	// FPS cap
	int frameRate = app->GetFrameRate();
	ImGui::Text("Frame Rate");
	ImGui::SameLine();
	if (ImGui::SliderInt("", &frameRate, 0, app->window->GetDisplayRefreshRate()))
		app->SetFrameRate(frameRate);

	ImGui::Separator();

	ImPlotFlags plotFlags = ImPlotFlags_NoInputs | ImPlotFlags_NoFrame | ImPlotFlags_NoLegend;
	ImPlotAxisFlags axisFlags = ImPlotAxisFlags_NoTickMarks;
	app->gui->PlotChart("FPS", fpsHistory, plotFlags, axisFlags);
}

void PanelSettings::Window()
{
	// Display Mode
	int displayMode = (int)app->window->GetDisplayMode();	
	ImGui::Text("Display Mode");
	ImGui::SameLine();
	if (ImGui::Combo("##Display Mode", &displayMode, displayModes, 4))
		app->window->SetDisplayMode((DisplayMode)displayMode);

	// Resolution
	int resolution = (int)app->window->GetResolution();
	ImGui::Text("Resolution");
	ImGui::SameLine();
	if (ImGui::Combo("##Resolution", &resolution, resolutions, 8))
		app->window->SetResolution((Resolution)resolution);

	// Vsync
	bool vsync = engine->GetVSync();
	ImGui::Text("V-Sync");
	ImGui::SameLine();
	if (ImGui::Checkbox("##V-Sync", &vsync))
		engine->SetVSync(vsync);

	ImGui::SameLine();

	// Fps
	int maxFPS = (int)app->GetFrameRate();
	ImGui::Text("Maximum FPS");
	ImGui::SameLine();
	if (ImGui::Combo("##Maximum FPS", &maxFPS, fpsList, 8))
		app->SetFrameRate(atoi(fpsList[maxFPS]));
}

void PanelSettings::Input()
{
	ImGuiIO& io = ImGui::GetIO();
	ImVec4 grey = ImVec4(0.5, 0.5, 0.5, 1);

	// Display inputs submitted to ImGuiIO
	
	// Mouse Position
	ImGui::Text("Mouse pos:");
	ImGui::SameLine();
	if (ImGui::IsMousePosValid())
		ImGui::TextColored(grey, "%g, %g", io.MousePos.x, io.MousePos.y);
	else
		ImGui::TextColored(grey, "<INVALID>");

	// Mouse Movement
	ImGui::Text("Mouse delta:");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%g, %g", io.MouseDelta.x, io.MouseDelta.y);

	// Mouse Down
	ImGui::Text("Mouse down:");
	for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
		if (ImGui::IsMouseDown(i))
		{ 
			ImGui::SameLine();
			ImGui::TextColored(grey, "b%d (%.02f secs)", i, io.MouseDownDuration[i]);
		}

	//Mouse Wheel
	ImGui::Text("Mouse wheel:");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%.1f", io.MouseWheel);

	// Keys Down
	struct funcs
	{
		static bool IsLegacyNativeDupe(ImGuiKey key)
		{ 
			return key < 512 && ImGui::GetIO().KeyMap[key] != -1;
		}
	}; // Hide Native<>ImGuiKey duplicates when both exists in the array
	ImGuiKey start_key = (ImGuiKey)0;

	ImGui::Text("Keys down:");
	for (ImGuiKey key = start_key; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1))
	{
		if (funcs::IsLegacyNativeDupe(key) || !ImGui::IsKeyDown(key)) continue;
		ImGui::SameLine();
		ImGui::TextColored(grey, (key < ImGuiKey_NamedKey_BEGIN) ? "\"%s\"" : "\"%s\" %d", ImGui::GetKeyName(key), key);
	}

	// Key Mods
	ImGui::Text("Keys mods:");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%s%s%s%s", io.KeyCtrl ? "CTRL " : "", io.KeyShift ? "SHIFT " : "", io.KeyAlt ? "ALT " : "", io.KeySuper ? "SUPER " : "");

	// Chars
	ImGui::Text("Chars queue:");
	for (int i = 0; i < io.InputQueueCharacters.Size; i++)
	{
		ImWchar c = io.InputQueueCharacters[i];
		ImGui::SameLine();
		ImGui::TextColored(grey, "\'%c\' (0x%04X)", (c > ' ' && c <= 255) ? (char)c : '?', c);
	} // FIXME: We should convert 'c' to UTF-8 here but the functions are not public.
}

void PanelSettings::Renderer()
{
}

void PanelSettings::Hardware()
{
	HardwareInfo hardware_info = app->hardware->GetInfo();
	ImVec4 isSupported;
	ImVec4 grey = ImVec4(0.5, 0.5, 0.5, 1);
	ImVec4 green = ImVec4(0, 0.8, 0, 1);
	ImVec4 red = ImVec4(0.8, 0, 0, 1);


	// CPU 
	ImGui::Text("CPU");
	ImGui::Text(" Logic Cores:");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%i", hardware_info.cpu_count);

	ImGui::Text(" L1 Cache (Kb):");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%i", hardware_info.l1_cachekb);

	ImGui::Text(" Instruction Support:");
	ImGui::SameLine();
	isSupported = hardware_info.rdtsc == true ? green : red;
	ImGui::TextColored(isSupported, "%s", "rdtsc");
	ImGui::SameLine();
	isSupported = hardware_info.altivec == true ? green : red;
    ImGui::TextColored(isSupported, "%s", "altivec");
	ImGui::SameLine();
	isSupported = hardware_info.now3d == true ? green : red;
	ImGui::TextColored(isSupported, "%s", "now3d");
	ImGui::SameLine();
	isSupported = hardware_info.mmx == true ? green : red;
	ImGui::TextColored(isSupported, "%s", "mmx");

	isSupported = hardware_info.sse == true ? green : red;
	ImGui::TextColored(isSupported, "\t\t\t\t\t  %s", "sse");
	ImGui::SameLine();
	isSupported = hardware_info.sse2 == true ? green : red;
	ImGui::TextColored(isSupported, "%s", "sse2");
	ImGui::SameLine();
	isSupported = hardware_info.sse3 == true ? green : red;
	ImGui::TextColored(isSupported, "%s", "sse3");
	ImGui::SameLine();
	isSupported = hardware_info.sse41 == true ? green : red;
	ImGui::TextColored(isSupported, "%s", "sse41");
	ImGui::SameLine();
	isSupported = hardware_info.sse42 == true ? green : red;
	ImGui::TextColored(isSupported, "%s", "sse42");

	isSupported = hardware_info.avx == true ? green : red;
	ImGui::TextColored(isSupported, "\t\t\t\t\t  %s", "avx");
	ImGui::SameLine();
	isSupported = hardware_info.avx2 == true ? green : red;
	ImGui::TextColored(isSupported, "%s", "avx2");


	// RAM 
	ImGui::Separator();
	ImGui::Text("RAM");
	ImGui::Text("  Memory");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%.1f GB", hardware_info.ram_gb);


	// GPU (Currently NVIDIA only)
	ImGui::Separator();
	ImGui::Text("GPU");
	ImGui::Text(" Vendor");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%s", hardware_info.gpu_vendor.data());

	ImGui::Text(" Model");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%s", hardware_info.gpu_brand.data());

	ImGui::Text(" Driver");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%s", hardware_info.gpu_driver.data());

	ImGui::Text(" VRAM Budget");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%.1f MB", hardware_info.vram_mb_budget);

	ImGui::Text(" VRAM Usage");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%.1f MB", hardware_info.vram_mb_usage);

	ImGui::Text(" VRAM Available");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%.1f MB", hardware_info.vram_mb_available);
}

void PanelSettings::Software()
{
	HardwareInfo hardware_info = app->hardware->GetInfo();
	ImVec4 grey = ImVec4(0.5, 0.5, 0.5, 1);

	// SDL Version
	ImGui::Text("SDL ");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%s", hardware_info.sdl_version);

	ImGui::Text("OpenGL ");
	ImGui::SameLine();
	ImGui::TextColored(grey, "2022-12-04#3");
	
	ImGui::Text("Glew ");
	ImGui::SameLine();
	ImGui::TextColored(grey, "2.2.0#3");

	ImGui::Text("ImGui ");
	ImGui::SameLine();
	ImGui::TextColored(grey, "1.89.9");

	ImGui::Text("DevIL ");
	ImGui::SameLine();
	ImGui::TextColored(grey, "1.8.0#11");

	ImGui::Text("Assimp ");
	ImGui::SameLine();
	ImGui::TextColored(grey, "5.2.5");
}