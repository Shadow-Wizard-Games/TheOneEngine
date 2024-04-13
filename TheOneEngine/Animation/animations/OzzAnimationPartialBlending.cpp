//#include <wipch.h>
#include "OzzAnimationPartialBlending.h"
//#include <Wiwa/core/Renderer3D.h>
//#include <Wiwa/core/Application.h>

#include "..\..\Log.h"


OzzAnimationPartialBlending::OzzAnimationPartialBlending() :
    upper_body_root_(0),
    threshold_(ozz::animation::BlendingJob().threshold),
    m_LoadedLower(false),
    m_LoadedUpper(false)
{
    m_AnimType = AT_PARTIAL_BLEND;
}

OzzAnimationPartialBlending::~OzzAnimationPartialBlending()
{}

bool OzzAnimationPartialBlending::LoadLowerAnimation(const char* lower)
{
    if (!skeleton_) return false;

    m_LoadedLower = ozz::sample::LoadAnimation(lower, &samplers_[kLowerBody].animation);
    samplers_[kLowerBody].controller.set_time_ratio(0.0f);

    if (m_LoadedLower)
    {
        m_LowerBodyFile = lower;

        if (skeleton_->num_joints() != samplers_[kLowerBody].animation.num_tracks())
        {
            LOG(LogType::LOG_WARNING, "The provided animation doesn't match skeleton (joint count mismatch).");
            LOG(LogType::LOG_INFO, "- Joints: %d", skeleton_->num_joints());
            LOG(LogType::LOG_INFO, "- Tracks: %d", samplers_[kLowerBody].animation.num_tracks());

            m_LoadedLower = false;
        }
    }

    Validate();

    return m_LoadedLower;
}

bool OzzAnimationPartialBlending::LoadUpperAnimation(const char* upper)
{
    if (!skeleton_) return false;

    m_LoadedUpper = ozz::sample::LoadAnimation(upper, &samplers_[kUpperBody].animation);
    samplers_[kUpperBody].controller.set_time_ratio(0.0f);

    if (m_LoadedUpper)
    {
        m_UpperBodyFile = upper;

        if (skeleton_->num_joints() != samplers_[kUpperBody].animation.num_tracks())
        {
            LOG(LogType::LOG_WARNING, "The provided animation doesn't match skeleton (joint count mismatch).");
            LOG(LogType::LOG_INFO, "- Joints: %d", skeleton_->num_joints());
            LOG(LogType::LOG_INFO, "- Tracks: %d", samplers_[kLowerBody].animation.num_tracks());

            m_LoadedUpper = false;
        }
    }

    Validate();

    return m_LoadedUpper;
}

bool OzzAnimationPartialBlending::SampleLocals(float _dt)
{
    // Updates and samples both animations to their respective local space
    // transform buffers.
    for (int i = 0; i < kNumLayers; ++i)
    {
        Sampler& sampler = samplers_[i];

        // Updates animations time.
        sampler.controller.Update(sampler.animation, _dt);

        // Setup sampling job.
        ozz::animation::SamplingJob sampling_job;
        sampling_job.animation = &sampler.animation;
        sampling_job.context = &sampler.context;
        sampling_job.ratio = sampler.controller.time_ratio();
        sampling_job.output = make_span(sampler.locals);

        // Samples animation.
        if (!sampling_job.Run())
            return false;
    }

    // Blends animations.
    // Blends the local spaces transforms computed by sampling all animations
    // (1st stage just above), and outputs the result to the local space
    // transform buffer blended_locals_

    // Prepares blending layers.
    ozz::animation::BlendingJob::Layer layers[kNumLayers];
    for (int i = 0; i < kNumLayers; ++i)
    {
        layers[i].transform = make_span(samplers_[i].locals);
        layers[i].weight = samplers_[i].weight_setting;

        // Set per-joint weights for the partially blended layer.
        layers[i].joint_weights = make_span(samplers_[i].joint_weights);
    }

    // Setups blending job.
    ozz::animation::BlendingJob blend_job;
    blend_job.threshold = threshold_;
    blend_job.layers = layers;
    blend_job.rest_pose = skeleton_->joint_rest_poses();
    blend_job.output = make_span(blended_locals_);

    // Blends.
    if (!blend_job.Run())
        return false;

    return true;
}

bool OzzAnimationPartialBlending::Update(float _dt)
{
    if (m_Status == INVALID) return false;

    bool update = true;

    update &= UpdateAnimation(_dt);

    return update;
}

OzzAnimation::Status OzzAnimationPartialBlending::Validate()
{
    m_Status = INVALID;

    if (!skeleton_) return INVALID;
    if (!m_LoadedLower) return INVALID;
    if (!m_LoadedUpper) return INVALID;

    m_Status = VALID;

    return VALID;
}

void OzzAnimationPartialBlending::OnSkeletonSet()
{
    // Initialize skeleton joint data
    const int num_joints = skeleton_->num_joints();
    const int num_soa_joints = skeleton_->num_soa_joints();

    // Initialize animations.

    for (int i = 0; i < kNumLayers; ++i)
    {
        Sampler& sampler = samplers_[i];

        // Allocates sampler runtime buffers.
        sampler.locals.resize(num_soa_joints);

        // Allocates per-joint weights used for the partial animation. Note that
        // this is a Soa structure.
        sampler.joint_weights.resize(num_soa_joints);

        // Allocates a context that matches animation requirements.
        sampler.context.Resize(num_joints);
    }

    // Default weight settings.
    Sampler& lower_body_sampler = samplers_[kLowerBody];
    lower_body_sampler.weight_setting = 1.f;
    lower_body_sampler.joint_weight_setting = 0.f;

    Sampler& upper_body_sampler = samplers_[kUpperBody];
    upper_body_sampler.weight_setting = 1.f;
    upper_body_sampler.joint_weight_setting = 1.f;


    // Allocates local space runtime buffers of blended data.
    blended_locals_.resize(num_soa_joints);

    // Init joint weights
    SetupPerJointWeights();
}

bool OzzAnimationPartialBlending::HasFinished()
{
    if (m_Loop) return false;

    return samplers_[kLowerBody].controller.finished() && samplers_[kUpperBody].controller.finished();
}

void OzzAnimationPartialBlending::OnPlaybackSpeedChange()
{
    for (int i = 0; i < kNumLayers; ++i) {
        samplers_[i].controller.set_playback_speed(m_PlaybackSpeed);
    }
}

void OzzAnimationPartialBlending::OnLoopChange()
{
    for (int i = 0; i < kNumLayers; ++i) {
        samplers_[i].controller.set_loop(m_Loop);
    }
}

void OzzAnimationPartialBlending::AddKeyAction(Action<> action, float time)
{
    samplers_[kLowerBody].controller.add_key_event(action, time);
}

void OzzAnimationPartialBlending::RemoveKeyAction(Action<> action)
{
    samplers_[kLowerBody].controller.remove_key_event(action);
}

void OzzAnimationPartialBlending::Play()
{
    for (int i = 0; i < kNumLayers; ++i) {
        samplers_[i].controller.play();
    }
}

void OzzAnimationPartialBlending::Stop()
{
    for (int i = 0; i < kNumLayers; ++i) {
        samplers_[i].controller.stop();
    }
}


// Get/Set ---------------------------------------------------------------

void OzzAnimationPartialBlending::SetUpperBodyRoot(int ubr)
{
    if (ubr < skeleton_->num_joints()) {
        upper_body_root_ = ubr;
    }

    if (m_Status == INVALID) return;

    SetupPerJointWeights();
}

void OzzAnimationPartialBlending::SetUpperPlaybackSpeed(float playback_speed)
{
    samplers_[kUpperBody].controller.set_playback_speed(playback_speed);
}

float OzzAnimationPartialBlending::GetUpperPlaybackSpeed()
{
    return samplers_[kUpperBody].controller.playback_speed();
}

void OzzAnimationPartialBlending::SetLowerPlaybackSpeed(float playback_speed)
{
    samplers_[kLowerBody].controller.set_playback_speed(playback_speed);

    m_PlaybackSpeed = playback_speed;
}

float OzzAnimationPartialBlending::GetLowerPlaybackSpeed()
{
    return samplers_[kLowerBody].controller.playback_speed();
}

void OzzAnimationPartialBlending::SetTimeRatio(float time_ratio)
{
    for (int i = 0; i < kNumLayers; ++i) {
        samplers_[i].controller.set_time_ratio(time_ratio);
    }
}

float OzzAnimationPartialBlending::GetTimeRatio()
{
    return samplers_[kLowerBody].controller.time_ratio();
}


// private ------------------

void OzzAnimationPartialBlending::SetupPerJointWeights()
{
    // Setup partial animation mask. This mask is defined by a weight_setting
    // assigned to each joint of the hierarchy. Joint to disable are set to a
    // weight_setting of 0.f, and enabled joints are set to 1.f.
    // Per-joint weights of lower and upper body layers have opposed values
    // (weight_setting and 1 - weight_setting) in order for a layer to select
    // joints that are rejected by the other layer.
    Sampler& lower_body_sampler = samplers_[kLowerBody];
    Sampler& upper_body_sampler = samplers_[kUpperBody];

    // Disables all joints: set all weights to 0.
    for (int i = 0; i < skeleton_->num_soa_joints(); ++i)
    {
        lower_body_sampler.joint_weights[i] = ozz::math::simd_float4::one();
        upper_body_sampler.joint_weights[i] = ozz::math::simd_float4::zero();
    }

    // Sets the weight_setting of all the joints children of the lower and upper
    // body weights. Note that they are stored in SoA format.
    WeightSetupIterator lower_it(&lower_body_sampler.joint_weights,
        lower_body_sampler.joint_weight_setting);
    ozz::animation::IterateJointsDF(*skeleton_, lower_it, upper_body_root_);

    WeightSetupIterator upper_it(&upper_body_sampler.joint_weights,
        upper_body_sampler.joint_weight_setting);
    ozz::animation::IterateJointsDF(*skeleton_, upper_it, upper_body_root_);
}

bool OzzAnimationPartialBlending::UpdateAnimation(float _dt)
{
    if (!SampleLocals(_dt)) return false;

    return true;
}
