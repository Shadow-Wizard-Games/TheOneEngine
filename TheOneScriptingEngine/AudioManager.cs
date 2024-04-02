using System;
using System.Runtime.CompilerServices;

public class AudioManager
{
    public enum EventIDs : uint
    {
        CLICK = 1584507803U,
        COMBAT = 2764240573U,
        ENEMYATTACK = 2144693767U,
        EXPLORE = 579523862U,
        GUNSHOT = 1287408361U,
        MAINMENU = 3604647259U,
        HOVER = 3753593413U,
        MUSIC1 = 1730564819U,
        STEP = 621108255U,
    }

    public static void PlaySource(IntPtr GOptr, EventIDs audio)
    {
        InternalCalls.PlaySource(GOptr, audio);
    }

    public static void StopSource(IntPtr GOptr, EventIDs audio)
    {
        InternalCalls.StopSource(GOptr, audio);
    }
}
