#include "App.h"
#include "Input.h"
#include "Gui.h"
#include "Window.h"
#include "Renderer3D.h"
#include "SceneManager.h"

#include "../TheOneEngine/Log.h"
#include "../TheOneEngine/InputManager.h"
#include <string>
#include <iostream>
#include <filesystem>

#define MAX_KEYS 300
#define SCREEN_SIZE 1

namespace fs = std::filesystem;

Input::Input(App* app) : Module(app)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
}

Input::~Input()
{
	delete[] keyboard;
}

bool Input::Awake()
{
	bool ret = true;
	LOG(LogType::LOG_INFO, "# Initializing Input Events...");

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) == 0)
	{
		LOG(LogType::LOG_OK, "-Init SDL Input Event subsystem");
	}
	else
	{
		LOG(LogType::LOG_ERROR, "-SDL Input Event subsystem could not be initialized! %s", SDL_GetError());
		ret = false;
	}

	LOG(LogType::LOG_OK, "-File System current path: %s", std::filesystem::current_path().string().c_str());

	inputManagerInstance = engine->inputManager;

	return ret;
}

bool Input::PreUpdate()
{
	bool ret = true;

	ret = processSDLEvents();

#pragma region GamepadInput_debugging

	if (inputManagerInstance->GetGamepadButton(0, SDL_CONTROLLER_BUTTON_X) == InputManagerNamespace::KEY_DOWN)
	{
		LOG(LogType::LOG_OK, "BUTTON X FUNCTIONAL");

		//for (int button = 0; button < SDL_CONTROLLER_BUTTON_MAX; ++button)
		//{
		//	LOG(LogType::LOG_OK, "Button %d: %s", button,
		//		SDL_GameControllerGetButton(inputManagerInstance.pads[0].controller,
		//			static_cast<SDL_GameControllerButton>(button)) == 1 ? "pressed" : "relesed");
		//}
	}
	if (inputManagerInstance->GetGamepadButton(0, SDL_CONTROLLER_BUTTON_A) == InputManagerNamespace::KEY_DOWN)
	{
		LOG(LogType::LOG_OK, "BUTTON A FUNCTIONAL");
	}
	if (inputManagerInstance->GetGamepadButton(0, SDL_CONTROLLER_BUTTON_B) == InputManagerNamespace::KEY_DOWN)
	{
		LOG(LogType::LOG_OK, "BUTTON B FUNCTIONAL");
	}
	if (inputManagerInstance->GetGamepadButton(0, SDL_CONTROLLER_BUTTON_Y) == InputManagerNamespace::KEY_DOWN)
	{
		LOG(LogType::LOG_OK, "BUTTON Y FUNCTIONAL");
	}
	if (inputManagerInstance->GetGamepadButton(0, SDL_CONTROLLER_BUTTON_DPAD_UP) == InputManagerNamespace::KEY_DOWN)
	{
		LOG(LogType::LOG_OK, "BUTTON DPAD_UP FUNCTIONAL");
	}
	if (inputManagerInstance->GetGamepadButton(0, SDL_CONTROLLER_BUTTON_DPAD_DOWN) == InputManagerNamespace::KEY_DOWN)
	{
		LOG(LogType::LOG_OK, "BUTTON DPAD_DOWN FUNCTIONAL");
	}
	if (inputManagerInstance->GetGamepadButton(0, SDL_CONTROLLER_BUTTON_DPAD_LEFT) == InputManagerNamespace::KEY_DOWN)
	{
		LOG(LogType::LOG_OK, "BUTTON DPAD_LEFT FUNCTIONAL");
	}
	if (inputManagerInstance->GetGamepadButton(0, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == InputManagerNamespace::KEY_DOWN)
	{
		LOG(LogType::LOG_OK, "BUTTON DPAD_RIGHT FUNCTIONAL");
	}
#pragma endregion

	return ret;
}

bool Input::Update(double dt)
{
	bool ret = true;

	return ret;
}

bool Input::processSDLEvents()
{
	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keys[i] == 1)
		{
			if (keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	mouse_z = 0;

	for (int i = 0; i < 5; ++i)
	{
		if (buttons & SDL_BUTTON(i))
		{
			if (mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if (mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	static SDL_Event event;
	while (SDL_PollEvent(&event) != 0)
	{
		app->gui->HandleInput(&event);
		switch (event.type)
		{
		case SDL_MOUSEWHEEL:
			mouse_z = event.wheel.y;
			break;

		case SDL_MOUSEMOTION:
			mouse_x = event.motion.x / SCREEN_SIZE;
			mouse_y = event.motion.y / SCREEN_SIZE;

			mouse_x_motion = event.motion.xrel / SCREEN_SIZE;
			mouse_y_motion = event.motion.yrel / SCREEN_SIZE;
			break;

		case SDL_QUIT: return false;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			//case SDLK_ESCAPE: //return false;
			}
			break;

		case SDL_CONTROLLERDEVICEADDED:
			inputManagerInstance->HandleDeviceConnection(event.cdevice.which);
			break;

		case SDL_CONTROLLERDEVICEREMOVED:
			inputManagerInstance->HandleDeviceRemoval(event.cdevice.which);
			break;

		case SDL_WINDOWEVENT:
		{
			if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
				app->window->OnResizeWindow(event.window.data1, event.window.data2);
			}
			if (event.window.event == SDL_WINDOWEVENT_CLOSE)
				return false;
			break;
		}
		case SDL_DROPFILE:
		{
			// Just flag event here
			// This code elsewhere
			std::string fileDir = event.drop.file;
			std::string fileNameExt = fileDir.substr(fileDir.find_last_of('\\') + 1);

			std::string fbxName = fileDir.substr(fileDir.find_last_of("\\/") + 1, fileDir.find_last_of('.') - fileDir.find_last_of("\\/") - 1);

			// FBX
			if (fileDir.ends_with(".fbx") || fileDir.ends_with(".FBX"))
			{
				fs::path assetsDir = fs::path(ASSETS_PATH) / "Meshes" / fileNameExt;

				LOG(LogType::LOG_ASSIMP, "Importing %s from: %s", fileNameExt.data(), fileDir.data());

				// Check if it already exists in Library
				if (std::filesystem::exists(assetsDir))
				{
					LOG(LogType::LOG_WARNING, "-%s already exists in %s", fileNameExt.data(), assetsDir.string().data());

					//Find Meshes in Library
					engine->N_sceneManager->CreateExistingMeshGO(assetsDir.string());
				}
				else
				{
					LOG(LogType::LOG_OK, "-%s Imported successfully into: %s", fileNameExt.data(), assetsDir.string().data());
					std::filesystem::copy(fileDir, assetsDir, std::filesystem::copy_options::overwrite_existing);

					//Creates GO and Serialize
					engine->N_sceneManager->CreateMeshGO(assetsDir.string());
					LOG(LogType::LOG_OK, "-Created GameObject: %s", assetsDir.string().data());
				}
			}

			// PNG / DDS
			else if (fileDir.ends_with(".png") || fileDir.ends_with(".dds"))
			{
				std::filesystem::copy(fileDir, "Assets", std::filesystem::copy_options::overwrite_existing);
			}
			SDL_free(event.drop.file);
			break;
		}
		}
	}
	return true;
}