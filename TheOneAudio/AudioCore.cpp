#include "AudioCore.h"
#include "..\TheOneEngine\Log.h"

AudioEvent::AudioEvent()
{
    playing_id = 0L;
    event_call_back = &AudioCore::EventCallBack;
}

bool AudioEvent::IsEventPlaying()
{
    return playing_id != 0L;
}

AudioCore::AudioCore()
{
    nextSong = false;
}

AudioCore::~AudioCore() {}

bool AudioCore::InitEngine()
{
    LOG(LogType::LOG_INFO, "# Initializing Audio Engine...");

    if (InitMemoryManager())  LOG(LogType::LOG_OK, "-Init Memory Manager");
    else LOG(LogType::LOG_ERROR, "- Failed Init Memory Manager");

    if (InitStreamingManager()) LOG(LogType::LOG_OK, "-Init Streaming Manager");
    else LOG(LogType::LOG_ERROR, "- Failed Init Streaming Manager");

    if (InitSoundEngine()) LOG(LogType::LOG_OK, "-Init Sound Engine");
    else LOG(LogType::LOG_ERROR, "- Failed Init Sound Engine");

    if (InitMusicEngine()) LOG(LogType::LOG_OK, "-Init Music Engine");
    else LOG(LogType::LOG_ERROR, "- Failed Init Music Engine");

    g_lowLevelIO.SetBasePath(AKTEXT("Assets\\Audio\\Wwise Project\\GeneratedSoundBanks\\Windows"));
    g_lowLevelIO.SetBasePath(AKTEXT("Assets\\Audio\\Wwise Project\\GeneratedSoundBanks"));
    AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(us)"));

    //bank init
    AkBankID bankID;
    if (AK::SoundEngine::LoadBank(BANKNAME_INIT, bankID) == AK_Success)
    {
        LOG(LogType::LOG_OK, "-Loaded BANKNAME_INIT ");
    }
    else
    {
        LOG(LogType::LOG_ERROR, "-Failed loading BANKNAME_INIT");
        return false;
    }

    if (AK::SoundEngine::LoadBank(BANKNAME_THEONEENGINE, bankID) == AK_Success)
    {
        LOG(LogType::LOG_OK, "-Loaded BANKNAME_THEONEENGINE");
    }
    else
    {
        LOG(LogType::LOG_ERROR, "-Failed loading BANKNAME_THEONEENGINE");
        return false;
    }

    if (AK::SoundEngine::LoadBank(BANKNAME_VOICEOVER, bankID) == AK_Success)
    {
        LOG(LogType::LOG_OK, "-Loaded BANKNAME_VOICEOVER");
    }
    else
    {
        LOG(LogType::LOG_ERROR, "-Failed loading BANKNAME_VOICEOVER");
        return false;
    }

    if (InitSpatialAudio()) LOG(LogType::LOG_OK, "-Init Spatial Audio");
    else LOG(LogType::LOG_ERROR, "-Failed Init Spatial Audio.");

#ifndef AK_OPTIMIZED
    if (InitCommunication()) LOG(LogType::LOG_OK, "-Initialized Communication");
    else LOG(LogType::LOG_ERROR, "-Failed Init Communication");
#endif // AK_OPTIMIZED

    return true;
}

bool AudioCore::InitMemoryManager()
{
    AkMemSettings memSettings;
    AK::MemoryMgr::GetDefaultSettings(memSettings);

    if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
    {
        LOG(LogType::LOG_ERROR, "Failed Init Memory Manager.");
        return false;
    }

    return true;
}

bool AudioCore::InitStreamingManager()
{
    AkStreamMgrSettings stmSettings;
    AK::StreamMgr::GetDefaultSettings(stmSettings);

    // Customize the Stream Manager settings here.
    // (...)

    if (!AK::StreamMgr::Create(stmSettings))
    {
        LOG(LogType::LOG_ERROR, "Failed Init Streaming Manager");
        return false;
    }

    // Create a streaming device.
    // Note that you can override the default low-level I/O module with your own. 
    AkDeviceSettings deviceSettings;
    AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

    // Customize the streaming device settings here.
    // (...)

    // CAkFilePackageLowLevelIODeferred::Init() creates a streaming device
    // in the Stream Manager, and registers itself as the File Location Resolver.

    if (g_lowLevelIO.Init(deviceSettings) != AK_Success)
    {
        LOG(LogType::LOG_AUDIO, "Could not create the streaming device and Low-Level I/O system");
        return false;
    }

    return true;
}

bool AudioCore::InitSoundEngine()
{
    AkInitSettings initSettings;
    AkPlatformInitSettings platformInitSettings;
    AK::SoundEngine::GetDefaultInitSettings(initSettings);
    AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

    if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
    {
        LOG(LogType::LOG_AUDIO, "Could not initialize the Sound Engine.");
        return false;
    }

    return true;
}

bool AudioCore::InitMusicEngine()
{
    AkMusicSettings musicInit;
    AK::MusicEngine::GetDefaultInitSettings(musicInit);

    if (AK::MusicEngine::Init(&musicInit) != AK_Success)
    {
        LOG(LogType::LOG_AUDIO, "Could not initialize the Music Engine.");
        return false;
    }
    return true;
}

bool AudioCore::InitSpatialAudio()
{
    // Initialize Spatial Audio -> Using default initialization parameters
    AkSpatialAudioInitSettings settings; // The constructor fills AkSpatialAudioInitSettings with the recommended default settings. 

    if (AK::SpatialAudio::Init(settings) != AK_Success)
    {
        LOG(LogType::LOG_AUDIO, "Could not initialize the Spatial Audio.");
        return false;
    }
    return true;
}

bool AudioCore::InitCommunication()
{
#ifndef AK_OPTIMIZED
    // Initialize communications (not in release build!)

    AkCommSettings commSettings;
    AK::Comm::GetDefaultInitSettings(commSettings);
    if (AK::Comm::Init(commSettings) != AK_Success)
    {
        LOG(LogType::LOG_AUDIO, "Could not initialize the communications.");
        return false;
    }
#endif // AK_OPTIMIZED

    return true;
}

void AudioCore::Awake()
{
    if (InitEngine())
        LOG(LogType::LOG_AUDIO, "Initialized the Audio Engine.");
    else
        LOG(LogType::LOG_AUDIO, "Could not initialize the Audio Engine.");

    //creating audio events
    for (size_t i = 0; i < MAX_AUDIO_EVENTS; i++)
    {
        audioEvents.push_back(new AudioEvent);
    }

    SetGlobalVolume(globalVolume);

}

void AudioCore::Update(double dt)
{
    //always call this function on update to make things work
    AK::SoundEngine::RenderAudio();


    if (state == EngineState::PLAYING)
    {

    }
}

void AudioCore::CleanUp()
{
    // Free audioEvents memory
    for (auto event : audioEvents) delete event;
    audioEvents.clear();

    AK::SoundEngine::ClearBanks(); 

#ifndef AK_OPTIMIZED
    AK::Comm::Term();
#endif // AK_OPTIMIZED
    //commented cz theres no term function xd
    //AK::SpatialAudio::Term();
    AK::MusicEngine::Term();
    AK::SoundEngine::Term();

    g_lowLevelIO.Term();

    if (AK::IAkStreamMgr::Get())
    {
        AK::IAkStreamMgr::Get()->Destroy();
    }

    AK::MemoryMgr::Term();
}

void AudioCore::SetDefaultListener(AkGameObjectID goID)
{
    AK::SoundEngine::SetDefaultListeners(&goID, 1);
    LOG(LogType::LOG_AUDIO, "Set default listener: %d audiogameobject", goID);
}

void AudioCore::RemoveDefaultListener(AkGameObjectID goID)
{
    AK::SoundEngine::RemoveDefaultListener(goID);
    LOG(LogType::LOG_AUDIO, "Remove default listener: %d audiogameobject", goID);
}

AkGameObjectID AudioCore::RegisterGameObject(std::string name)
{
    if (AK::SoundEngine::RegisterGameObj((AkGameObjectID)gameObjectIDs.size(), name.c_str()) == AK_Success)
    {
        LOG(LogType::LOG_AUDIO, "Game Object %s with ID %d SUCCESS on Register", name.c_str(), gameObjectIDs.size());
        gameObjectIDs.push_back((AkGameObjectID)gameObjectIDs.size());
        return gameObjectIDs.size() - 1;
    }
    else
    {
        LOG(LogType::LOG_AUDIO, "Game Object %s ERROR on Register", name.c_str());
        return -1;
    }
}

void AudioCore::UnregisterGameObject(AkGameObjectID goID, std::string name)
{
    if (AK::SoundEngine::UnregisterGameObj(goID) == AK_Success)
    {
        LOG(LogType::LOG_AUDIO, "Game Object %s with ID %d SUCCESS on Unregister", name.c_str(), gameObjectIDs.size());
        gameObjectIDs.erase(gameObjectIDs.begin() + goID);
        //return gameObjectIDs.size() - 1;
    }
    else
    {
        LOG(LogType::LOG_AUDIO, "Game Object %s ERROR on Unregister", name.c_str());
        //return -1;
    }
}

void AudioCore::PlayEvent(AkUniqueID event, AkGameObjectID goID)
{
    for (size_t i = 0; i < MAX_AUDIO_EVENTS; i++)
    {
        //if we found unused audio event we use it
        if (audioEvents[i]->playing_id == 0L)
        {
            AK::SoundEngine::PostEvent(event, goID, AkCallbackType::AK_EndOfEvent, audioEvents[i]->event_call_back, (void*)audioEvents[i]);
            //LOG(LogType::LOG_AUDIO, "Playing event from %d audiogameobject", goID);
            audioEvents[i]->playing_id = 1L;
            return;
        }
    }
    LOG(LogType::LOG_AUDIO, "Maximum amount of audio events at the same time reached: %d", MAX_AUDIO_EVENTS);
}

void AudioCore::StopEvent(AkUniqueID event, AkGameObjectID goID)
{
    AK::SoundEngine::ExecuteActionOnEvent(event, AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Stop, gameObjectIDs[goID]);
    LOG(LogType::LOG_AUDIO, "Stopping event from %d audiogameobject", goID);
}

void AudioCore::PauseEvent(AkUniqueID event, AkGameObjectID goID)
{
    AK::SoundEngine::ExecuteActionOnEvent(event, AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Pause, gameObjectIDs[goID]);
    LOG(LogType::LOG_AUDIO, "Pausing event from %d audiogameobject", goID);
}

void AudioCore::ResumeEvent(AkUniqueID event, AkGameObjectID goID)
{
    AK::SoundEngine::ExecuteActionOnEvent(event, AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Resume, gameObjectIDs[goID]);
    LOG(LogType::LOG_AUDIO, "Resuming event from %d audiogameobject", goID);
}

void AudioCore::SetState(AkStateGroupID stateGroup, AkStateID state)
{
    AK::SoundEngine::SetState(stateGroup, state);
    LOG(LogType::LOG_AUDIO, "Setting state");
}

void AudioCore::SetState(const char* stateGroup, const char* state)
{
    AK::SoundEngine::SetState(stateGroup, state);
    LOG(LogType::LOG_AUDIO, "Setting state");
}

void AudioCore::SetSwitch(AkSwitchGroupID switchGroup, AkSwitchStateID switchState, AkGameObjectID goID)
{
    AK::SoundEngine::SetSwitch(switchGroup, switchState, goID);
    LOG(LogType::LOG_AUDIO, "Setting switch");
}

void AudioCore::SetRTPCValue(const char* name, int value)
{
    AK::SoundEngine::SetRTPCValue(name, value);
}

// JULS: Probably not necessary actually
/*
void AudioCore::PlayEngine()
{
    if (state == EngineState::PAUSED)
    {
        //resume stuff
        for (size_t i = 0; i < MAX_AUDIO_EVENTS; i++)
        {
            if (audioEvents[i] != NULL)
            {
                AK::SoundEngine::ExecuteActionOnPlayingID(AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Resume, audioEvents[i]->playing_id);
            }
        }
    }
    else if (state == EngineState::STOPPED)
    {
        //play stuff

    }

    state = EngineState::PLAYING;
}

void AudioCore::PauseEngine()
{
    state = EngineState::PAUSED;

    //will PAUSE all sounds
    for (size_t i = 0; i < MAX_AUDIO_EVENTS; i++)
    {
        if (audioEvents[i] != NULL)
        {
            AK::SoundEngine::ExecuteActionOnPlayingID(AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Pause, audioEvents[i]->playing_id);
        }
    }
}

void AudioCore::StopEngine()
{
    state = EngineState::STOPPED;

    //will STOP all sounds (not pause)
    for (size_t i = 0; i < gameObjectIDs.size(); i++)
    {
        AK::SoundEngine::StopAll(gameObjectIDs[i]);
    }
}
*/

void AudioCore::SetGlobalVolume(float volume)
{
    if (volume < 0.0f)
    {
        volume = 0.0f;
    }
    else if (volume > 100.0f)
    {
        volume = 100.0f;
    }
    globalVolume = volume;
    AK::SoundEngine::SetOutputVolume(AK::SoundEngine::GetOutputID(AK_INVALID_UNIQUE_ID, 0.0f), (AkReal32)(volume * 0.01f));
}

void AudioCore::SetMaster(int volume)
{
    AK::SoundEngine::SetRTPCValue(AK::GAME_PARAMETERS::MASTERVOLUME, (AkRtpcValue)volume);
}

void AudioCore::SetDialog(int volume)
{
    AK::SoundEngine::SetRTPCValue(AK::GAME_PARAMETERS::VOICESVOLUME, (AkRtpcValue)volume);
}

void AudioCore::SetSFX(int volume)
{
    AK::SoundEngine::SetRTPCValue(AK::GAME_PARAMETERS::SFXVOLUME, (AkRtpcValue)volume);
}

void AudioCore::SetMusic(int volume)
{
    AK::SoundEngine::SetRTPCValue(AK::GAME_PARAMETERS::MUSICVOLUME, (AkRtpcValue)volume);
}

void AudioCore::EventCallBack(AkCallbackType in_eType, AkCallbackInfo* in_pCallbackInfo)
{
    AudioEvent* a_event = (AudioEvent*)in_pCallbackInfo->pCookie;

    switch (in_eType)
    {
    case(AkCallbackType::AK_EndOfEvent):
    {
        a_event->playing_id = 0L;
        break;
    }
    }
}