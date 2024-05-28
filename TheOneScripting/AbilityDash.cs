using System;

public class AbilityDash : Ability
{
    IGameObject playerGO;
    PlayerScript player;

    readonly float rollPotency = 2.0f;
    readonly float dashPotency = 3.0f;

    public override void Start()
    {
        abilityName = "Roll";
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
                if (Input.GetKeyboardButton(Input.KeyboardCode.LSHIFT) || Input.GetControllerButton(Input.ControllerButtonCode.A)) // change input
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
        player.currentAction = PlayerScript.CurrentAction.DASH;

        state = AbilityState.ACTIVE;

        if (player.dashAbilityName == "Roll")
        {
            attachedGameObject.source.Play(IAudioSource.AudioEvent.P_ROLL);
        }
        else
        {
            attachedGameObject.source.Play(IAudioSource.AudioEvent.P_DASH);
        }

        Debug.Log("Ability " + player.dashAbilityName + " Activated");
    }

    public override void WhileActive()
    {
        if (activeTimeCounter > 0)
        {
            // update time
            activeTimeCounter -= Time.deltaTime;
            if (player.dashAbilityName == "Roll")
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