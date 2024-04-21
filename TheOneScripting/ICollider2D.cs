using System;

public class ICollider2D : IComponent
{
    public float radius
    {
        get
        {
            return InternalCalls.GetColliderRadius(containerGOptr);
        }
        set
        {
            InternalCalls.SetColliderRadius(containerGOptr, ref value);
        }
    }
    public Vector2 size
    {
        get
        {
            return InternalCalls.GetColliderBoxSize(containerGOptr);
        }
        set
        {
            InternalCalls.SetColliderBoxSize(containerGOptr, ref value);
        }
    }

    public ICollider2D() : base() { }
    public ICollider2D(IntPtr gameObjectRef) : base(gameObjectRef) { }
}