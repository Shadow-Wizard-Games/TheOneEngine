using System;
using System.Runtime.CompilerServices;

public class AudioManager
{
    public enum EventIDs : uint
    {
        A_AMBIENT_1 = 1547765315U,
        A_COMBAT_1 = 3486252281U,
        DEBUG_MUSIC1 = 830312575U,
        DEBUG_MUSIC2 = 830312572U,
        DEBUG_SPATIAL1 = 3632435752U,
        DEBUG_SPATIAL2 = 3632435755U,
        E_EGG_CRACK = 3286329979U,
        E_FH_CRAWL = 3938483819U,
        E_FH_DEATH = 4203716004U,
        E_FH_JUMP = 2711568226U,
        E_REBEL_DEATH = 4009974304U,
        E_REBEL_SHOOT = 319492235U,
        E_REBEL_STEP = 2489932272U,
        E_X_ADULT_DEATH = 3828971157U,
        E_X_ADULT_MELEE = 3271687631U,
        E_X_ADULT_SPIT = 2247807735U,
        E_X_ADULT_STEP = 1644504899U,
        P_DEATH = 947529532U,
        P_HIT = 1073763149U,
        P_PICK = 1941667005U,
        P_STEP = 3875642140U,
        UI_A_MENU = 464895987U,
        UI_CLICK = 2249769530U,
        UI_HOVER = 2118900976U,
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
