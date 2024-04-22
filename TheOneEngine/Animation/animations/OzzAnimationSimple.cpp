#include "OzzAnimationSimple.h"

#include "TheOneEngine/Log.h"
#include "TheOneEngine/Action.h"


OzzAnimationSimple::OzzAnimationSimple() :
	m_Loaded(false)
{
	m_AnimType = AT_SIMPLE;
}

OzzAnimationSimple::~OzzAnimationSimple()
{
}

bool OzzAnimationSimple::LoadAnimation(const char* file)
{
	if (!skeleton_) return false;

	m_Loaded = ozz::sample::LoadAnimation(file, &animation_);
	controller_.set_time_ratio(0.0f);

	if (m_Loaded)
	{
		m_AnimationPath = file;

		if (skeleton_->num_joints() != animation_.num_tracks())
		{
			LOG(LogType::LOG_WARNING, "The provided animation doesn't match skeleton (joint count mismatch).");
			LOG(LogType::LOG_INFO, "- Joints: %d", skeleton_->num_joints());
			LOG(LogType::LOG_INFO, "- Tracks: %d", animation_.num_tracks());

			m_Loaded = false;
		}
	}

	Validate();

	return m_Loaded;
}

OzzAnimation::Status OzzAnimationSimple::Validate()
{
	m_Status = INVALID;

	if (!skeleton_) return INVALID;
	if (!m_Loaded) return INVALID;

	m_Status = VALID;

	return VALID;
}

void OzzAnimationSimple::OnSkeletonSet()
{
	// Initialize skeleton joint data
	const int num_joints = skeleton_->num_joints();
	const int num_soa_joints = skeleton_->num_soa_joints();

	context_.Resize(num_joints);

	// Allocates local space runtime buffers of blended data.
	locals_.resize(num_soa_joints);
}

bool OzzAnimationSimple::Update(float _dt)
{
	// Updates current animation time.
	controller_.Update(animation_, _dt);

	// Samples optimized animation at t = animation_time_.
	ozz::animation::SamplingJob sampling_job;
	sampling_job.animation = &animation_;
	sampling_job.context = &context_;
	sampling_job.ratio = controller_.time_ratio();
	sampling_job.output = make_span(locals_);
	if (!sampling_job.Run()) {
		return false;
	}

	return true;
}

void OzzAnimationSimple::SetTimeRatio(float time_ratio)
{
	controller_.set_time_ratio(time_ratio);
}
float OzzAnimationSimple::GetTimeRatio()
{
	return controller_.time_ratio();
}

bool OzzAnimationSimple::HasFinished()
{
	if (m_Loop) return false;

	return controller_.finished();
}

void OzzAnimationSimple::AddKeyAction(Action<> action, float time)
{
	controller_.add_key_event(action, time);
}

void OzzAnimationSimple::RemoveKeyAction(Action<> action)
{
	controller_.remove_key_event(action);
}

void OzzAnimationSimple::Play()
{
	controller_.play();
}

void OzzAnimationSimple::Stop()
{
	controller_.stop();
}

void OzzAnimationSimple::OnPlaybackSpeedChange()
{
	controller_.set_playback_speed(m_PlaybackSpeed);
}

void OzzAnimationSimple::OnLoopChange()
{
	controller_.set_loop(m_Loop);
}