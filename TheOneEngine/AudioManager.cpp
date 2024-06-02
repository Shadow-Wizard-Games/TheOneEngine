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

	delete audio;
	audio = nullptr;

	return true;
}

void AudioManager::PlayAudio(AudioSource* source, AkUniqueID event)
{
	if (source != nullptr)
		audio->PlayEvent(event, source->goID);
	else
		LOG(LogType::LOG_ERROR, "There is no component Audio Source to play");
}

void AudioManager::StopAudio(AudioSource* source, AkUniqueID event)
{
	if (source != nullptr)
		audio->StopEvent(event, source->goID);
	else
		LOG(LogType::LOG_ERROR, "There is no component Audio Source to stop");
}

void AudioManager::PauseAudio(AudioSource* source, AkUniqueID event)
{
	audio->PauseEvent(event, source->goID);
}

void AudioManager::ResumeAudio(AudioSource* source, AkUniqueID event)
{
	audio->ResumeEvent(event, source->goID);
}

void AudioManager::SetSwitch(AudioSource* source, AkSwitchGroupID switchGroup, AkSwitchStateID switchState)
{
	audio->SetSwitch(switchGroup, switchState, source->goID);
}

void AudioManager::AddAudioObject(std::shared_ptr<AudioComponent> audioGO)
{
	audioComponents.push_back(audioGO);
}

void AudioManager::RemoveAudioObject(std::shared_ptr<AudioComponent> audioGO)
{
	//audioComponents.
}

void AudioManager::DeleteAudioComponents()
{
	audioComponents.clear();
}