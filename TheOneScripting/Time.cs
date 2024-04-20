using System;
using System.Runtime.CompilerServices;

public class Time
{
	public static float deltaTime
	{
		get
		{
			return InternalCalls.GetAppDeltaTime();
		}
	}

}
