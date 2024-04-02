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
		K = 14,
		RIGHT = 79,
		LEFT = 80,
		DOWN = 81,
		UP = 82,
		SPACEBAR = 44,
		ESCAPE = 41,
		RETURN = 40,
		LSHIFT = 225,
	}

	public enum MouseButtonCode
	{
		LEFT = 1,
		MIDDLE,
		RIGHT
	}

	public enum ControllerButtonCode
	{
		A,
		B,
		X,
		Y,
		BACK,
		GUIDE,
		START,
		L3,
		R3,
		L1,
		R1,
		UP,
		DOWN,
		LEFT,
		RIGHT

		//L2,
		//R2,
	}

	public enum ControllerJoystickCode
	{
		JOY_LEFT, JOY_RIGHT
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

