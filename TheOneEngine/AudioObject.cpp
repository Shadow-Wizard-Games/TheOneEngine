#include "AudioObject.h"
#include "..\TheOneEditor\Log.h"

AudioObject::AudioObject(std::shared_ptr<GameObject> containerGO)
{
	//SetPosition(containerGO);
}

AudioObject::~AudioObject()
{
}

void AudioObject::SetPosition(std::shared_ptr<GameObject> containerGO)
{
	//AkSoundPosition tTransform;
	//tTransform.SetPosition({ -containerGO.get()->GetComponent<Transform>()->getPosition().x, containerGO.get()->GetComponent<Transform>()->getPosition().y, -containerGO.get()->GetComponent<Transform>()->getPosition().z });
	//
	//if (AK::SoundEngine::SetPosition(goID, tTransform) != AK_Success)
	//{
	//	LOG(LogType::LOG_AUDIO, "ERROR setting position to backgroundmusic (default listener)");
	//}
}

void AudioObject::SetRotation(std::shared_ptr<GameObject> containerGO)
{
	//AkSoundPosition tTransform;
	//AkVector forward;
	//forward.X = containerGO.get()->GetComponent<Transform>()->getForward().x;
	//forward.Y = containerGO.get()->GetComponent<Transform>()->getForward().y;
	//forward.X = containerGO.get()->GetComponent<Transform>()->getForward().z;
	//
	//AkVector up;
	//up.X = containerGO.get()->GetComponent<Transform>()->getUp().x;
	//up.Y = containerGO.get()->GetComponent<Transform>()->getUp().y;
	//up.Z = containerGO.get()->GetComponent<Transform>()->getUp().z;
	//
	//tTransform.SetOrientation(forward, up);
	//
	//if (AK::SoundEngine::SetPosition(goID, tTransform) != AK_Success)
	//{
	//	LOG(LogType::LOG_AUDIO, "ERROR setting position to backgroundmusic (default listener)");
	//}
}
