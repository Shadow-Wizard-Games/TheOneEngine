#pragma once

#include "Panel.h"
#include "../TheOneEngine/Defs.h"

//#include <Wiwa/animation/OzzAnimator.h>
//#include <Wiwa/animation/animations/OzzAnimationSimple.h>
//#include <glm/glm.hpp>

#include <string>


class Camera;
class OzzAnimator;
class OzzAnimationSimple;
class OzzAnimationPartialBlending;

class PanelAnimation : public Panel
{
public:
	PanelAnimation(PanelType type, std::string name);
	virtual ~PanelAnimation();

	bool Draw() override;

private:
	void DrawTopbar();
	void DrawBody();
	void DrawAnimationViewer();

	void DrawMeshContainer();
	void DrawSkeletonContainer();
	void DrawMaterialContainer();
	void DrawAnimations();

	void DrawSimpleAnimation(OzzAnimationSimple* simple_animation);
	void DrawPartialBlendingAnimation(OzzAnimationPartialBlending* partial_animation);

private:
	std::string m_ActiveAnimatorPath;
	OzzAnimator* m_ActiveAnimator;

	mat4 m_Transform;

	Camera* m_Camera;
};