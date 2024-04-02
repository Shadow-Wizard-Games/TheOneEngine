using System;
using System.Runtime.CompilerServices;
using static AudioManager;

public class ISource : IComponent
{
    public ISource(IntPtr GOptr) : base(GOptr) { }

    public void PlayAudio(EventIDs audio)
    {
        AudioManager.PlaySource(containerGOptr, audio);
    }

    public void StopAudio(EventIDs audio)
    {
        AudioManager.StopSource(containerGOptr, audio);
    }
}
