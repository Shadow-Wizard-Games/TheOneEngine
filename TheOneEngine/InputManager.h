#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "SDL2/SDL_scancode.h"
#include "SDL2/SDL_gamecontroller.h"

#define MAX_KEYS 256
#define MAX_PADS 4

struct _SDL_GameController;
struct _SDL_Haptic;

namespace InputManagerNamespace {
	enum KeyState
	{
		KEY_IDLE,
		KEY_DOWN,
		KEY_REPEAT,
		KEY_UP
	};
}

struct GamePad
{
	bool start, back, guide;
	bool x, y, a, b, l1, r1, l3, r3;
	bool up, down, left, right;
	float l2, r2;
	float left_x, left_y, right_x, right_y, left_dz, right_dz;

	bool enabled;
	int index;
	_SDL_GameController* controller;
};

class InputManager
{
public:
	InputManager();
	~InputManager();

	bool Init();
	bool PreUpdate();
	bool CleanUp();

	void HandleDeviceConnection(int index);
	void HandleDeviceRemoval(int index);

	// Called at PreUpdate
	void UpdateGamepadsInput();

public:
	InputManagerNamespace::KeyState keys[MAX_KEYS] = { InputManagerNamespace::KEY_IDLE };
	InputManagerNamespace::KeyState GetKey(int id) const { return keys[id]; }
	InputManagerNamespace::KeyState GetGamepadButton(int gamepadId, SDL_GameControllerButton button) const;

	GamePad pads[MAX_PADS];

	bool shutDownEngine = false;
};

#endif //INPUT_MANAGER_H
