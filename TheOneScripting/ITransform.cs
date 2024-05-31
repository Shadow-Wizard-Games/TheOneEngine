using System;
using System.Runtime.CompilerServices;

public class ITransform : IComponent
{
    public Vector3 Position
    {
        get
        {
            return InternalCalls.GetPosition(containerGOptr);
        }
        set
        {
            InternalCalls.SetPosition(containerGOptr, ref value);
        }
    }

    public Vector3 Rotation
    {
        get
        {
            return InternalCalls.GetRotation(containerGOptr);
        }
        set
        {
            InternalCalls.SetRotation(containerGOptr, ref value);
        }
    }

    public Vector3 Forward
    {
        get
        {
            return InternalCalls.GetTransformForward(containerGOptr);
        }
    }
    public Vector3 Right
    {
        get
        {
            return InternalCalls.GetTransformRight(containerGOptr);
        }
    }

    public ITransform() : base() { }
    public ITransform(IntPtr gameObjectRef) : base(gameObjectRef) { }

    public bool ComponentCheck()
    {
        return InternalCalls.TransformCheck(containerGOptr);
    }
    public void Translate(Vector3 increment)
    {
        //This implementation is temporary, engine Transform.Translate is not working properly.

        Vector3 finalPos = Position + increment;
        InternalCalls.Translate(containerGOptr, ref finalPos);
    }

    public void Rotate(Vector3 increment)
    {
        InternalCalls.Rotate(containerGOptr, ref increment);
    }


    public void LookAt2D(Vector3 targetPosition)
    {
        Vector3 directorVector = targetPosition - Position;

        if (directorVector == Vector3.zero) return;

        float targetAngle = (float)Math.Atan2(directorVector.x, directorVector.z);

        Rotation = new Vector3(0.0f, targetAngle, 0.0f);
    }

    public void CamLookAt(Vector3 targetPosition)
    {
        Vector3 directorVector = targetPosition - Position;

        if (directorVector == Vector3.zero) return;

        float distanceXZ = (float)Math.Sqrt(directorVector.x * directorVector.x + directorVector.z * directorVector.z);

        float angleY = -(float)Math.Atan2(directorVector.x, directorVector.z);
        float angleX = (float)Math.Atan2(directorVector.y, distanceXZ);

        Rotation = Vector3.zero;
        Rotate(new Vector3(angleX * 180.0f / (float)Math.PI, angleY * 180.0f / (float)Math.PI, 0.0f));
    }
}