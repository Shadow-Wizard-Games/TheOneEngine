using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class IGameObject : IObject
{
    public ITransform transform;
    public ISource source;

    public IGameObject() : base()
    {
        name = "";

        transform = new ITransform();

        source = new ISource();
    }
    public IGameObject(IntPtr GOref) : base(GOref)
    {
        containerGOptr = GOref;
        name = InternalCalls.GetGameObjectName(containerGOptr);
        transform = new ITransform(GOref);

        source = new ISource(GOref);
    }

    public void Destroy()
    {
        InternalCalls.DestroyGameObject(containerGOptr);
    }

    public static IGameObject Find(string name)
    {
        IntPtr foundGOptr = InternalCalls.FindGameObject(name);

        if(foundGOptr == IntPtr.Zero)
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
                    //case IComponent.ComponentType.ICamera:
                    //    componentToReturn = new ICamera(containerGOptr) as TComponent;
                    //    Debug.LogCheck("The GetType of the class is: " + type.ToString());
                    //    break;
                    //case IComponent.ComponentType.IMesh:
                    //    componentToReturn = new IMesh(containerGOptr) as TComponent;
                    //    Debug.LogCheck("The GetType of the class is: " + type.ToString());
                    //    break;
                    //case IComponent.ComponentType.ITexture:
                    //    componentToReturn = new ITexture(containerGOptr) as TComponent;
                    //    Debug.LogCheck("The GetType of the class is: " + type.ToString());
                    //    break;
                    //case IComponent.ComponentType.ICollider2D:
                    //    componentToReturn = new ICollider2D(containerGOptr) as TComponent;
                    //    Debug.LogCheck("The GetType of the class is: " + type.ToString());
                    //    break;
                    case IComponent.ComponentType.ICanvas:
                        componentToReturn = new ICanvas(containerGOptr) as TComponent;
                        break;
                    //case IComponent.ComponentType.IListener:
                    //    componentToReturn = new IListener(containerGOptr) as TComponent;
                    //    Debug.LogCheck("The GetType of the class is: " + type.ToString());
                    //    break;
                    case IComponent.ComponentType.ISource:
                        componentToReturn = new ISource(containerGOptr) as TComponent;
                        break;
                    case IComponent.ComponentType.IParticleSystem:
                        componentToReturn = new IParticleSystem(containerGOptr) as TComponent;
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
}