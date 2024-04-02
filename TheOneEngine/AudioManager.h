#pragma once
#include "AudioComponent.h"
#include "Source.h"

#include "..\TheOneAudio\AudioCore.h"

class AudioManager {
public:
	AudioManager();

	bool Awake();
	bool Update(double dt);
	bool CleanUp();

	void PlayAudio(Source* source, AkUniqueID event);
	void StopAudio(Source* source, AkUniqueID event);
	void PauseAudio(Source* source, AkUniqueID event);
	void ResumeAudio(Source* source, AkUniqueID event);

	//static AudioManager* GetAudioManager() { return &this; }

	AudioCore* audio = nullptr;

	//AudioCore* GetAudioCore() { return audio; }
	void AddAudioObject(std::shared_ptr<AudioComponent> audioGO); // Change parameter to shared_ptr};

private:
	std::vector<std::shared_ptr<AudioComponent>> audioComponents;
};