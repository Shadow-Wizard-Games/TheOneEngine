using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;

public class IGameObject
{
    private IntPtr containerGOptr;

    public string name;
    public bool isActive;
    public ITransform transform;
    public ISource source;

    public IGameObject()
    {
        containerGOptr = InternalCalls.GetGameObjectPtr();

        transform = new ITransform(containerGOptr);

        source = new ISource(containerGOptr);
    }

    public void Destroy()
    {
        InternalCalls.DestroyGameObject(containerGOptr);
    }

    public static IGameObject Find(string name)
    {
        IntPtr foundGOptr = InternalCalls.FindGameObject(name);

        if(foundGOptr == null)
        {
            //Insert error or something

            return new IGameObject();
        }

        IGameObject goToReturn = new IGameObject();

        goToReturn.containerGOptr = foundGOptr;
        goToReturn.transform = new ITransform(foundGOptr);

        return goToReturn;
    }
}