using System;
public class IAnimator : IComponent
{
    public IAnimator() : base() { }

    public IAnimator(IntPtr gameObjectRef) : base(gameObjectRef) { }
    
    public void PlayAnimation()
    {

    }
}
