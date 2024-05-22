#pragma once
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include <AK/SoundEngine/Common/AkMemoryMgr.h>			// Memory Manager interface
#include <AK/SoundEngine/Common/AkModule.h>				// Default memory manager

#include <AK/SoundEngine/Common/IAkStreamMgr.h>			// Streaming Manager
#include <AK/Tools/Common/AkPlatformFuncs.h>			// Thread defines
#include <Common/AkFilePackageLowLevelIODeferred.h>		// Sample low-level I/O implementation

#include <AK/SoundEngine/Common/AkSoundEngine.h>		// Sound engine

#include <AK/MusicEngine/Common/AkMusicEngine.h>		// Music Engine

#include <AK/SpatialAudio/Common/AkSpatialAudio.h>		// Spatial Audio

//file packaging
#include <../Common/AkFilePackage.h>
#include <../Common/AkFilePackageLUT.h>

//communications
// Include for communication between Wwise and the game -- Not needed in the release version
#ifndef AK_OPTIMIZED
#include <AK/Comm/AkCommunication.h>
#endif // AK_OPTIMIZED

#define BANKNAME_INIT L"Init.bnk"
#define BANKNAME_THEONEENGINE L"TheOneEngine.bnk"
#define BANKNAME_VOICEOVER L"VoiceOver.bnk"

#include "../TheOneEditor/Assets/Audio/Wwise Project/GeneratedSoundBanks/Wwise_IDs.h"

//others
using namespace std;