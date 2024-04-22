using System;
using System.Runtime.CompilerServices;

public class IAudioSource : IComponent
{
    public enum EventIDs : uint
    {
        A_AMBIENT_1 = 1547765315U,
        A_CAMP_2 = 895381461U,
        A_COMBAT_1 = 3486252281U,
        A_RUINS_2 = 4189504465U,
        DEBUG_GUNSHOT = 1535709413U,
        DEBUG_MUSIC1 = 830312575U,
        DEBUG_MUSIC2 = 830312572U,
        DEBUG_SPATIAL1 = 3632435752U,
        DEBUG_SPATIAL2 = 3632435755U,
        E_CB_DEATH = 2100764501U,
        E_EGG_CRACK = 3286329979U,
        E_FH_CRAWL = 3938483819U,
        E_FH_DEATH = 4203716004U,
        E_FH_JUMP = 2711568226U,
        E_QUEEN_BOMB = 975078008U,
        E_QUEEN_CHANGE = 587747760U,
        E_QUEEN_DEATH = 1479922750U,
        E_QUEEN_JUMP = 3480783404U,
        E_QUEEN_SPIT = 3576793690U,
        E_QUEEN_STEP = 3778713374U,
        E_REBEL_DEATH = 4009974304U,
        E_REBEL_SHOOT = 319492235U,
        E_REBEL_STEP = 2489932272U,
        E_X_ADULT_DEATH = 3828971157U,
        E_X_ADULT_MELEE = 3271687631U,
        E_X_ADULT_SPAWN = 3991779178U,
        E_X_ADULT_SPIT = 2247807735U,
        E_X_ADULT_STEP = 1644504899U,
        P_DASH = 3919743498U,
        P_DEATH = 947529532U,
        P_HIT = 1073763149U,
        P_PICK = 1941667005U,
        P_ROLL = 2206507859U,
        P_STEP = 3875642140U,
        UI_A_MENU = 464895987U,
        UI_CLICK = 2249769530U,
        UI_HOVER = 2118900976U,
        UI_PAUSEGAME = 3783962382U,
        VO_GUARD2_VL1_0 = 1098830151U,
        VO_GUARD2_VL2_0 = 2172450704U,
        W_M4_RELOAD = 2147632472U,
        W_M4_SHOOT = 933371860U,
        W_SL_SHOOT = 3010972734U,
    }

    public IAudioSource() : base() { }
    public IAudioSource(IntPtr GOptr) : base(GOptr) { }
    
    public EventIDs currentID = 0;
    public void PlayAudio(EventIDs audio)
    {
        InternalCalls.PlaySource(containerGOptr, audio);
    }

    public void StopAudio(EventIDs audio)
    {
        InternalCalls.StopSource(containerGOptr, audio);
    }
}
