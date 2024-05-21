using System;
using System.Runtime.CompilerServices;

public class IAnimator : IComponent
{
    public IAnimator() : base() { }

    public IAnimator(IntPtr gameObjectRef) : base(gameObjectRef) { }

    public void Play(string name)
    {
        InternalCalls.PlayAnimation(containerGOptr, name);
    }
    public void Stop()
    {
        InternalCalls.StopAnimation(containerGOptr);
    }

    public bool CurrentAnimHasFinished
    {
        get
        {
            return InternalCalls.AnimationHasFinished(containerGOptr);
        }
    }

    public bool Blend
    {
        get
        {
            return InternalCalls.GetTransitionBlend(containerGOptr);
        }
        set
        {
            InternalCalls.SetTransitionBlend(containerGOptr, ref value);
        }
    }

    public float TransitionTime
    {
        get
        {
            return InternalCalls.GetTransitionTime(containerGOptr);
        }
        set
        {
            InternalCalls.SetTransitionTime(containerGOptr, ref value);
        }
    }

    public void UpdateAnimation()
    {
        float dt = Time.deltaTime;
        InternalCalls.UpdateAnimation(containerGOptr, ref dt);
    }
}
