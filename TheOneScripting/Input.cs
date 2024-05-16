using System;
using System.Runtime.CompilerServices;

public class Input
{

	public enum KeyboardCode
	{
		A = 4,
		D = 7,
		S = 22,
		W = 26,
		E = 8,
		F = 9,
		G = 10,
		K = 14,
		I = 12,
		Q = 20,
		R = 21,
		T = 23,
		F1 = 58,
		F2 = 59,
		F3 = 60,
		RIGHT = 79,
		LEFT = 80,
		DOWN = 81,
		UP = 82,
		SPACEBAR = 44,
		ESCAPE = 41,
		RETURN = 40,
		LSHIFT = 225,
		//Test abilities TO DELETE when changed
		ONE = 30,
		TWO = 31,
        THREE = 32,
        FOUR = 33,
        FIVE = 34,
        SIX = 35,
	}

	public enum MouseButtonCode
	{
		LEFT = 1,
		MIDDLE,
		RIGHT
	}

	public enum ControllerButtonCode
	{
		A = 0,
		B = 1,
		X = 2,
		Y = 3,
		BACK = 4,
		GUIDE = 5,
		START = 6,
		L3 = 7,
		R3 = 8,
		L1 = 9,
		R1 = 10,
		UP = 11,
		DOWN = 12,
		LEFT = 13,
		RIGHT = 14,

		L2 = 22,
		R2 = 23,
	}

	public enum ControllerJoystickCode
	{
		JOY_LEFT,
		JOY_RIGHT,
	}

	public static bool GetKeyboardButton(KeyboardCode key)
	{
		return InternalCalls.GetKeyboardButton((int) key);
	}

	public static bool GetControllerButton(ControllerButtonCode button, int controller = 0)
	{
		return InternalCalls.GetControllerButton((int)button, controller);
	}

	public static Vector2 GetControllerJoystick(ControllerJoystickCode joystick, int controller = 0)
	{
		Vector2 joyResult = Vector2.zero;
		InternalCalls.GetControllerJoystick((int)joystick, ref joyResult, controller);
		return joyResult;
	}
}

