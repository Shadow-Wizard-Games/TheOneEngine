using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

//Base Class for Components and GameObjects to be linked in some way
public class IObject
{
    protected IntPtr containerGOptr;

    public string name
    {
        get
        {
            return InternalCalls.GetGameObjectName(containerGOptr);
        }
        set
        {
            //InternallCalls.SetGameObjectName(containerGOptr, ref value);
        }
    }

    public bool isActive;

    public IObject()
    {
        containerGOptr = InternalCalls.GetGameObjectPtr();
    }

    public IObject(IntPtr gameObjectRef)
    {
        containerGOptr = gameObjectRef;
    }
}