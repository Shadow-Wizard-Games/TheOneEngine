using System;

public class IParticleSystem : IComponent
{
    public IParticleSystem(IntPtr GOptr) : base(GOptr) { }

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
