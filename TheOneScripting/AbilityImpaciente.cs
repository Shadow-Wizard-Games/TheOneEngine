public class AbilityImpaciente : MonoBehaviour
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

    public int damage = 10;

    public float slowAmount = 0.25f;
    public float impacienteShootingCd = 0.05f;
    public float knockbackPotency = -3f;

    public int ammo = 100;

    public override void Start()
    {
        abilityName = "Impaciente";

        activeTime = 25.0f;
        activeTimeCounter = activeTime;
        cooldownTime = 5.0f;
        cooldownTimeCounter = cooldownTime;

        playerGO = attachedGameObject.parent;
        player = playerGO.GetComponent<PlayerScript>();

        state = AbilityState.READY;
    }

    // put update and call the abilityStatUpdate from there or 
    public override void Update()
    {
        switch (state)
        {
            case AbilityState.ACTIVE:

                WhileActive();

                break;
            case AbilityState.COOLDOWN:

                OnCooldown();

                break;
        }
    }

    public void Activated()
    {
        // rpm
        player.shootingCooldown = impacienteShootingCd;

        float speedReduce = player.baseSpeed * slowAmount;
        player.currentSpeed -= speedReduce;

        player.currentWeaponDamage = damage;

        state = AbilityState.ACTIVE;

        //attachedGameObject.source.Play(IAudioSource.AudioEvent.A_LI);

        Debug.Log("Ability Impaciente Activated");
    }

    public void WhileActive()
    {
        if (activeTimeCounter > 0)
        {
            // update time
            activeTimeCounter -= Time.deltaTime;

            if (player.currentAction == PlayerScript.CurrentAction.SHOOT)
                player.attachedGameObject.transform.Translate(player.lastMovementDirection * knockbackPotency * Time.deltaTime);

            if (Input.GetKeyboardButton(Input.KeyboardCode.THREE))
            {
                // reset stats
                //player.shootingCooldown = player.mp4ShootingCd;
                player.currentSpeed = player.baseSpeed;
                player.currentWeaponType = PlayerScript.CurrentWeapon.M4;
                player.currentWeaponDamage = damage;

                activeTimeCounter = activeTime;
                state = AbilityState.COOLDOWN;

                Debug.Log("Ability Impaciente on Cooldown");
            }
        }
        else
        {
            // reset stats
            player.currentSpeed = player.baseSpeed;
            player.currentWeaponType = PlayerScript.CurrentWeapon.M4;
            player.currentWeaponDamage = damage;
            activeTimeCounter = activeTime;

            state = AbilityState.COOLDOWN;

            Debug.Log("Ability Impaciente on Cooldown");
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

            Debug.Log("Ability Impaciente Ready");
        }
    }
}
