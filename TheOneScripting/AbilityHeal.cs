using System;

public class AbilityHeal : Ability
{
    IGameObject playerGO;
    PlayerScript player;

    float healAmount = 0.6f; // in %
    float slowAmount = 0.4f; // in %

    float totalHeal = 0.0f;

    public override void Start()
    {
        name = "Heal";
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        activeTime = 1.0f;
        cooldownTime = 8.0f;
    }

    // put update and call the abilityStatUpdate from there or 
    public override void Update()
    {
        switch (state)
        {
            case AbilityState.CHARGING:
                break;
            case AbilityState.READY:
                if (Input.GetKeyboardButton(Input.KeyboardCode.E)) // change input
                {
                    Activated();
                    state = AbilityState.ACTIVE;    
                    break;
                }
                // controller input
                break;
            case AbilityState.ACTIVE:
                WhileActive();
                Debug.Log("Heal active time --> " + activeTime.ToString("F2"));
                break;
            case AbilityState.COOLDOWN:
                OnCooldown();
                Debug.Log("Heal active time" + cooldownTime.ToString("F2"));
                break;
        }
    }

    public override void Activated()
    {
        // Calculate heal amount
        totalHeal = player.maxLife * healAmount;
        totalHeal += player.life;
        
        float speedReduce = player.baseSpeed * slowAmount;
        player.speed -= speedReduce;
    }

    public override void WhileActive()
    {
        if (activeTime > 0)
        {
            // update time
            activeTime -= Time.deltaTime;
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

            activeTime = 0.3f;
            state = AbilityState.COOLDOWN;
        }
    }

    public override void OnCooldown()
    {
        if(cooldownTime > 0)
        {
            // update time
            cooldownTime -= Time.deltaTime;
        }
        else
        {
            cooldownTime = 8.0f;
            state = AbilityState.READY;
        }
    }
} 