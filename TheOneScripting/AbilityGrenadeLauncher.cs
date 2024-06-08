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

    Item_GrenadeLauncher grenadeLauncherItem;

    public string abilityName;
    public float cooldownTime;
    public float cooldownTimeCounter;

    public AbilityState state;

    public int damage; 
    public float range = 200.0f;
    public float explosionRadius;
    public Vector3 explosionCenterPos;

    public float grenadeVelocity = 250f;

    public override void Start()
    {
        grenadeLauncherItem = new Item_GrenadeLauncher();

        abilityName = "Grenade Launcher";

        cooldownTime = grenadeLauncherItem.cooldownTime;
        cooldownTimeCounter = cooldownTime;

        damage = grenadeLauncherItem.damage;
        explosionRadius = grenadeLauncherItem.grenadeExplosionRadius;

        grenadeVelocity = grenadeLauncherItem.grenadeVelocity;

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
