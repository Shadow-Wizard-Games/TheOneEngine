#include "Easing.h"

Easing::Easing() {}

Easing::Easing(double totalTime)
{
    this->totalTime = totalTime;
    this->elapsedTime = 0;
    this->delayTime = 0;
    this->bFinished = true;
}

Easing::~Easing()
{
    //release stuff
}

// Sin
double Easing::EaseInSin(double t) {
    return sin(1.5707963 * t);
}
double Easing::EaseOutSin(double t) {
    return 1 + sin(1.5707963 * (--t));
}
double Easing::EaseInOutSin(double t) {
    return 0.5 * (1 + sin(3.1415926 * (t - 0.5)));
}

// Quad
double Easing::EaseInQuad(double t) {
    return t * t;
}
double Easing::EaseOutQuad(double t) {
    return t * (2 - t);
}
double Easing::EaseInOutQuad(double t) {
    return t < 0.5 ? 2 * t * t : t * (4 - 2 * t) - 1;
}

// Cubic
double Easing::EaseInCubic(double t) {
    return t * t * t;
}
double Easing::EaseOutCubic(double t) {
    return 1 + (--t) * t * t;
}
double Easing::EaseInOutCubic(double t) {
    return t < 0.5 ? 4 * t * t * t : 0.5 * pow(2 * t - 2, 3) + 1;
}

// Quart
double Easing::EaseInQuart(double t) {
    t *= t;
    return t * t;
}
double Easing::EaseOutQuart(double t) {
    t = (--t) * t;
    return 1 - t * t;
}
double Easing::EaseInOutQuart(double t) {
    if (t < 0.5) {
        t *= t;
        return 8 * t * t;
    }
    else {
        t = (--t) * t;
        return 1 - 8 * t * t;
    }
}

//Quint
double Easing::EaseInQuint(double t) {
    double t2 = t * t;
    return t * t2 * t2;
}
double Easing::EaseOutQuint(double t) {
    double t2 = (--t) * t;
    return 1 + t * t2 * t2;
}
double Easing::EaseInOutQuint(double t) {
    double t2;
    if (t < 0.5) {
        t2 = t * t;
        return 16 * t * t2 * t2;
    }
    else {
        t2 = (--t) * t;
        return 1 + 16 * t * t2 * t2;
    }
}

//Exponential
double Easing::EaseInExp(double t) {
    return (pow(2, 8 * t) - 1) / 255;
}
double Easing::EaseOutExp(double t) {
    return 1 - pow(2, -8 * t);
}
double Easing::EaseInOutExp(double t) {
    if (t < 0.5) {
        return (pow(2, 16 * t) - 1) / 510;
    }
    else {
        return 1 - 0.5 * pow(2, -16 * (t - 0.5));
    }
}

//Circ
double Easing::EaseInCirc(double t) {
    return 1 - sqrt(1 - t);
}
double Easing::EaseOutCirc(double t) {
    return sqrt(t);
}
double Easing::EaseInOutCirc(double t) {
    if (t < 0.5) {
        return (1 - sqrt(1 - 2 * t)) * 0.5;
    }
    else {
        return (1 + sqrt(2 * t - 1)) * 0.5;
    }
}

//Back
double Easing::EaseInBack(double t) {
    return t * t * (2.70158 * t - 1.70158);
}
double Easing::EaseOutBack(double t) {
    return 1 + (--t) * t * (2.70158 * t + 1.70158);
}
double Easing::EaseInOutBack(double t) {
    if (t < 0.5) {
        return t * t * (7 * t - 2.5) * 2;
    }
    else {
        return 1 + (--t) * t * 2 * (7 * t + 2.5);
    }
}

//Elastic
double Easing::EaseInElastic(double t) {
    double t2 = t * t;
    return t2 * t2 * sin(t * PI * 4.5);
}
double Easing::EaseOutElastic(double t) {
    double t2 = (t - 1) * (t - 1);
    return 1 - t2 * t2 * cos(t * PI * 4.5);
}
double Easing::EaseInOutElastic(double t) {
    double t2;
    if (t < 0.45) {
        t2 = t * t;
        return 8 * t2 * t2 * sin(t * PI * 9);
    }
    else if (t < 0.55) {
        return 0.5 + 0.75 * sin(t * PI * 4);
    }
    else {
        t2 = (t - 1) * (t - 1);
        return 1 - 8 * t2 * t2 * sin(t * PI * 9);
    }
}

//Bounce
double Easing::EaseInBounce(double t) {
    return pow(2, 6 * (t - 1)) * abs(sin(t * PI * 3.5));
}
double Easing::EaseOutBounce(double t) {
    return 1 - pow(2, -6 * t) * abs(cos(t * PI * 3.5));
}
double Easing::EaseInOutBounce(double t) {
    if (t < 0.5) {
        return 8 * pow(2, 8 * (t - 1)) * abs(sin(t * PI * 7));
    }
    else {
        return 1 - 8 * pow(2, -8 * t) * abs(sin(t * PI * 7));
    }
}


double Easing::EasingAnimation(int start, int end, double time, EasingType easingType)
{
    // Lambda function:
    // [capture list](parameters) -> return type {function body}
    // Captures current object and easingType by value,
    // making them accessible within the function's scope.
    auto easing = [this, easingType](double t) -> double
    {
        switch (easingType)
        {
            case EasingType::EASE_IN_SIN:           return EaseInSin(t);
            case EasingType::EASE_OUT_SIN:          return EaseOutSin(t);    
            case EasingType::EASE_INOUT_SIN:        return EaseInOutSin(t);
            case EasingType::EASE_IN_QUAD:          return EaseInQuad(t);
            case EasingType::EASE_OUT_QUAD:         return EaseOutQuad(t);
            case EasingType::EASE_INOUT_QUAD:       return EaseInOutQuad(t);
            case EasingType::EASE_IN_CUBIC:         return EaseInCubic(t);
            case EasingType::EASE_OUT_CUBIC:        return EaseOutCubic(t);
            case EasingType::EASE_INOUT_CUBIC:      return EaseInOutCubic(t);
            case EasingType::EASE_IN_QUART:         return EaseInQuart(t);
            case EasingType::EASE_OUT_QUART:        return EaseOutQuart(t);
            case EasingType::EASE_INOUT_QUART:      return EaseInOutQuart(t);
            case EasingType::EASE_IN_QUINT:         return EaseInQuint(t);
            case EasingType::EASE_OUT_QUINT:        return EaseOutQuint(t);
            case EasingType::EASE_INOUT_QUINT:      return EaseInOutQuint(t);
            case EasingType::EASE_IN_EXP:           return EaseInExp(t);
            case EasingType::EASE_OUT_EXP:          return EaseOutExp(t);
            case EasingType::EASE_INOUT_EXP:        return EaseInOutExp(t);
            case EasingType::EASE_IN_CIRC:          return EaseInCirc(t);
            case EasingType::EASE_OUT_CIRC:         return EaseOutCirc(t);
            case EasingType::EASE_INOUT_CIRC:       return EaseInOutCirc(t);
            case EasingType::EASE_IN_BACK:          return EaseInBack(t);
            case EasingType::EASE_OUT_BACK:         return EaseOutBack(t);
            case EasingType::EASE_INOUT_BACK:       return EaseInOutBack(t);
            case EasingType::EASE_IN_ELASTIC:       return EaseInElastic(t);
            case EasingType::EASE_OUT_ELASTIC:      return EaseOutElastic(t);
            case EasingType::EASE_INOUT_ELASTIC:    return EaseInOutElastic(t);
            case EasingType::EASE_IN_BOUNCE:        return EaseInBounce(t);
            case EasingType::EASE_OUT_BOUNCE:       return EaseOutBounce(t);
            case EasingType::EASE_INOUT_BOUNCE:     return EaseInOutBounce(t);
            default:                                return EaseInSin(t);
        }
    };

    double t = easing(time);
    return start + (end - start) * t;
}

double Easing::TrackTime(double dt)
{
    double dtCap = fmin(dt, 32);

    elapsedTime += (dtCap / 1000);

    if (elapsedTime < delayTime)
    {
        return 0;
    }
    else if (elapsedTime - delayTime < totalTime)
    {
        return (elapsedTime - delayTime) / totalTime;
    }
    else
    {
        bFinished = true;
        elapsedTime = 0;
        return 1;
    }
}