using System;

public class IParticleSystem : IComponent
{
    public IParticleSystem() : base() { }
    public IParticleSystem(IntPtr gameObjectRef) : base(gameObjectRef) { }

    public void Play()
    {
        InternalCalls.PlayPS(containerGOptr);
    }

    public void Stop()
    {
        InternalCalls.StopPS(containerGOptr);
    }

    public void Replay()
    {
        InternalCalls.ReplayPS(containerGOptr);
    }

}
