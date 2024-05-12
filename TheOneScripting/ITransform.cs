using System;
using System.Runtime.CompilerServices;

public class ITransform : IComponent
{
    public Vector3 position
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

    public Vector3 rotation
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

    public Vector3 forward
    {
        get
        {
            return InternalCalls.GetTransformForward(containerGOptr);
        }
    }
    public Vector3 right
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

        Vector3 finalPos = position + increment;
        InternalCalls.Translate(containerGOptr, ref finalPos);
    }

    public void SetPosition(Vector3 setPos)
    {
        position = setPos;
    }

    public void Rotate(Vector3 increment)
    {
       InternalCalls.Rotate(containerGOptr, ref increment);
    }


    public void LookAt2D(Vector3 targetPosition)
    {
        Vector3 directorVector = targetPosition - position;

        if (directorVector == Vector3.zero) return;

        float targetAngle = (float)Math.Atan2(directorVector.x, directorVector.z);

        rotation = new Vector3(0.0f, targetAngle, 0.0f);
    }

    public void CamLookAt(Vector3 targetPosition)
    {
        Vector3 directorVector = targetPosition - position;

        if (directorVector == Vector3.zero) return;

        float distanceXZ = (float)Math.Sqrt(directorVector.x * directorVector.x + directorVector.z * directorVector.z);

        float angleY = -(float)Math.Atan2(directorVector.x, directorVector.z);
        float angleX =  (float)Math.Atan2(directorVector.y, distanceXZ);

        rotation = Vector3.zero;
        Rotate(new Vector3(angleX * 180.0f / (float)Math.PI, angleY * 180.0f / (float)Math.PI, 0.0f));
    }
}