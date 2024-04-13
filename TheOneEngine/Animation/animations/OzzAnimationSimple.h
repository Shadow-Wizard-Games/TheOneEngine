#pragma once

#include "../OzzAnimation.h"

class OzzAnimationSimple : public OzzAnimation
{
private:
	std::string m_AnimationPath;
	bool m_Loaded;

	// Playback animation controller. This is a utility class that helps with
	// controlling animation playback time.
	ozz::sample::PlaybackController controller_;

	// Runtime animation.
	ozz::animation::Animation animation_;

	// Buffer of local transforms as sampled from animation_.
	ozz::vector<ozz::math::SoaTransform> locals_;
public:
	OzzAnimationSimple();
	~OzzAnimationSimple();

	const char* getAnimationPath() { return m_AnimationPath.c_str(); }
	bool IsAnimationLoaded() { return m_Loaded; }

	bool LoadAnimation(const char* file);

	Status Validate() override;

	ozz::vector<ozz::math::SoaTransform>& getLocals() override { return locals_; }

	void OnSkeletonSet() override;

	bool Update(float _dt) override;

	void SetTimeRatio(float time_ratio) override;
	float GetTimeRatio() override;

	bool HasFinished() override;

	void AddKeyAction(Action<> action, float time) override;
	void RemoveKeyAction(Action<> action) override;

	void Play() override;
	void Stop() override;

	void OnPlaybackSpeedChange() override;
	void OnLoopChange() override;
};