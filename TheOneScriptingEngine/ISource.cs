using System;
using System.Runtime.CompilerServices;
using static AudioManager;

public class ISource : IComponent
{
    public ISource() : base() { }
    public ISource(IntPtr gameObjectRef) : base(gameObjectRef) { }

    public void PlayAudio(EventIDs audio)
    {
        AudioManager.PlaySource(containerGOptr, audio);
    }

    public void StopAudio(EventIDs audio)
    {
        AudioManager.StopSource(containerGOptr, audio);
    }
}
