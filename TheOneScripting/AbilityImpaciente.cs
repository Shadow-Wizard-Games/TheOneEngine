public class AbilityImpaciente : MonoBehaviour
{
    public enum AbilityState
    {
        CHARGING,
        READY,
        ACTIVE,
        COOLDOWN,
    }

    Item_Impaciente ImpacienteItem;

    public string abilityName;

    public float activeTime;
    public float cooldownTime;
    public float activeTimeCounter;
    public float cooldownTimeCounter;

    public AbilityState state;

    IGameObject playerGO;
    PlayerScript player;

    GameManager gameManager;

    public uint damage;

    public float slowAmount;
    public float fireRate;
    public float knockbackPotency;

    public int ammo = 100;

    public override void Start()
    {
        ImpacienteItem = new Item_Impaciente();

        abilityName = ImpacienteItem.name;

        damage = 10;
        fireRate = 0.05f;

        activeTime = 25.0f;
        cooldownTime = 45.0f;

        slowAmount = 0.25f;
        knockbackPotency = -3.0f;

        activeTimeCounter = activeTime;
        cooldownTimeCounter = cooldownTime;

        playerGO = attachedGameObject.parent;
        player = playerGO.GetComponent<PlayerScript>();

        gameManager = IGameObject.Find("GameManager").GetComponent<GameManager>();

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
        float speedReduce = gameManager.GetSpeed() * slowAmount;
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
                player.currentSpeed = gameManager.GetSpeed();
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
            player.currentSpeed = gameManager.GetSpeed();
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
