#pragma once

#include <cmath>

#define PI acos(-1)


enum class EasingState
{
    PLAY,
    PAUSE,
    END,
    END_LOOP
};

enum class EasingType
{
    EASE_IN_SIN,
    EASE_OUT_SIN,
    EASE_INOUT_SIN,
    EASE_IN_QUAD,
    EASE_OUT_QUAD,
    EASE_INOUT_QUAD,
    EASE_IN_CUBIC,
    EASE_OUT_CUBIC,
    EASE_INOUT_CUBIC,
    EASE_IN_QUART,
    EASE_OUT_QUART,
    EASE_INOUT_QUART,
    EASE_IN_QUINT,
    EASE_OUT_QUINT,
    EASE_INOUT_QUINT,
    EASE_IN_EXP,
    EASE_OUT_EXP,
    EASE_INOUT_EXP,
    EASE_IN_CIRC,
    EASE_OUT_CIRC,
    EASE_INOUT_CIRC,
    EASE_IN_BACK,
    EASE_OUT_BACK,
    EASE_INOUT_BACK,
    EASE_IN_ELASTIC,
    EASE_OUT_ELASTIC,
    EASE_INOUT_ELASTIC,
    EASE_IN_BOUNCE,
    EASE_OUT_BOUNCE,
    EASE_INOUT_BOUNCE
};


class Easing
{
public:
    Easing(double totalTime, double delay = 0, bool loop = false);
    ~Easing();

    // Sin
    double EaseInSin(double t);
    double EaseOutSin(double t);
    double EaseInOutSin(double t);

    // Quadratic
    double EaseInQuad(double t);
    double EaseOutQuad(double t);
    double EaseInOutQuad(double t);

    // Cubic
    double EaseInCubic(double t);
    double EaseOutCubic(double t);
    double EaseInOutCubic(double t);

    // Quartic
    double EaseInQuart(double t);
    double EaseOutQuart(double t);
    double EaseInOutQuart(double t);

    //Quintic
    double EaseInQuint(double t);
    double EaseOutQuint(double t);
    double EaseInOutQuint(double t);

    //Exponential
    double EaseInExp(double t);
    double EaseOutExp(double t);
    double EaseInOutExp(double t);

    //Circular
    double EaseInCirc(double t);
    double EaseOutCirc(double t);
    double EaseInOutCirc(double t);

    //Back
    double EaseInBack(double t);
    double EaseOutBack(double t);
    double EaseInOutBack(double t);

    //Elastic
    double EaseInElastic(double t);
    double EaseOutElastic(double t);
    double EaseInOutElastic(double t);

    //Bounce
    double EaseInBounce(double t);
    double EaseOutBounce(double t);
    double EaseInOutBounce(double t);


    double TrackTime(double dt, bool loop);
    double Ease(int start, int end, double dt, EasingType easingType, bool loop);

    void Play() { state = EasingState::PLAY; }

    void Pause() { state = EasingState::PAUSE; }

    void Reset()
    {
        elapsedTime = 0;
        state = EasingState::PAUSE;
    }

    EasingState GetState() const { return state; }
    void SetState(EasingState state) { this->state = state; }

    double GetElapsedTime() const { return elapsedTime; }
    void SetElapsedTime(double elapsedTime) { this->elapsedTime = elapsedTime; }

    double GetTotalTime() const { return totalTime; }
    void SetTotalTime(double totalTime) { this->totalTime = totalTime; }

    double GetDelayTime() const { return delay; }
    void SetDelayTime(double delay) { this->delay = delay; }

    double IsLooped() const { return loop; }
    void SetLoop(bool loop) { this->loop = loop; }

private:

    EasingState state;
    double elapsedTime;
    double totalTime;
    double delay;
    bool loop;
};