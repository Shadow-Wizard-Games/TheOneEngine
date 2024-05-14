using System;

public class AbilityHeal : Ability
{
    IGameObject playerGO;
    PlayerScript player;

    int numHeals = 0;
    int maxHeals = 3;

    float healAmount = 0.6f; // in %
    float slowAmount = 0.4f; // in %

    float totalHeal = 0.0f;

    public override void Start()
    {
        abilityName = "Heal";
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        activeTime = 1.0f;
        activeTimeCounter = activeTime;
        cooldownTime = 8.0f;
        cooldownTimeCounter = cooldownTime;
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
                Debug.Log("Heal active time -> " + activeTimeCounter.ToString("F2"));
                break;
            case AbilityState.COOLDOWN:
                OnCooldown();
                Debug.Log("Heal cooldown time -> " + activeTimeCounter.ToString("F2"));
                break;
        }
    }

    public override void Activated()
    {
        // Calculate heal amount

        if(player.healAbilityName == "Bandage")
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
        }
        else
        {
            // heal
            if (totalHeal > player.maxLife)
                player.life = player.maxLife;
            else
                player.life = totalHeal;

            // reset stats
            player.speed = player.baseSpeed;

            activeTimeCounter = activeTime;
            state = AbilityState.COOLDOWN;

            Debug.Log("Ability Heal on Cooldown");
        }
    }

    public override void OnCooldown()
    {
        if(cooldownTimeCounter > 0)
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