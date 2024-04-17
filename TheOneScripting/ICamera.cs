using System;
using System.Runtime.CompilerServices;

public class ICamera : IComponent
{
    public enum CameraType
    {
        PERSPECTIVE,
        ORTHOGONAL
    }

    //Accessible Variables
    public double fov
    {
        get
        {
            return InternalCalls.GetFov(containerGOptr);
        }
        set
        {
            InternalCalls.SetFov(containerGOptr, ref value);
        }
    }

    public double aspect
    {
        get
        {
            return InternalCalls.GetAspect(containerGOptr);
        }
        set
        {
            InternalCalls.SetAspect(containerGOptr, ref value);
        }
    }

    public float yaw
    {
        get
        {
            return InternalCalls.GetYaw(containerGOptr);
        }
        set
        {
            InternalCalls.SetYaw(containerGOptr, ref value);
        }
    }

    public float pitch
    {
        get
        {
            return InternalCalls.GetPitch(containerGOptr);
        }
        set
        {
            InternalCalls.SetPitch(containerGOptr, ref value);
        }
    }

    public CameraType cameraType
    {
        get
        {
            return InternalCalls.GetCameraType(containerGOptr);
        }
        set
        {
            InternalCalls.SetCameraType(containerGOptr, ref value);
        }
    }

    public bool primaryCam
    {
        get
        {
            return InternalCalls.GetPrimaryCam(containerGOptr);
        }
        set
        {
            InternalCalls.SetPrimaryCam(containerGOptr, ref value);
        }
    }

    //Constructors
    public ICamera() : base() { }
    public ICamera(IntPtr gameObjectRef) : base(gameObjectRef) { }

    //Accessible Methods

}

