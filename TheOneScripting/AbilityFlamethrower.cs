﻿using System;

public class AbilityFlamethrower : Ability
{


    public override void Start()
    {
        name = "Flamethrower";

        activeTime = 10.0f;
        cooldownTime = 20.0f;
    }

    public override void Update()
    {
        switch (state)
        {
            case AbilityState.READY:
                {
                    if (Input.GetKeyboardButton(Input.KeyboardCode.THREE))
                    {
                        Activated();

                        break;
                    }
                    break;
                }
            case AbilityState.ACTIVE:
                {
                    WhileActive();
                    break;
                }
            case AbilityState.COOLDOWN:
                {
                    OnCooldown();
                    break;
                }
        }
    }

    public override void Activated()
    {
        // Set current weapon to the flamethrower
        activeTimeCounter = activeTime;

        attachedGameObject.source.Play(IAudioSource.AudioEvent.A_FT);

        this.state = AbilityState.ACTIVE;
    }

    public override void WhileActive()
    {
        if (activeTimeCounter > 0.0f)
        {
            // update time
            activeTimeCounter -= Time.deltaTime;
        }
        else
        {
            cooldownTimeCounter = cooldownTime;

            this.state = AbilityState.COOLDOWN;
        }
    }

    public override void OnCooldown()
    {
        if (cooldownTime > 0.0f)
        {
            // update time
            cooldownTimeCounter -= Time.deltaTime;
        }
        else
        {
            this.state = AbilityState.READY;
        }
    }
}

