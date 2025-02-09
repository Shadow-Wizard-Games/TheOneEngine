﻿using System.Security.AccessControl;

public class AbilityFlamethrower : MonoBehaviour
{
    public enum AbilityState
    {
        CHARGING,
        READY,
        ACTIVE,
        COOLDOWN,
    }

    public Item_FlameThrower FlamethrowerItem;

    public string abilityName;
    public float activeTime;
    public float cooldownTime;
    public float activeTimeCounter;
    public float cooldownTimeCounter;

    public uint damage;
    public float slowAmount;

    public AbilityState state;

    public override void Start()
    {
        FlamethrowerItem = new Item_FlameThrower();

        name = FlamethrowerItem.name;

        activeTime = FlamethrowerItem.activeTime;
        activeTimeCounter = activeTime;
        cooldownTime = FlamethrowerItem.cooldownTime;
        cooldownTimeCounter = cooldownTime;

        damage = FlamethrowerItem.damage;

        state = AbilityState.READY;
    }

    public override void Update()
    {
        switch (state)
        {
            case AbilityState.READY:

                if (Input.GetKeyboardButton(Input.KeyboardCode.THREE))
                {
                    Activated();

                    break;
                }
                break;

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
        // Set current weapon to the flamethrower
        activeTimeCounter = activeTime;

        attachedGameObject.source.Play(IAudioSource.AudioEvent.A_FT);

        state = AbilityState.ACTIVE;
    }

    public void WhileActive()
    {
        if (activeTimeCounter > 0.0f)
        {
            // update time
            activeTimeCounter -= Time.deltaTime;
        }
        else
        {
            cooldownTimeCounter = cooldownTime;
        }
    }

    public void OnCooldown()
    {
        if (cooldownTime > 0.0f)
        {
            // update time
            cooldownTimeCounter -= Time.deltaTime;

            Debug.Log("Flamethrower on cooldown");
        }
        else
        {
            state = AbilityState.READY;
        }
    }
}

