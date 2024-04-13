#include "AudioManager.h"
#include "AudioSource.h"

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
	//DeleteAudioComponents();

	audio->CleanUp();

	audio = nullptr;
	delete audio;

	return true;
}

void AudioManager::PlayAudio(AudioSource* source, AkUniqueID event)
{
	audio->PlayEvent(event, source->goID);
}

void AudioManager::StopAudio(AudioSource* source, AkUniqueID event)
{
	audio->StopEvent(event, source->goID);
}

void AudioManager::PauseAudio(AudioSource* source, AkUniqueID event)
{
	audio->PauseEvent(event, source->goID);
}

void AudioManager::ResumeAudio(AudioSource* source, AkUniqueID event)
{
	audio->ResumeEvent(event, source->goID);
}

void AudioManager::AddAudioObject(std::shared_ptr<AudioComponent> audioGO)
{
	audioComponents.push_back(audioGO);
}

void AudioManager::DeleteAudioComponents()
{
	audioComponents.clear();
}