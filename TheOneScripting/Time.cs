﻿using System;
using System.Runtime.CompilerServices;

public class Time
{
    const float maxDeltaTime = 0.5f;
    public static float deltaTime
    {
        get
        {
            if (InternalCalls.GetAppDeltaTime() > maxDeltaTime)
            {
                return maxDeltaTime;
            }
            else { return InternalCalls.GetAppDeltaTime(); }
        }
    }

    public static float realDeltaTime
    {
        get
        {
            return InternalCalls.GetAppDeltaTime();
        }
    }
}
