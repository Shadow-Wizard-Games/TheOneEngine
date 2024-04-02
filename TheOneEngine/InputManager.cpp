#include "InputManager.h"
#include "Log.h"
#include <SDL2/SDL.h>
#include <string.h>

InputManager::InputManager()
{
	for (int i = 0; i < MAX_KEYS; ++i) keys[i] = InputManagerNamespace::KEY_IDLE;

	memset(&pads[0], 0, sizeof(GamePad) * MAX_PADS);
}

InputManager::~InputManager()
{

}

bool InputManager::Init()
{
	LOG(LogType::LOG_OK, "Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG(LogType::LOG_ERROR, "SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0)
	{
		LOG(LogType::LOG_ERROR, "SDL_INIT_GAMECONTROLLER could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool InputManager::PreUpdate()
{
	bool ret = true;

	SDL_PumpEvents();

	// Read all keyboard data and update our custom array
	const Uint8* keyboard = SDL_GetKeyboardState(NULL);
	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keyboard[i])
			keys[i] = (keys[i] == InputManagerNamespace::KeyState::KEY_IDLE) ? InputManagerNamespace::KeyState::KEY_DOWN : InputManagerNamespace::KeyState::KEY_REPEAT;
		else
			keys[i] = (keys[i] == InputManagerNamespace::KeyState::KEY_REPEAT || keys[i] == InputManagerNamespace::KeyState::KEY_DOWN) ? InputManagerNamespace::KeyState::KEY_UP : InputManagerNamespace::KeyState::KEY_IDLE;
	}

	// Read new SDL events
	static SDL_Event event;
	while (SDL_PollEvent(&event) != 0)
	{
		switch (event.type)
		{
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
		case(SDL_QUIT):
		{
			ret = false;
			break;
		}
		}
	}

	UpdateGamepadsInput();

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

void InputManager::UpdateGamepadsInput()
{
	// Iterate through all active gamepads and update all input data
	for (int i = 0; i < MAX_PADS; ++i)
	{
		GamePad& pad = pads[i];

		if (pad.enabled == true)
		{
			pad.a = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_A) == 1;
			pad.b = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_B) == 1;
			pad.x = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_X) == 1;
			pad.y = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_Y) == 1;
			pad.l1 = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER) == 1;
			pad.r1 = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == 1;
			pad.l3 = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_LEFTSTICK) == 1;
			pad.r3 = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_RIGHTSTICK) == 1;
			pad.up = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_DPAD_UP) == 1;
			pad.down = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN) == 1;
			pad.left = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT) == 1;
			pad.right = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == 1;

			pad.start = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_START) == 1;
			pad.guide = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_GUIDE) == 1;
			pad.back = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_BACK) == 1;

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

InputManagerNamespace::KeyState InputManager::GetGamepadButton(int gamepadId, SDL_GameControllerButton button) const
{
	if (gamepadId >= 0 && gamepadId < MAX_PADS)
	{
		if (pads[gamepadId].controller)
		{
			if (SDL_GameControllerGetButton(pads[gamepadId].controller, button) == 1)
			{
				return InputManagerNamespace::KEY_DOWN;
			}
			else
			{
				return InputManagerNamespace::KEY_UP;
			}
		}
	}

	return InputManagerNamespace::KEY_IDLE;
}