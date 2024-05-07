using System.Collections;

namespace TheOneScripting
{
    public class Ability : MonoBehaviour
    {
        public enum AbilityState
        {
            READY,
            ACTIVE,
            COOLDOWN,
        }

        public string name;
        public float activeTime;
        public float cooldownTime;

        public AbilityState state = AbilityState.READY;

        public virtual void UpdateAbilityState() { }
        public virtual void Activated() { }
        public virtual void OnCooldown() { }
    }
}