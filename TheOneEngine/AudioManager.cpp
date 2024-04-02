#include "AudioManager.h"
#include "Source.h"

AudioManager::AudioManager()
{
	audio = new AudioCore();
}

bool AudioManager::Awake()
{
	audio->Awake();
	return true;
}

bool AudioManager::Update(double dt)
{
	audio->Update(dt);
	for (const auto& audioComponent : audioComponents) {
		audioComponent->SetTransform(audioComponent->GetGameObject());
	}
	return true;
}

bool AudioManager::CleanUp()
{
	audio->CleanUp();
	delete audio;
	return true;
}

// JULS: Not sure tho if they should be added like this
void AudioManager::PlayAudio(Source* source, AkUniqueID event)
{
	audio->PlayEvent(event, source->goID);
}

void AudioManager::StopAudio(Source* source, AkUniqueID event)
{
	audio->StopEvent(event, source->goID);
}

void AudioManager::PauseAudio(Source* source, AkUniqueID event)
{
	audio->PauseEvent(event, source->goID);
}

void AudioManager::ResumeAudio(Source* source, AkUniqueID event)
{
	audio->ResumeEvent(event, source->goID);
}

// Implementation of AddAudioObject
void AudioManager::AddAudioObject(std::shared_ptr<AudioComponent> audioGO)
{
	audioComponents.push_back(audioGO);
}

