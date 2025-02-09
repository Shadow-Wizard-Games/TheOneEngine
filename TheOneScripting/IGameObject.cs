﻿using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class IGameObject : IObject
{
    public ITransform transform;
    public IAudioSource source;
    public IAnimator animator;
    public IGameObject parent
    {
        get
        {
            IntPtr foundGOptr = InternalCalls.GetParent(containerGOptr);

            if (foundGOptr == IntPtr.Zero)
            {
                Debug.LogError("Did not find parent to the GameObject");

                return null;
            }

            IGameObject goToReturn = new IGameObject(foundGOptr);

            return goToReturn;
        }
    }


    public IGameObject() : base()
    {
        name = "";

        transform = new ITransform();

        source = new IAudioSource();

        animator = new IAnimator();
    }
    public IGameObject(IntPtr GOref) : base(GOref)
    {
        containerGOptr = GOref;
        name = InternalCalls.GetGameObjectName(containerGOptr);
        transform = new ITransform(GOref);
        source = new IAudioSource(GOref);
        animator = new IAnimator(GOref);
    }

    public void Destroy()
    {
        InternalCalls.DestroyGameObject(containerGOptr);
    }

    public static IGameObject Find(string name)
    {
        IntPtr foundGOptr = InternalCalls.FindGameObject(name);

        if (foundGOptr == IntPtr.Zero)
        {
            Debug.LogError("GameObject with name '" + name + "' not found.");

            return null;
        }

        IGameObject goToReturn = new IGameObject(foundGOptr);

        return goToReturn;
    }

    public IGameObject FindInChildren(string name)
    {
        IntPtr foundGOptr = InternalCalls.FindGameObjectInChildren(containerGOptr, name);

        if (foundGOptr == IntPtr.Zero)
        {
            Debug.LogError("GameObject with name '" + name + "' not found.");

            return null;
        }

        IGameObject goToReturn = new IGameObject(foundGOptr);

        return goToReturn;
    }


    //Used to get any component except scripts
    public TComponent GetComponent<TComponent>() where TComponent : IComponent
    {
        IntPtr component = IntPtr.Zero;

        TComponent componentToReturn = null;

        if (Enum.TryParse(typeof(TComponent).ToString(), out IComponent.ComponentType type))
        {
            component = InternalCalls.ComponentCheck(containerGOptr, (int)type);

            if (component != IntPtr.Zero)
            {

                switch (type)
                {
                    case IComponent.ComponentType.ITransform:
                        componentToReturn = new ITransform(containerGOptr) as TComponent;
                        break;
                    case IComponent.ComponentType.ICamera:
                        componentToReturn = new ICamera(containerGOptr) as TComponent;
                        Debug.LogCheck("The GetType of the class is: " + type.ToString());
                        break;
                    //case IComponent.ComponentType.IMesh:
                    //    componentToReturn = new IMesh(containerGOptr) as TComponent;
                    //    Debug.LogCheck("The GetType of the class is: " + type.ToString());
                    //    break;
                    //case IComponent.ComponentType.ITexture:
                    //    componentToReturn = new ITexture(containerGOptr) as TComponent;
                    //    Debug.LogCheck("The GetType of the class is: " + type.ToString());
                    //    break;
                    case IComponent.ComponentType.ICollider2D:
                        componentToReturn = new ICollider2D(containerGOptr) as TComponent;
                        Debug.LogCheck("The GetType of the class is: " + type.ToString());
                        break;
                    case IComponent.ComponentType.ICanvas:
                        componentToReturn = new ICanvas(containerGOptr) as TComponent;
                        break;
                    //case IComponent.ComponentType.IListener:
                    //    componentToReturn = new IListener(containerGOptr) as TComponent;
                    //    Debug.LogCheck("The GetType of the class is: " + type.ToString());
                    //    break;
                    case IComponent.ComponentType.IAudioSource:
                        componentToReturn = new IAudioSource(containerGOptr) as TComponent;
                        break;
                    case IComponent.ComponentType.IParticleSystem:
                        componentToReturn = new IParticleSystem(containerGOptr) as TComponent;
                        break;
                    case IComponent.ComponentType.IAnimator:
                        componentToReturn = new IAnimator(containerGOptr) as TComponent;
                        break;
                    case IComponent.ComponentType.ILight:
                        componentToReturn = new ILight(containerGOptr) as TComponent;
                        break;
                    default:
                        break;
                }
            }
        }
        else if (typeof(TComponent).IsSubclassOf(typeof(MonoBehaviour)))
        {
            component = InternalCalls.ComponentCheck(containerGOptr, (int)IComponent.ComponentType.MonoBehaviour, typeof(TComponent).ToString());

            if (component != IntPtr.Zero)
            {
                componentToReturn = InternalCalls.GetScript<TComponent>(containerGOptr, typeof(TComponent).ToString());
            }
        }


        if (componentToReturn != null)
        {
            return componentToReturn;
        }
        else
        {
            return new IComponent() as TComponent;
        }
    }
    public void Disable()
    {
        InternalCalls.Disable(containerGOptr);
    }
    public void Enable()
    {
        InternalCalls.Enable(containerGOptr);
    }

    static public void InstanciatePrefab(string name, Vector3 position, Vector3 rotation)
    {
        InternalCalls.CreatePrefab(name, position, rotation);
    }
}