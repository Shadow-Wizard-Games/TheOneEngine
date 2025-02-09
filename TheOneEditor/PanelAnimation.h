#pragma once

#include "Panel.h"
#include "TheOneEngine/Defs.h"

#include <string>


class SkeletalModel;
class OzzAnimationSimple;
class OzzAnimationPartialBlending;
class GameObject;
class FrameBuffer;

class PanelAnimation : public Panel
{
public:
	PanelAnimation(PanelType type, std::string name);
	virtual ~PanelAnimation();

	bool Draw() override;

private:

	void CameraControl();
	bool AnimationAvaliable();
	void Settings();
	void Viewport();

	void DrawAnimations();

	void DrawSimpleAnimation(OzzAnimationSimple* simple_animation);
	void DrawPartialBlendingAnimation(OzzAnimationPartialBlending* partial_animation);

private:
	SkeletalModel* activeAnimator;
	bool isPlaying;

	std::shared_ptr<GameObject> animationCamera;
	std::shared_ptr<FrameBuffer> frameBuffer;
};