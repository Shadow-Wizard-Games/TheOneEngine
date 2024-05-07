using System.Collections;

namespace TheOneScripting
{
    public class Ability : MonoBehaviour
    {
        string name;
        float cooldownTime;
        float activeTime;

        public virtual void Activate() { }
    }
}