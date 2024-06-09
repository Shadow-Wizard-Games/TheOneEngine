#include "InputManager.h"
#include "Defs.h"
#include "Log.h"

#include <SDL2/SDL.h>
#include <string.h>

InputManager::InputManager()
{
	for (int i = 0; i < MAX_KEYS; ++i) keyboard[i] = KEY_IDLE;

	memset(keyboard, KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KeyState) * MAX_MOUSE_BUTTONS);
	memset(&pads[0], 0, sizeof(GamePad) * MAX_PADS);

	for (int i = 0; i < MAX_PADS; ++i)
	{
		for (int j = 0; j < SDL_CONTROLLER_BUTTON_MAX; ++j)
		{
			pads[i].buttonStates[j] = KEY_IDLE;
			pads[i].previousButtonStates[j] = KEY_IDLE;
		}
	}
}

InputManager::~InputManager() {}

bool InputManager::Init()
{
	bool ret = true;
	LOG(LogType::LOG_INFO, "# Initializing Input Events...");
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) == 0)
	{
		LOG(LogType::LOG_OK, "-Init SDL Input Event subsystem");
	}
	else
	{
		LOG(LogType::LOG_ERROR, "-SDL Input Event subsystem could not be initialized! %s", SDL_GetError());
		ret = false;
	}

	if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0)
	{
		LOG(LogType::LOG_ERROR, "SDL_INIT_GAMECONTROLLER could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	LOG(LogType::LOG_OK, "-File System current path: %s", std::filesystem::current_path().string().c_str());

	return ret;
}

bool InputManager::PreUpdate()
{
	bool ret = true;

	ret = ProcessSDLEvents();

	UpdateGamepadsInput();

	Debug();

	if (shutDownEngine) ret = false;

	return ret;
}

bool InputManager::CleanUp()
{
	LOG(LogType::LOG_ERROR, "Quitting SDL input event subsystem");

	SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
	SDL_QuitSubSystem(SDL_INIT_EVENTS);

	return true;
}

bool InputManager::ProcessSDLEvents()
{
	bool ret = true;

	SDL_PumpEvents();

	// Read all keyboard data and update our custom array
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keys[i])
			keyboard[i] = (keyboard[i] == KeyState::KEY_IDLE) ?
				KeyState::KEY_DOWN : KeyState::KEY_REPEAT;
		else
			keyboard[i] = (keyboard[i] == KeyState::KEY_REPEAT || keyboard[i] == KeyState::KEY_DOWN) ?
				KeyState::KEY_UP : KeyState::KEY_IDLE;
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	mouse_z = 0;

	for (int i = 0; i < 5; ++i)
	{
		if (buttons & SDL_BUTTON(i))
		{
			mouse_buttons[i] = (mouse_buttons[i] == KeyState::KEY_IDLE) ?
				KeyState::KEY_DOWN : KeyState::KEY_REPEAT;
		}
		else
		{
			mouse_buttons[i] = (mouse_buttons[i] == KeyState::KEY_REPEAT || mouse_buttons[i] == KeyState::KEY_DOWN) ?
				KeyState::KEY_UP : KeyState::KEY_IDLE;
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	// Read new SDL events
	static SDL_Event event;
	events.clear();
	windowResize = false;
	dropFile = false;

	while (SDL_PollEvent(&event) != 0)
	{
		events.push_back(event);

		switch (event.type)
		{
			case SDL_WINDOWEVENT:
			{
				if (event.window.event == SDL_WINDOWEVENT_CLOSE)
					return false;

				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					windowResize = true;
					windowSize.x = event.window.data1;
					windowSize.y = event.window.data2;
				}
				break;
			}
			case SDL_MOUSEWHEEL:
			{
				mouse_z = event.wheel.y;
				break;
			}
			case SDL_MOUSEMOTION:
			{
				mouse_x = event.motion.x / SCREEN_SIZE;
				mouse_y = event.motion.y / SCREEN_SIZE;
				mouse_x_motion = event.motion.xrel / SCREEN_SIZE;
				mouse_y_motion = event.motion.yrel / SCREEN_SIZE;
				break;
			}
			case SDL_KEYDOWN:
			{
				//switch (event.key.keysym.sym)
				//{
				///*case SDLK_ESCAPE: 
				//	return false;*/
				//}
				break;
			}
			case(SDL_CONTROLLERDEVICEADDED):
			{
				HandleDeviceConnection(event.cdevice.which);
				break;
			}
			case(SDL_CONTROLLERDEVICEREMOVED):
			{
				HandleDeviceRemoval(event.cdevice.which);
				break;
			}
			case SDL_DROPFILE:
			{
				dropFile = true;
				fileDir = event.drop.file;
				SDL_free(event.drop.file);
				break;
			}
			case(SDL_QUIT):
			{
				ret = false;
				break;
			}
		}
	}

	return true;
}

void InputManager::UpdateGamepadsInput()
{
	for (int i = 0; i < MAX_PADS; ++i)
	{
		GamePad& pad = pads[i];

		if (pad.enabled)
		{
			for (int j = 0; j < SDL_CONTROLLER_BUTTON_MAX; ++j)
			{
				pad.previousButtonStates[j] = pad.buttonStates[j];

				bool isPressed = SDL_GameControllerGetButton(pad.controller, (SDL_GameControllerButton)j) == 1;

				if (isPressed)
				{
					pad.buttonStates[j] = (pad.previousButtonStates[j] == KEY_IDLE || pad.previousButtonStates[j] == KEY_UP) ? KEY_DOWN : KEY_REPEAT;
				}
				else
				{
					pad.buttonStates[j] = (pad.previousButtonStates[j] == KEY_DOWN || pad.previousButtonStates[j] == KEY_REPEAT) ? KEY_UP : KEY_IDLE;
				}
			}

			pad.l2 = float(SDL_GameControllerGetAxis(pad.controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT)) / 32767.0f;
			pad.r2 = float(SDL_GameControllerGetAxis(pad.controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT)) / 32767.0f;

			pad.left_x = float(SDL_GameControllerGetAxis(pad.controller, SDL_CONTROLLER_AXIS_LEFTX)) / 32767.0f;
			pad.left_y = float(SDL_GameControllerGetAxis(pad.controller, SDL_CONTROLLER_AXIS_LEFTY)) / 32767.0f;
			pad.right_x = float(SDL_GameControllerGetAxis(pad.controller, SDL_CONTROLLER_AXIS_RIGHTX)) / 32767.0f;
			pad.right_y = float(SDL_GameControllerGetAxis(pad.controller, SDL_CONTROLLER_AXIS_RIGHTY)) / 32767.0f;

			// Apply deadzone. All values below the deadzone will be discarded
			pad.left_x = (fabsf(pad.left_x) > pad.left_dz) ? pad.left_x : 0.0f;
			pad.left_y = (fabsf(pad.left_y) > pad.left_dz) ? pad.left_y : 0.0f;
			pad.right_x = (fabsf(pad.right_x) > pad.right_dz) ? pad.right_x : 0.0f;
			pad.right_y = (fabsf(pad.right_y) > pad.right_dz) ? pad.right_y : 0.0f;
		}
	}
}

void InputManager::HandleDeviceConnection(int index)
{
	if (SDL_IsGameController(index))
	{
		LOG(LogType::LOG_OK, "Gamepad connected");
		for (int i = 0; i < MAX_PADS; ++i)
		{
			GamePad& pad = pads[i];

			if (pad.enabled == false)
			{
				if (pad.controller = SDL_GameControllerOpen(index))
				{
					pad.enabled = true;
					pad.left_dz = pad.right_dz = 0.1f;
					pad.index = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(pad.controller));
				}
			}
			pads[i] = pad;
		}
	}
}

void InputManager::HandleDeviceRemoval(int index)
{
	// If an existing gamepad has the given index, deactivate all SDL device functionallity
	LOG(LogType::LOG_ERROR, "Gamepad disconnected");
	for (int i = 0; i < MAX_PADS; ++i)
	{
		GamePad& pad = pads[i];
		if (pad.enabled && pad.index == index)
		{
			SDL_GameControllerClose(pad.controller);
			memset(&pad, 0, sizeof(GamePad));
		}
	}
}

void InputManager::Debug()
{
	if (GetGamepadButton(0, SDL_CONTROLLER_BUTTON_X) == KEY_DOWN)
	{
		LOG(LogType::LOG_OK, "BUTTON X FUNCTIONAL");

		//for (int button = 0; button < SDL_CONTROLLER_BUTTON_MAX; ++button)
		//{
		//	LOG(LogType::LOG_OK, "Button %d: %s", button,
		//		SDL_GameControllerGetButton(inputManagerInstance.pads[0].controller,
		//			static_cast<SDL_GameControllerButton>(button)) == 1 ? "pressed" : "relesed");
		//}
	}
	if (GetGamepadButton(0, SDL_CONTROLLER_BUTTON_A) == KEY_DOWN)
	{
		LOG(LogType::LOG_OK, "BUTTON A FUNCTIONAL");
	}
	if (GetGamepadButton(0, SDL_CONTROLLER_BUTTON_B) == KEY_DOWN)
	{
		LOG(LogType::LOG_OK, "BUTTON B FUNCTIONAL");
	}
	if (GetGamepadButton(0, SDL_CONTROLLER_BUTTON_Y) == KEY_DOWN)
	{
		LOG(LogType::LOG_OK, "BUTTON Y FUNCTIONAL");
	}
	if (GetGamepadButton(0, SDL_CONTROLLER_BUTTON_DPAD_UP) == KEY_DOWN)
	{
		LOG(LogType::LOG_OK, "BUTTON DPAD_UP FUNCTIONAL");
	}
	if (GetGamepadButton(0, SDL_CONTROLLER_BUTTON_DPAD_DOWN) == KEY_DOWN)
	{
		LOG(LogType::LOG_OK, "BUTTON DPAD_DOWN FUNCTIONAL");
	}
	if (GetGamepadButton(0, SDL_CONTROLLER_BUTTON_DPAD_LEFT) == KEY_DOWN)
	{
		LOG(LogType::LOG_OK, "BUTTON DPAD_LEFT FUNCTIONAL");
	}
	if (GetGamepadButton(0, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == KEY_DOWN)
	{
		LOG(LogType::LOG_OK, "BUTTON DPAD_RIGHT FUNCTIONAL");
	}
}

KeyState InputManager::GetGamepadButton(int gamepadId, int button) const
{
	if (gamepadId >= 0 && gamepadId < MAX_PADS)
	{
		if (pads[gamepadId].controller)
		{
			return pads[gamepadId].buttonStates[button];
		}
	}

	return KEY_IDLE;
}
