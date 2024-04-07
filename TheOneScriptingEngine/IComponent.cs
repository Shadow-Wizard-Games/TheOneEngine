using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class IComponent : IObject
{
    public enum ComponentType
    {
        ITransform,
        ICamera,
        IMesh,
        ITexture,
        MonoBehaviour,
        ICollider2D,
        ICanvas,
        IListener,
        ISource,
        IParticleSystem,
        Unknown
    }

   public IComponent() : base() { }
   public IComponent(IntPtr gameObjectRef) : base(gameObjectRef) { }
}
