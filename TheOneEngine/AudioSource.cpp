#include "AudioSource.h"
#include "EngineCore.h"
#include "AudioManager.h"

#include "..\TheOneEditor\App.h"

AudioSource::AudioSource(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::AudioSource) /*, path(""), volume(0.5)*/
{	
	this->GO = containerGO;
	SetTransform(containerGO);

	//am->AddAudioObject(std::shared_ptr<AudioComponent>(this));
}

AudioSource::AudioSource(std::shared_ptr<GameObject> containerGO, AudioSource* ref) : Component(containerGO, ComponentType::AudioSource) /*, path(ref->path), volume(ref->volume)*/
{
}

AudioSource::~AudioSource()
{
}

void AudioSource::SetTransform(std::shared_ptr<GameObject> containerGO)
{
	AkSoundPosition tTransform;
	AkVector pos;
	pos.X = -containerGO.get()->GetComponent<Transform>()->GetPosition().x;
	pos.Y = containerGO.get()->GetComponent<Transform>()->GetPosition().y;
	pos.Z = -containerGO.get()->GetComponent<Transform>()->GetPosition().z;

	tTransform.SetPosition(pos);

	AkVector forward;
	forward.X = containerGO.get()->GetComponent<Transform>()->GetForward().x;
	forward.Y = containerGO.get()->GetComponent<Transform>()->GetForward().y;
	forward.Z = containerGO.get()->GetComponent<Transform>()->GetForward().z;

	AkVector up;
	up.X = containerGO.get()->GetComponent<Transform>()->GetUp().x;
	up.Y = containerGO.get()->GetComponent<Transform>()->GetUp().y;
	up.Z = containerGO.get()->GetComponent<Transform>()->GetUp().z;

	tTransform.SetOrientation(forward, up);

	if (AK::SoundEngine::SetPosition(goID, tTransform) != AK_Success)
	{
		// I should put this function and rotation in virtual to change logs in Listener and Source
		LOG(LogType::LOG_AUDIO, "ERROR setting transform to AudioSource: %s", containerGO->GetName().c_str());
	}
}

// JULS: TODO -> SaveComponent and LoadComponent AudioSource
json AudioSource::SaveComponent()
{
	json sourceJSON;

	sourceJSON["Name"] = name;
	sourceJSON["Type"] = type;

	sourceJSON["AudioID"] = goID;
	//sourceJSON["Event"] = event;
	
	return sourceJSON;
}

void AudioSource::LoadComponent(const json& sourceJSON)
{
	if (sourceJSON.contains("Name"))
	{
		name = sourceJSON["Name"];
	}

	if (sourceJSON.contains("AudioID"))
	{
		goID = sourceJSON["AudioID"];
	}
}
