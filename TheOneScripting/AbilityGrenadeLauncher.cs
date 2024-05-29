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

    public string abilityName;
    public float activeTime;
    public float activeTimeCounter;
    public float cooldownTime;
    public float cooldownTimeCounter;

    public AbilityState state;

    IGameObject playerGO;
    PlayerScript player;
    
    readonly float range = 200.0f;
    readonly float explosionRadius = 40f;
    Vector3 explosionCenterPos = Vector3.zero;

    readonly float grenadeVelocity = 250f;

    public override void Start()
    {
        abilityName = "GrenadeLauncher";

        cooldownTime = 4.0f;
        cooldownTimeCounter = cooldownTime;

        playerGO = attachedGameObject.parent;
        player = playerGO.GetComponent<PlayerScript>();
    }

    public override void Update()
    {
        switch (state)
        {
            case AbilityState.CHARGING:
                break;
            case AbilityState.READY:
                if (Input.GetKeyboardButton(Input.KeyboardCode.FIVE) && player.currentWeaponType == PlayerScript.CurrentWeapon.M4) // change input
                {
                    Activated();
                    break;
                }
                // controller input
                break;
            case AbilityState.ACTIVE:
                break;
            case AbilityState.COOLDOWN:
                OnCooldown();

                Debug.Log("Dash cooldown time" + cooldownTimeCounter.ToString("F2"));
                break;
        }
    }

    public void Activated()
    {
        state = AbilityState.ACTIVE;

        explosionCenterPos = player.attachedGameObject.transform.Position + player.lastMovementDirection * range;

        Vector3 height = new Vector3(0.0f, 30.0f, 0.0f);
        InternalCalls.InstantiateGrenade(player.attachedGameObject.transform.Position + attachedGameObject.transform.Forward * 13.5f + height, attachedGameObject.transform.Rotation);
        player.grenadeInitialVelocity = player.lastMovementDirection * grenadeVelocity;

        state = AbilityState.COOLDOWN;

        player.attachedGameObject.source.Play(IAudioSource.AudioEvent.A_GL_SHOOT);

        Debug.Log("Ability Grenade Launcher Activated");
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

            Debug.Log("Ability Grenade Launcher Ready");
        }
    }
}
