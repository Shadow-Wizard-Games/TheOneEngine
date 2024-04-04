#pragma once
#include "AudioComponent.h"
#include "AudioSource.h"

#include "..\TheOneAudio\AudioCore.h"

class AudioManager {
public:
	AudioManager();

	bool Awake();
	bool Update(double dt);
	bool CleanUp();

public:
	void PlayAudio(AudioSource* source, AkUniqueID event);
	void StopAudio(AudioSource* source, AkUniqueID event);
	void PauseAudio(AudioSource* source, AkUniqueID event);
	void ResumeAudio(AudioSource* source, AkUniqueID event);

	void FadeAudio(AudioSource* source, AkUniqueID event1, AkUniqueID event2);

	void AddAudioObject(std::shared_ptr<AudioComponent> audioGO);
	void DeleteAudioComponents();

	AudioCore* audio = nullptr;
private:
	std::vector<std::shared_ptr<AudioComponent>> audioComponents;
};