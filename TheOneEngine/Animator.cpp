#include "Animator.h"


Animator::Animator(std::shared_ptr<GameObject> containerGO) :
	Component(containerGO, ComponentType::Animator),
	animatorPath()
{}

Animator::~Animator() {}

json Animator::SaveComponent()
{
	json somerandomFile;
	return somerandomFile;
}

void Animator::LoadComponent(const json& animatorJSON)
{
}