﻿using System;
using System.Runtime.CompilerServices;

public class IAudioSource : IComponent
{
    public enum AudioEvent : uint
    {
        A_AR = 3814696296U,
        A_BANDAGE = 2650455497U,
        A_FT = 3730808143U,
        A_GL_EXPLOSION = 337852012U,
        A_GL_SHOOT = 323248236U,
        A_LI = 3630142368U,
        A_S_ACTIVATE = 1589784596U,
        A_S_DEACTIVATE = 1176085861U,
        A_S_HIT = 2965692658U,
        DEBUG_GUNSHOT = 1535709413U,
        DEBUG_MUSIC1 = 830312575U,
        DEBUG_MUSIC2 = 830312572U,
        DEBUG_SPATIAL1 = 3632435752U,
        DEBUG_SPATIAL2 = 3632435755U,
        E_A_DEATH = 1015701557U,
        E_A_SHOOT = 2923735218U,
        E_A_STEP = 1828976739U,
        E_CB_DEATH = 2100764501U,
        E_EGG_CRACK = 3286329979U,
        E_FH_CRAWL = 3938483819U,
        E_FH_DEATH = 4203716004U,
        E_FH_JUMP = 2711568226U,
        E_Q_BOMB = 692574897U,
        E_Q_CHANGE = 3169029109U,
        E_Q_DEATH = 610103877U,
        E_Q_JUMP = 3869385341U,
        E_Q_SPIT = 2254856839U,
        E_Q_SPITCHARGE = 3547019691U,
        E_Q_STEP = 2725321747U,
        E_Q_TAILSHOT = 2084671035U,
        E_Q_TAILSWEEP = 1606613397U,
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
        PLAYMUSIC = 417627684U,
        STOPMUSIC = 1917263390U,
        UI_A_MENU = 464895987U,
        UI_CLICK = 2249769530U,
        UI_HOVER = 2118900976U,
        UI_PAUSEGAME = 3783962382U,
        VO_COMPUTER_LOG1 = 1406454956U,
        VO_COMPUTER_LOG2 = 1406454959U,
        VO_COMPUTER_LOG3 = 1406454958U,
        VO_COMPUTER_LOG4 = 1406454953U,
        VO_COMPUTER_LOG5 = 1406454952U,
        VO_GRANT_BACKCAMP1 = 1236238417U,
        VO_GRANT_ENDMISSIONSTUB1 = 2538900238U,
        VO_GRANT_ENDMISSIONSTUB2 = 2538900237U,
        VO_GRANT_FOURTHCOMPUTER1 = 2056092986U,
        VO_GRANT_FOURTHCOMPUTER2 = 2056092985U,
        VO_GRANT_NOAH1 = 1121060561U,
        VO_GRANT_NOAH2 = 1121060562U,
        VO_GRANT_NOAH3 = 1121060563U,
        VO_GRANT_NOAH4 = 1121060564U,
        VO_GRANT_OUTSIDESTUB1 = 4061054028U,
        VO_GUARD1_VOICELINE1_V1 = 3357592761U,
        VO_GUARD1_VOICELINE1_V2 = 3357592762U,
        VO_GUARD1_VOICELINE2 = 234843012U,
        VO_GUARD2_VOICELINE1 = 4164671226U,
        VO_GUARD2_VOICELINE2 = 4164671225U,
        VO_ISAAC_FULLHEALTH = 3566892078U,
        VO_ISAAC_HEALTH1 = 3701460526U,
        VO_ISAAC_HEALTH2 = 3701460525U,
        VO_ISAAC_HEALTH3 = 3701460524U,
        VO_ISAAC_HEALTH4 = 3701460523U,
        VO_ISAAC_VOICELINE1 = 2021844418U,
        VO_ISAAC_VOICELINE2 = 2021844417U,
        VO_ISAAC_VOICELINE3 = 2021844416U,
        VO_ISAAC_VOICELINE4 = 2021844423U,
        VO_ISAAC_VOICELINE5 = 2021844422U,
        VO_ISAAC_VOICELINE6 = 2021844421U,
        VO_ISAAC_VOICELINE7 = 2021844420U,
        VO_RESCUESHIP_NARRATIVEEND = 3906700528U,
        VO_RESCUESHIP_RESPONSE = 2704646508U,
        VO_SERGEANT_FIFHTCOMPUTER1 = 3072456904U,
        VO_SERGEANT_GENERAL1 = 1212771736U,
        VO_SERGEANT_GENERAL2 = 1212771739U,
        VO_SERGEANT_GENERAL3 = 1212771738U,
        VO_SERGEANT_GENERAL4 = 1212771741U,
        VO_SERGEANT_WAKEMED1 = 1695989290U,
        VO_SERGEANT_WAKEMED2 = 1695989289U,
        VO_SERGEANT_WAKEMED3 = 1695989288U,
        VO_SERGEANT_WAKEMED4 = 1695989295U,
        VO_SERGEANT_WAKEMED5 = 1695989294U,
        VO_SERGEANT_WAKEMED6 = 1695989293U,
        VO_STUB_BACKCAMP1 = 2462372217U,
        VO_STUB_CLEARROOM1 = 1857914381U,
        VO_STUB_CLEARROOM2 = 1857914382U,
        VO_STUB_CLEARROOM3 = 1857914383U,
        VO_STUB_CRASH1 = 2621040692U,
        VO_STUB_CRASH2 = 2621040695U,
        VO_STUB_DOOR1 = 26695775U,
        VO_STUB_DOOR2 = 26695772U,
        VO_STUB_ENDMISSIONNOAH1 = 4270907780U,
        VO_STUB_ENDMISSIONNOAH2 = 4270907783U,
        VO_STUB_FIFTHCOMPUTER1 = 485738767U,
        VO_STUB_FIGHTING1 = 827082109U,
        VO_STUB_FIGHTING2 = 827082110U,
        VO_STUB_FIGHTING3 = 827082111U,
        VO_STUB_FIGHTING4 = 827082104U,
        VO_STUB_FINDING1 = 3328660762U,
        VO_STUB_FINDING2 = 3328660761U,
        VO_STUB_FINDING3 = 3328660760U,
        VO_STUB_FIRSTALIEN = 3069220365U,
        VO_STUB_FIRSTAMIRA1 = 1225130969U,
        VO_STUB_FIRSTAMIRA2 = 1225130970U,
        VO_STUB_FIRSTAMIRA3 = 1225130971U,
        VO_STUB_FIRSTCOMPUTER1 = 2306385480U,
        VO_STUB_FIRSTNOAH1 = 207909039U,
        VO_STUB_FIRSTNOAH2 = 207909036U,
        VO_STUB_FIRSTNOAH3 = 207909037U,
        VO_STUB_FOURTHCOMPUTER1 = 633519362U,
        VO_STUB_FOURTHCOMPUTER2 = 633519361U,
        VO_STUB_GUARDWEAPON = 2456146089U,
        VO_STUB_HEAL1 = 2343389349U,
        VO_STUB_HEAL2 = 2343389350U,
        VO_STUB_HEAL3 = 2343389351U,
        VO_STUB_HURT1 = 1393191702U,
        VO_STUB_MEDCLEAR1 = 835901656U,
        VO_STUB_MEDCLEAR2 = 835901659U,
        VO_STUB_MISSIONAMIRA1 = 1255446693U,
        VO_STUB_MISSIONAMIRA2 = 1255446694U,
        VO_STUB_MISSIONAMIRA3 = 1255446695U,
        VO_STUB_MISSIONAMIRA4 = 1255446688U,
        VO_STUB_OUTSHIP = 1792990868U,
        VO_STUB_OUTSIDENOAH1 = 2130010906U,
        VO_STUB_PLANETINFO1 = 2925042445U,
        VO_STUB_PLANETINFO2 = 2925042446U,
        VO_STUB_PLANETINFO3 = 2925042447U,
        VO_STUB_SECONDCOMPUTER1 = 3696739954U,
        VO_STUB_SECONDCOMPUTER2 = 3696739953U,
        VO_STUB_SECONDCOMPUTER3 = 3696739952U,
        VO_STUB_THIRDCOMPUTER1 = 2301289131U,
        VO_STUB_WAKEMED1 = 3513509957U,
        VO_STUB_WAKEMED2 = 3513509958U,
        VO_STUB_WAKEMED3 = 3513509959U,
        VO_STUB_WAKEMED4 = 3513509952U,
        VO_STUB_WAKEMED5 = 3513509953U,
        VO_STUB_XENOFIGHT1 = 3871246353U,
        VO_STUB_XENOFIGHT2 = 3871246354U,
        VO_STUB_XENOFIGHT3 = 3871246355U,
        VO_STUB_XENOFIGHTEND1 = 3353530420U,
        VO_TTS = 1278196794U,
        W_M4_RELOAD = 2147632472U,
        W_M4_SHOOT = 933371860U,
        W_SL_SHOOT = 3010972734U
    }

    public enum AudioStateGroup : uint
    {
        GAMEPLAYMODE = 15699892U
    }

    public enum AudioStateID : uint
    {
        CAMP = 3736508534U,
        COMBAT = 2764240573U,
        NONE = 748895195U,
        RUINS = 417916826U,
        SHIP = 284967655U
    }

    public enum AudioSwitchGroup : uint
    {
        SURFACETYPE = 63790334U
    }

    public enum AudioSwitchID : uint
    {
        DIRT = 2195636714U,
        SHIP = 284967655U
    }

    public IAudioSource() : base() { }
    public IAudioSource(IntPtr GOptr) : base(GOptr) { }

    public AudioEvent currentID = 0;
    public void Play(AudioEvent audio)
    {
        InternalCalls.PlayAudioSource(containerGOptr, audio);
    }

    public void Stop(AudioEvent audio)
    {
        InternalCalls.StopAudioSource(containerGOptr, audio);
    }

    public void SetState(AudioStateGroup stateGroup, AudioStateID stateID)
    {
        InternalCalls.SetState(stateGroup, stateID);
    }

    public void SetSwitch(AudioSwitchGroup switchGroup, AudioSwitchID switchID)
    {
        InternalCalls.SetSwitch(containerGOptr, switchGroup, switchID);
    }
}
