#pragma once

#include "GameObject.h"


class Animator : public Component
{
public:
	Animator(std::shared_ptr<GameObject> containerGO);
	~Animator();

	json SaveComponent();
	void LoadComponent(const json& animatorJSON);

	const char* GetAnimatorPath() { return animatorPath; }

private:
	char animatorPath[256];
};