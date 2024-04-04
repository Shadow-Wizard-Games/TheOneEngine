#ifndef __SOURCE_H__
#define __SOURCE_H__
#pragma once

#include "Component.h"
#include "AudioComponent.h"

class GameObject;
class AudioManager; 

class AudioSource : public Component, public AudioComponent {
public:
	AudioSource(std::shared_ptr<GameObject> containerGO);
	AudioSource(std::shared_ptr<GameObject> containerGO, AudioSource* ref);

	virtual ~AudioSource();

	void SetTransform(std::shared_ptr<GameObject> containerGO);

	json SaveComponent();
	void LoadComponent(const json& sourceJSON);

	//string path;
	//int volume;
	//AkUniqueID event;
private:

};

#endif // !__SOURCE_H__