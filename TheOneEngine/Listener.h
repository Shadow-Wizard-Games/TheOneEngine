#ifndef __LISTENER_H__
#define __LISTENER_H__

#pragma once

#include "Component.h"
#include "AudioComponent.h"
#include "AudioManager.h"

class GameObject;
class AudioManager;

class Listener : public Component, public AudioComponent {
public:
    Listener(std::shared_ptr<GameObject> containerGO);
    Listener(std::shared_ptr<GameObject> containerGO, Listener* ref);

    virtual ~Listener();

    void SetListener(AkGameObjectID goID);
    void SetTransform(std::shared_ptr<GameObject> containerGO);

    json SaveComponent();
    void LoadComponent(const json& listenerJSON);

private:


};

#endif // !__LISTENER_H__