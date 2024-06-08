public class AbilityImpaciente : MonoBehaviour
{
    public enum AbilityState
    {
        CHARGING,
        READY,
        ACTIVE,
        COOLDOWN,
    }

    public Item_Impaciente ImpacienteItem;

    public string abilityName;

    public float activeTime;
    public float cooldownTime;
    public float activeTimeCounter;
    public float cooldownTimeCounter;

    public AbilityState state;

    IGameObject playerGO;
    PlayerScript player;

    public int damage;

    public float speedModification = 0;
    public float slowAmount;
    public float fireRate;
    public float knockbackPotency;

    public int ammo = 100;

    public float waitToReset = 0.5f;

    public override void Start()
    {
        managers.Start();

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
        // being used dont eliminate
        speedModification = managers.gameManager.GetSpeed() * -slowAmount;
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
                player.attachedGameObject.transform.Translate(player.attachedGameObject.transform.Forward * knockbackPotency * Time.deltaTime);
            
            if (Input.GetKeyboardButton(Input.KeyboardCode.FOUR) && activeTimeCounter < activeTime - waitToReset)
            {
                // reset stats
                //player.shootingCooldown = player.mp4ShootingCd;
                speedModification = 0;
                if(player.currentSkillSet == PlayerScript.SkillSet.M4A1SET)
                    player.currentWeaponType = PlayerScript.CurrentWeapon.M4;
                else if(player.currentSkillSet == PlayerScript.SkillSet.SHOULDERLASERSET)
                    player.currentWeaponType = PlayerScript.CurrentWeapon.SHOULDERLASER;

                player.currentWeaponDamage = damage;

                activeTimeCounter = activeTime;
                state = AbilityState.COOLDOWN;

                player.hudScript.SetPainlessOnCD();

                player.ImpacienteGO.Disable();

                Debug.Log("Ability Impaciente on Cooldown");
            }
        }
        else
        {
            // reset stats
            speedModification = 0;
            player.currentWeaponType = PlayerScript.CurrentWeapon.M4;
            player.currentWeaponDamage = damage;
            activeTimeCounter = activeTime;

            state = AbilityState.COOLDOWN;

            player.ImpacienteGO.Disable();

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
