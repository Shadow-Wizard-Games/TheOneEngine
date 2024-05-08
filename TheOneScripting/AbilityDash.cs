﻿using System;

public class AbilityDash : Ability
{
    IGameObject playerGO;
    PlayerScript player;

    float dashSpeed = 50.0f;
    
    public override void Start()
    {
        name = "Dash";
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        activeTime = 0.3f;
        cooldownTime = 4.0f;
    }

    public override void Update()
    {
        switch (state)
        {
            case AbilityState.CHARGING:
                break;
            case AbilityState.READY:
                if (Input.GetKeyboardButton(Input.KeyboardCode.SPACEBAR)) // change input
                {
                    Activated();
                    state = AbilityState.ACTIVE;
                    break;
                }
                // controller input
                break;
            case AbilityState.ACTIVE:
                WhileActive();
                Debug.Log("Dash active time" + activeTime.ToString("F2"));
                break;
            case AbilityState.COOLDOWN:
                OnCooldown();
                Debug.Log("Dash active time" + cooldownTime.ToString("F2"));
                break;
        }
    }

    public override void Activated()
    {
        player.speed += dashSpeed;
        //player.attachedGameObject.transform.Translate(attachedGameObject.transform.position);
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
            player.speed = player.baseSpeed;

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