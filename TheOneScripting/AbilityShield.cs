using System;

public class AbilityShield : Ability
{
    IGameObject playerGO;
    PlayerScript player;

    int killsToUnlock = 2;


    public override void Start()
    {
        name = "Shield";
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        state = AbilityState.CHARGING;

        activeTime = 6.0f;
        activeTimeCounter = activeTime;
        cooldownTime = 3.0f;
        cooldownTimeCounter = cooldownTime;
    }
        
    public override void Update()
    {
        switch (state)
        {
            case AbilityState.CHARGING:
                ChargeAbility();
                break;
            case AbilityState.READY:
                if (Input.GetKeyboardButton(Input.KeyboardCode.TWO)) // change input
                {
                    Activated();
                    break;
                }
                // controller input
                break;
            case AbilityState.ACTIVE:
                Debug.Log("Shield active time" + activeTimeCounter.ToString("F2"));
                WhileActive();
                break;
            case AbilityState.COOLDOWN:
                OnCooldown();
                Debug.Log("Shield cooldown time" + cooldownTimeCounter.ToString("F2"));
                break;
        }
    }

    public override void ChargeAbility()
    {
        if(player.shieldKillCounter == 2) 
        {
            player.shieldKillCounter = 0;
            state = AbilityState.READY;

            Debug.Log("Ability Shield Ready");
        }
    }

    public override void Activated()
    {
        player.shieldIsActive = true;

        state = AbilityState.ACTIVE;

        Debug.Log("Ability Shield Activated");
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
            activeTimeCounter = activeTime;
            state = AbilityState.COOLDOWN;

            Debug.Log("Ability Shield on Cooldown");
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
            state = AbilityState.CHARGING;

            Debug.Log("Ability Shield Charging");
        }
    }
}