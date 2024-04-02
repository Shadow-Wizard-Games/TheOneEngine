#ifndef __SOURCE_H__
#define __SOURCE_H__
#pragma once

#include "Component.h"
#include "AudioComponent.h"

class GameObject;
class AudioManager; 

class Source : public Component, public AudioComponent {
public:
	Source(std::shared_ptr<GameObject> containerGO);
	Source(std::shared_ptr<GameObject> containerGO, Source* ref);

	virtual ~Source();

	void SetTransform(std::shared_ptr<GameObject> containerGO);

	json SaveComponent();
	void LoadComponent(const json& sourceJSON);

	string path;
	int volume;
	//AkUniqueID event;
private:

};

#endif // !__SOURCE_H__