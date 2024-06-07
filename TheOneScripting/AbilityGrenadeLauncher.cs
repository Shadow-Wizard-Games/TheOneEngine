using System;

public class AbilityGrenadeLauncher : MonoBehaviour
{
    public enum AbilityState
    {
        CHARGING,
        READY,
        ACTIVE,
        COOLDOWN,
    }

    Item_M4A1 M4Item;

    public string abilityName;
    public float cooldownTime;
    public float cooldownTimeCounter;

    public AbilityState state;

    public int damage; 
    public float range = 200.0f;
    public float explosionRadius;
    public Vector3 explosionCenterPos;

    public readonly float grenadeVelocity = 250f;

    public override void Start()
    {
        M4Item = new Item_M4A1();

        abilityName = "Grenade Launcher";

        cooldownTime = M4Item.grenadeCooldownTime;
        cooldownTimeCounter = cooldownTime;

        damage = M4Item.grenadeDamage;
        explosionRadius = M4Item.grenadeExplosionRadius;

        state = AbilityState.READY;
    }

    public override void Update()
    {
        switch (state)
        {
            case AbilityState.COOLDOWN:

                OnCooldown();

                break;
        }
    }

    public void OnCooldown()
    {
        if (cooldownTimeCounter > 0)
        {
            // update time
            cooldownTimeCounter -= Time.deltaTime;
        }
        else
        {
            cooldownTimeCounter = cooldownTime;
            state = AbilityState.READY;
        }
    }
}
