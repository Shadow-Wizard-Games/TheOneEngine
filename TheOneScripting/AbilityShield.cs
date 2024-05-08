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
        cooldownTime = 8.0f;
    }
        
    public override void Update()
    {
        switch (state)
        {
            case AbilityState.CHARGING:
                ChargeAbility();
                break;
            case AbilityState.READY:
                if (Input.GetKeyboardButton(Input.KeyboardCode.W)) // change input
                {
                    Activated();
                    state = AbilityState.ACTIVE;
                    break;
                }
                // controller input
                break;
            case AbilityState.ACTIVE:
                Debug.Log("Shield active time" + activeTime.ToString("F2"));
                break;
            case AbilityState.COOLDOWN:
                OnCooldown();
                Debug.Log("Shield active time" + cooldownTime.ToString("F2"));
                break;
        }
    }

    public override void ChargeAbility()
    {
        if(player.shieldKillCounter == 2) 
        {
            player.shieldKillCounter = 0;
            state = AbilityState.READY;
        }
    }

    public override void Activated()
    {
        player.shieldIsActive = true;
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
            activeTime = 0.3f;
            state = AbilityState.COOLDOWN;
        }
    }

    public override void OnCooldown()
    {
        if (cooldownTime > 0)
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