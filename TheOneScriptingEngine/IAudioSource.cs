using System;
using System.Runtime.CompilerServices;
using static AudioManager;

public class IAudioSource : IComponent
{
    public IAudioSource() : base() { }
    public IAudioSource(IntPtr GOptr) : base(GOptr) { }

    public void PlayAudio(EventIDs audio)
    {
        AudioManager.PlaySource(containerGOptr, audio);
    }

    public void StopAudio(EventIDs audio)
    {
        AudioManager.StopSource(containerGOptr, audio);
    }
}
