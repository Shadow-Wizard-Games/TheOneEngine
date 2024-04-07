#pragma once

#include "../OzzAnimation.h"

class OzzAnimationPartialBlending : public OzzAnimation
{
private:
	// Sampler struct that helps to control an animation
	struct Sampler
	{
		// Constructor, default initialization.
		Sampler() : weight_setting(1.f), joint_weight_setting(1.f) {}

		// Playback animation controller. This is a utility class that helps with
		// controlling animation playback time.
		ozz::sample::PlaybackController controller;

		// Blending weight_setting for the layer.
		float weight_setting;

		// Blending weight_setting setting of the joints of this layer that are
		// affected
		// by the masking.
		float joint_weight_setting;

		// Runtime animation.
		ozz::animation::Animation animation;

		// Sampling context.
		ozz::animation::SamplingJob::Context context;

		// Buffer of local transforms as sampled from animation_.
		ozz::vector<ozz::math::SoaTransform> locals;

		// Per-joint weights used to define the partial animation mask. Allows to
		// select which joints are considered during blending, and their individual
		// weight_setting.
		ozz::vector<ozz::math::SimdFloat4> joint_weights;
	};

private:
	std::string m_LowerBodyFile;
	std::string m_UpperBodyFile;

	// Index of the joint at the base of the upper body hierarchy.
	int upper_body_root_;

	// Blending job rest pose threshold.
	float threshold_;

	enum
	{
		kLowerBody = 0,
		kUpperBody = 1,
		kNumLayers = 2,
	};

	// Samplers list
	Sampler samplers_[kNumLayers];

	// Buffer of local transforms as sampled from animation_.
	ozz::vector<ozz::math::SoaTransform> locals_;

	// Buffer of local transforms which stores the blending result.
	ozz::vector<ozz::math::SoaTransform> blended_locals_;

	void SetupPerJointWeights();

	// Helper functor used to set weights while traversing joints hierarchy.
	struct WeightSetupIterator
	{
		WeightSetupIterator(ozz::vector<ozz::math::SimdFloat4>* _weights,
			float _weight_setting)
			: weights(_weights), weight_setting(_weight_setting) {}
		void operator()(int _joint, int) {
			ozz::math::SimdFloat4& soa_weight = weights->at(_joint / 4);
			soa_weight = ozz::math::SetI(
				soa_weight, ozz::math::simd_float4::Load1(weight_setting),
				_joint % 4);
		}
		ozz::vector<ozz::math::SimdFloat4>* weights;
		float weight_setting;
	};

	bool m_LoadedLower;
	bool m_LoadedUpper;

	bool UpdateAnimation(float _dt);
	bool SampleLocals(float _dt);

public:
	OzzAnimationPartialBlending();
	~OzzAnimationPartialBlending();

	void SetUpperBodyRoot(int ubr);
	int GetUpperBodyRoot() { return upper_body_root_; }

	bool LoadLowerAnimation(const char* lower);
	bool LoadUpperAnimation(const char* upper);

	bool IsLowerAnimationLoaded() { return m_LoadedLower; }
	bool IsUpperAnimationLoaded() { return m_LoadedUpper; }

	bool Update(float _dt) override;

	Status Validate() override;

	void OnSkeletonSet() override;

	void setUpperPlaybackSpeed(float playback_speed);
	float getUpperPlaybackSpeed();

	void setLowerPlaybackSpeed(float playback_speed);
	float getLowerPlaybackSpeed();

	void setTimeRatio(float time_ratio) override;
	float getTimeRatio() override;

	bool HasFinished() override;

	void OnPlaybackSpeedChange() override;
	void OnLoopChange() override;

	void addKeyAction(Action<> action, float time) override;
	void removeKeyAction(Action<> action) override;

	void Play() override;
	void Stop() override;

	ozz::vector<ozz::math::SoaTransform>& getLocals() override { return blended_locals_; }

	const char* getLowerBodyFile() { return m_LowerBodyFile.c_str(); }
	const char* getUpperBodyFile() { return m_UpperBodyFile.c_str(); }
};