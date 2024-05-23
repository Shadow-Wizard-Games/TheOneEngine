using System;
using System.Runtime.CompilerServices;

public class Easing
{
    public enum EasingState
    {
        PLAY,
        PAUSE,
        END,
        END_LOOP
    };

    public enum EasingType
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

    EasingState state;
    double elapsedTime;
    double totalTime;
    double delay;
    bool loop;

    public Easing(double totalTime, double delay, bool loop)
    {
        this.totalTime = totalTime;
        this.delay = delay;
        this.loop = loop;
        elapsedTime = 0;
        state = EasingState.PAUSE;
    }

    public void Play()
    {
        state = EasingState.PLAY;
    }

    public void Pause()
    {
        state = EasingState.PAUSE;
    }

    public void Reset()
    {
        elapsedTime = 0;
        state = EasingState.PAUSE;
    }

    public void Replay()
    {
        elapsedTime = 0;
        state = EasingState.PLAY;
    }

    private double NormalizeTime()
    {
        switch (state)
        {
            case EasingState.PLAY:
                if (elapsedTime < delay) return 0;
                return (elapsedTime - delay) / totalTime;

            case EasingState.END:
                elapsedTime = totalTime;
                return 1;

            case EasingState.END_LOOP:
                elapsedTime -= totalTime;
                return (elapsedTime - delay) / totalTime;

            default: return (elapsedTime - delay) / totalTime;
        }
    }

    private double InterpolateEasing(double t, EasingType type)
    {
        switch(type)
        {
            case EasingType.EASE_IN_SIN: return Math.Sin((Math.PI * 0.5) * t);
            case EasingType.EASE_OUT_SIN: return (1 + Math.Sin((Math.PI * 0.5) * (--t)));
            case EasingType.EASE_INOUT_SIN: return 0.5 * (1 + Math.Sin(Math.PI * (t - 0.5)));

            case EasingType.EASE_IN_QUAD: return t * t;
            case EasingType.EASE_OUT_QUAD: return t * (2 - t);
            case EasingType.EASE_INOUT_QUAD: return t < 0.5 ? 2 * t * t : t * (4 - 2 * t) - 1;

            case EasingType.EASE_IN_CUBIC: return t * t * t;
            case EasingType.EASE_OUT_CUBIC: return 1 + (--t) * t * t;
            case EasingType.EASE_INOUT_CUBIC: return t < 0.5 ? 4 * t * t * t : 0.5 * Math.Pow(2 * t - 2, 3) + 1;

            case EasingType.EASE_IN_QUART: t *= t; return t * t;
            case EasingType.EASE_OUT_QUART: t = (--t) * t; return 1 - t * t;
            case EasingType.EASE_INOUT_QUART:
                if (t < 0.5)
                {
                    t *= t;
                    return 8 * t * t;
                }
                else
                {
                    t = (--t) * t;
                    return 1 - 8 * t * t;
                }

            case EasingType.EASE_IN_QUINT:
                return t * t * t * t * t;
            case EasingType.EASE_OUT_QUINT:
                return 1 + t * (--t) * t * (--t) * t;
            case EasingType.EASE_INOUT_QUINT:
                if (t < 0.5)
                {
                    return 16 * t * t * t * t * t;
                }
                else
                {
                    return 1 + 16 * t * (--t) * t * (--t) * t;
                }

            case EasingType.EASE_IN_EXP: return (Math.Pow(2, 8 * t) - 1) / 255;
            case EasingType.EASE_OUT_EXP: return 1 - Math.Pow(2, -8 * t);
            case EasingType.EASE_INOUT_EXP:
                if (t < 0.5)
                {
                    return (Math.Pow(2, 16 * t) - 1) / 510;
                }
                else
                {
                    return 1 - 0.5 * Math.Pow(2, -16 * (t - 0.5));
                }

            case EasingType.EASE_IN_CIRC: return 1 - Math.Sqrt(1 - t);
            case EasingType.EASE_OUT_CIRC: return Math.Sqrt(t);
            case EasingType.EASE_INOUT_CIRC:
                if (t < 0.5)
                {
                    return (1 - Math.Sqrt(1 - 2 * t)) * 0.5;
                }
                else
                {
                    return (1 + Math.Sqrt(2 * t - 1)) * 0.5;
                }

            case EasingType.EASE_IN_BACK: return t * t * (2.70158 * t - 1.70158);
            case EasingType.EASE_OUT_BACK: return 1 + (--t) * t * (2.70158 * t + 1.70158);
            case EasingType.EASE_INOUT_BACK:
                if (t < 0.5)
                {
                    return t * t * (7 * t - 2.5) * 2;
                }
                else
                {
                    return 1 + (--t) * t * 2 * (7 * t + 2.5);
                }

            case EasingType.EASE_IN_ELASTIC:
                return t * t * t * t * Math.Sin(t * Math.PI * 4.5);

            case EasingType.EASE_OUT_ELASTIC:
                return 1 - (t - 1) * (t - 1) * (t - 1) * (t - 1) * Math.Cos(t * Math.PI * 4.5);

            case EasingType.EASE_INOUT_ELASTIC:
                if (t < 0.45)
                {
                    return 8 * t * t * t * t * Math.Sin(t * Math.PI * 9);
                }
                else if (t < 0.55)
                {
                    return 0.5 + 0.75 * Math.Sin(t * Math.PI * 4);
                }
                else
                {
                    return 1 - 8 * (t - 1) * (t - 1) * (t - 1) * (t - 1) * Math.Sin(t * Math.PI * 9);
                }

            case EasingType.EASE_IN_BOUNCE: return Math.Pow(2, 6 * (t - 1)) * Math.Abs(Math.Sin (t * Math.PI * 3.5));

            case EasingType.EASE_OUT_BOUNCE: return 1 - Math.Pow(2, -6 * t) * Math.Abs(Math.Cos(t * Math.PI * 3.5));

            case EasingType.EASE_INOUT_BOUNCE:
                if (t < 0.5)
                {
                    return 8 * Math.Pow(2, 8 * (t - 1)) * Math.Abs(Math.Sin (t * Math.PI * 7));
                }
                else
                {
                    return 1 - 8 * Math.Pow(2, -8 * t) * Math.Abs(Math.Sin (t * Math.PI * 7));
                }

            default: return Math.Sin((Math.PI * 0.5) * t); ;
        }
    }
    public double UpdateEasing(double startValue, double endValue, double dt, EasingType type)
    {
        if (state != EasingState.PAUSE)
        {
            elapsedTime += dt;

            // Set state
            if (elapsedTime - delay < totalTime)
                state = EasingState.PLAY;
            else if (!loop)
                state = EasingState.END;
            else
                state = EasingState.END_LOOP;
        }

        double time = NormalizeTime();

        double t = InterpolateEasing(time, type);

        return startValue + (endValue - startValue) * t;
    }
}

