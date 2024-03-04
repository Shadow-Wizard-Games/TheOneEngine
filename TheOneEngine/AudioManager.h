#pragma once
#include "Defs.h"
#include "..\TheOneAudio\AudioCore.h"
#include "AudioComponent.h"
 
class AudioManager {
public:
	AudioManager();

	bool Awake();
	bool Update(double dt);
	bool CleanUp();

	void PlayAudio();
	void StopAudio();
	void PauseAudio();
	void ResumeAudio();

	//static AudioManager* GetAudioManager() { return &this; }

	AudioCore* audio = nullptr;

	//AudioCore* GetAudioCore() { return audio; }
	void AddAudioObject(std::shared_ptr<AudioComponent> audioGO); // Change parameter to shared_ptr};

private:
	std::vector<std::shared_ptr<AudioComponent>> audioComponents;
};