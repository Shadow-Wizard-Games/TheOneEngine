using System;

public class AbilityDash : Ability
{
    IGameObject playerGO;
    PlayerScript player;

    float dashSpeed = 50.0f;
    float rollPotency = 2.0f;
    float dashPotency = 3.0f;

    Vector3 moveDir = Vector3.zero;
    
    public override void Start()
    {
        name = "Roll";
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        activeTime = 0.3f;
        activeTimeCounter = activeTime;
        cooldownTime = 4.0f;
        cooldownTimeCounter = cooldownTime;
    }

    public override void Update()
    {
        switch (state)
        {
            case AbilityState.CHARGING:
                break;
            case AbilityState.READY:
                if (Input.GetKeyboardButton(Input.KeyboardCode.LSHIFT)) // change input
                {
                    Activated();
                    break;
                }
                // controller input
                break;
            case AbilityState.ACTIVE:
                WhileActive();
                Debug.Log("Dash active time" + activeTimeCounter.ToString("F2"));
                break;
            case AbilityState.COOLDOWN:
                OnCooldown();
                Debug.Log("Dash cooldown time" + cooldownTimeCounter.ToString("F2"));
                break;
        }
    }

    public override void Activated()
    {
        player.isDashing = true;

        state = AbilityState.ACTIVE;

        Debug.Log("Ability " + player.dashAbilityName + " Activated");
    }

    public override void WhileActive()
    {
        if (activeTimeCounter > 0)
        {
            // update time
            activeTimeCounter -= Time.deltaTime;
            if(player.dashAbilityName == "Roll")
            {
                player.attachedGameObject.transform.Translate(player.lastMovementDirection * rollPotency * player.baseSpeed * Time.deltaTime);
            }
            else
            {
                player.attachedGameObject.transform.Translate(player.lastMovementDirection * dashPotency * player.baseSpeed * Time.deltaTime);
            }
        }
        else
        {
            player.isDashing = false;
            activeTimeCounter = activeTime;
            state = AbilityState.COOLDOWN;

            Debug.Log("Ability " + player.dashAbilityName + " on Cooldown");
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

            Debug.Log("Ability " + player.dashAbilityName + " Ready");
        }
    }
}