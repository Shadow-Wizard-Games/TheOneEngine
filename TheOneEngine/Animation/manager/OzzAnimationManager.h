#pragma once

#include "../OzzAnimator.h"
#include "../animations/OzzAnimationPartialBlending.h"


class OzzAnimationManager
{
public:
	OzzAnimationManager();
	~OzzAnimationManager();

	void PlayAnimation(const char* name);

	OzzAnimator* getAnimator() { return m_Animator; }

	void Awake();
	void Init();
	void Update();


private:
	OzzAnimator* m_Animator;
	/*EntityManager::ComponentIterator m_TransformIt;
	EntityManager::ComponentIterator m_AnimatorIt;
	EntityManager::ComponentIterator m_HitIt;*/
};