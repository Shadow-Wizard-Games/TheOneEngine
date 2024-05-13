#pragma once
#include "GameObject.h"
#include "Transform.h"

#include "..\TheOneAudio\AudioUtility.h"

#include <memory>

class GameObject;
class AudioManager;

class AudioComponent {
public:
	virtual void SetTransform(std::shared_ptr<GameObject> containerGO) = 0;
	
	// Get the associated game object
	std::shared_ptr<GameObject> GetGameObject() {return GO;}

public:
	std::shared_ptr<GameObject> GO;
	AkGameObjectID goID;

	AudioManager* AM = nullptr;
};