using System;

public class Ability : MonoBehaviour
{
    public enum AbilityState
    {
        CHARGING,
        READY,
        ACTIVE,
        COOLDOWN,
    }

    public string abilityName;
    public float activeTime;
    public float activeTimeCounter;
    public float cooldownTime;
    public float cooldownTimeCounter;

    public AbilityState state = AbilityState.READY;

    public virtual void ChargeAbility() { }
    public virtual void Activated() { }
    public virtual void WhileActive() { }
    public virtual void OnCooldown() { }
}