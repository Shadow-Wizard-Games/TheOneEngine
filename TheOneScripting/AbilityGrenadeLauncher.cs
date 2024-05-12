using System;

public class AbilityGrenadeLauncher : Ability
{
    IGameObject playerGO;
    PlayerScript player;

    float range = 120.0f;
    float explosionRadius = 40f;
    Vector3 explosionCenterPos = Vector3.zero;

    float grenadeVelocity = 200f;

    public override void Start()
    {
        abilityName = "GrenadeLauncher";
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        cooldownTime = 4.0f;
        cooldownTimeCounter = cooldownTime;
    }

    public override void Update()
    {
        switch (state)
        {
            case AbilityState.CHARGING:
                break;
            case AbilityState.READY:
                if (Input.GetKeyboardButton(Input.KeyboardCode.FIVE)) // change input
                {
                    Activated();
                    break;
                }
                // controller input
                break;
            case AbilityState.ACTIVE:
                WhileActive();
                break;
            case AbilityState.COOLDOWN:
                OnCooldown();
                Debug.Log("Dash cooldown time" + cooldownTimeCounter.ToString("F2"));
                break;
        }
    }

    public override void Activated()
    {
        state = AbilityState.ACTIVE;

        Debug.Log("Ability Grenade Launcher Activated");
    }

    public override void WhileActive()
    {
        explosionCenterPos = player.attachedGameObject.transform.position + player.lastMovementDirection * range;

        if (Input.GetKeyboardButton(Input.KeyboardCode.FIVE))
        {
            Vector3 height = new Vector3(0.0f, 30.0f, 0.0f);
            InternalCalls.InstantiateGrenade(attachedGameObject.transform.position + attachedGameObject.transform.forward * 13.5f + height, attachedGameObject.transform.rotation);
            player.grenadeInitialVelocity = player.lastMovementDirection * grenadeVelocity;

            state = AbilityState.COOLDOWN;
        }

        Debug.DrawWireCircle(player.attachedGameObject.transform.position + Vector3.up * 4, range, new Vector3(0.0f, 0.3f, 1.0f));
        Debug.DrawWireCircle(explosionCenterPos + Vector3.up * 4, explosionRadius, new Vector3(1.0f, 0.4f, 0.0f));
    }

    public override void OnCooldown()
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

            Debug.Log("Ability Grenade Launcher Ready");
        }
    }
}
