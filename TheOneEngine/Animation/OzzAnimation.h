#pragma once

// Include core
//#include <Wiwa/core/Core.h>

// Include ozz
#include "animation/runtime/animation.h"
#include "animation/runtime/blending_job.h"
#include "animation/runtime/local_to_model_job.h"
#include "animation/runtime/sampling_job.h"
#include "animation/runtime/skeleton.h"
#include "animation/runtime/skeleton_utils.h"
#include "base/containers/vector.h"
#include "base/log.h"
#include "base/maths/simd_math.h"
#include "base/maths/soa_transform.h"
#include "base/maths/vec_float.h"
#include "options/options.h"
#include "samples/utils.h"
#include "samples/mesh.h"


enum AnimationType
{
    AT_PARTIAL_BLEND,
    AT_SIMPLE,
    AT_LAST
};

class OzzAnimation
{
public:
    enum Status
    {
        VALID,
        INVALID,
        LAST
    };

public:
    OzzAnimation() : m_AnimType(AT_LAST), skeleton_(nullptr), m_Status(INVALID), m_PlaybackSpeed(1.0f), m_Loop(true) {}

    AnimationType getAnimationType() { return m_AnimType; }

    virtual ozz::vector<ozz::math::SoaTransform>& getLocals() = 0;

    virtual Status Validate() = 0;

    virtual bool Update(float _dt) = 0;

    virtual void OnSkeletonSet() = 0;

    virtual void setTimeRatio(float time_ratio) = 0;
    virtual float getTimeRatio() = 0;

    virtual void OnPlaybackSpeedChange() = 0;
    virtual void OnLoopChange() = 0;

    virtual bool HasFinished() = 0;

    virtual void Play() = 0;
    virtual void Stop() = 0;

    virtual void addKeyAction(Action<> action, float time) = 0;
    virtual void removeKeyAction(Action<> action) = 0;

    void setLoop(bool loop) { m_Loop = loop; OnLoopChange(); }
    bool getLoop() { return m_Loop; }

    void setPlaybackSpeed(float speed) { m_PlaybackSpeed = speed; OnPlaybackSpeedChange(); }
    float getPlaybackSpeed() { return m_PlaybackSpeed; }

    void SetSkeleton(ozz::animation::Skeleton* skeleton);

    Status getStatus() { return m_Status; }

protected:
    AnimationType m_AnimType;

    Status m_Status;

    // Runtime skeleton.
    ozz::animation::Skeleton* skeleton_;

    // Sampling context.
    ozz::animation::SamplingJob::Context context_;

    float m_PlaybackSpeed;
    bool m_Loop;
};