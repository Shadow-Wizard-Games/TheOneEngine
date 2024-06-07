using System;
using System.Runtime.CompilerServices;

public class ILight : IComponent
{
    //Accessible Variables
    public Vector3 color
    {
        get
        {
            return InternalCalls.GetLightColor(containerGOptr);
        }
        set
        {
            InternalCalls.SetLightColor(containerGOptr, ref value);
        }
    }
    
    public float intensity
    {
        get
        {
            return InternalCalls.GetLightIntensity(containerGOptr);
        }
        set
        {
            InternalCalls.SetLightIntensity(containerGOptr, ref value);
        }
    }
    
    public float radius
    {
        get
        {
            return InternalCalls.GetLightRadius(containerGOptr);
        }
        set
        {
            InternalCalls.SetLightRadius(containerGOptr, ref value);
        }
    }

    //Constructors
    public ILight() : base() { }
    public ILight(IntPtr gameObjectRef) : base(gameObjectRef) { }

    public void SwitchOff()
    {
        intensity = 0;
    }

    public void SwitchOn()
    {
        intensity = 6;
    }
}

