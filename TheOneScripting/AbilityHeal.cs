public class AbilityHeal : Ability
{
    PlayerScript player;

    readonly float healAmount = 0.6f; // in %
    readonly float slowAmount = 0.4f; // in %

    float totalHeal = 0.0f;

    int numHeals;

    public override void Start()
    {
        abilityName = "Heal";

        activeTime = 1.0f;
        activeTimeCounter = activeTime;
        cooldownTime = 8.0f;
        cooldownTimeCounter = cooldownTime;

        numHeals = 2;

        player = attachedGameObject.parent?.GetComponent<PlayerScript>();
    }

    // put update and call the abilityStatUpdate from there or 
    public override void Update()
    {
        switch (state)
        {
            case AbilityState.CHARGING:
                break;
            case AbilityState.READY:
                if (Input.GetKeyboardButton(Input.KeyboardCode.ONE) && numHeals > 0)
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
                break;
        }
    }

    public override void Activated()
    {
        // Calculate heal amount

        if (player.healAbilityName == "Bandage")
        {
            totalHeal = player.maxLife * healAmount;
        }
        else
        {
            totalHeal = player.maxLife * healAmount;
        }
        totalHeal += player.life;

        float speedReduce = player.baseSpeed * slowAmount;
        player.speed -= speedReduce;

        state = AbilityState.ACTIVE;

        Debug.Log("Ability Heal Activated");
    }

    public override void WhileActive()
    {

        if (activeTimeCounter > 0)
        {
            // update time
            activeTimeCounter -= Time.deltaTime;

            // cancel healing
            if (player.currentAction == PlayerScript.CurrentAction.DASH || player.currentWeaponType == PlayerScript.CurrentWeapon.IMPACIENTE)
            {
                player.speed = player.baseSpeed;
                state = AbilityState.READY;
            }
        }
        else
        {
            // heal
            if (totalHeal > player.maxLife)
                player.life = player.maxLife;
            else
                player.life = totalHeal;

            numHeals--;

            // reset stats
            player.speed = player.baseSpeed;

            activeTimeCounter = activeTime;
            state = AbilityState.COOLDOWN;

            Debug.Log("Ability Heal on Cooldown");
        }
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

            Debug.Log("Ability Heal Ready");
        }
    }
}