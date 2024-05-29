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

    readonly int damage = 10;

    readonly float slowAmount = 0.25f;
    readonly float impacienteShootingCd = 0.05f;
    readonly float knockbackPotency = -3f;

    public override void Start()
    {
        abilityName = "Impaciente";

        activeTime = 25.0f;
        activeTimeCounter = activeTime;
        cooldownTime = 45.0f;
        cooldownTimeCounter = cooldownTime;

        playerGO = attachedGameObject.parent;
        player = playerGO.GetComponent<PlayerScript>();
    }

    // put update and call the abilityStatUpdate from there or 
    public override void Update()
    {
        switch (state)
        {
            case AbilityState.CHARGING:
                break;
            case AbilityState.READY:
                if (Input.GetKeyboardButton(Input.KeyboardCode.FOUR))
                {
                    Activated();
                    break;
                }
                // controller input
                break;
            case AbilityState.ACTIVE:
                WhileActive();
                Debug.Log("Impaciente active time -> " + activeTimeCounter.ToString("F2"));
                break;
            case AbilityState.COOLDOWN:
                OnCooldown();
                Debug.Log("Impaciente cooldown time -> " + cooldownTimeCounter.ToString("F2"));
                break;
        }
    }

    public void Activated()
    {
        player.currentWeaponType = PlayerScript.CurrentWeapon.IMPACIENTE;

        // rpm
        player.shootingCooldown = impacienteShootingCd;

        float speedReduce = player.baseSpeed * slowAmount;
        player.currentSpeed -= speedReduce;

        player.currentWeaponDamage = damage;

        state = AbilityState.ACTIVE;

        attachedGameObject.source.Play(IAudioSource.AudioEvent.A_LI);

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

            if (Input.GetKeyboardButton(Input.KeyboardCode.FOUR) || player.currentWeaponType != PlayerScript.CurrentWeapon.IMPACIENTE /*|| player.impacienteBulletCounter >= player.impacienteBullets*/)
            {
                // reset stats
                //player.shootingCooldown = player.mp4ShootingCd;
                player.currentSpeed = player.baseSpeed;
                player.currentWeaponType = PlayerScript.CurrentWeapon.M4;
                player.currentWeaponDamage = damage;
                //player.impacienteBulletCounter = 0;

                activeTimeCounter = activeTime;
                state = AbilityState.COOLDOWN;

                Debug.Log("Ability Impaciente on Cooldown");
            }
        }
        else
        {
            // reset stats
            //player.shootingCooldown = player.mp4ShootingCd;
            player.currentSpeed = player.baseSpeed;
            player.currentWeaponType = PlayerScript.CurrentWeapon.M4;
            player.currentWeaponDamage = damage;
            activeTimeCounter = activeTime;
            /*player.impacienteBulletCounter = 0*/
            ;

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
