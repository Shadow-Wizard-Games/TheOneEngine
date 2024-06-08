using System;

public class AbilityShield : MonoBehaviour
{
    public enum AbilityState
    {
        CHARGING,
        READY,
        ACTIVE,
        COOLDOWN,
    }

    public string abilityName;
    public float activeTime;
    public float activeTimeCounter;
    public float cooldownTime;
    public float cooldownTimeCounter;

    public AbilityState state;

    IGameObject playerGO;
    PlayerScript player;

    int shieldKillsToCharge;

    public override void Start()
    {
        abilityName = "Shield";

        state = AbilityState.CHARGING;

        activeTime = 6.0f;
        activeTimeCounter = activeTime;
        cooldownTime = 3.0f;
        cooldownTimeCounter = cooldownTime;

        shieldKillsToCharge = 5;

        playerGO = attachedGameObject.parent;
        player = playerGO.GetComponent<PlayerScript>();

        state = AbilityState.READY;
    }

    public override void Update()
    {
        switch (state)
        {
            case AbilityState.CHARGING:
                ChargeAbility();
                break;
            case AbilityState.READY:
                //if (Input.GetKeyboardButton(Input.KeyboardCode.TWO)) // change input
                //{
                //    Activated();
                //    break;
                //}
                // controller input
                break;
            case AbilityState.ACTIVE:
                //Debug.Log("Shield active time" + activeTimeCounter.ToString("F2"));
                WhileActive();
                break;
            case AbilityState.COOLDOWN:
                OnCooldown();

                //Debug.Log("Shield cooldown time" + cooldownTimeCounter.ToString("F2"));
                break;
        }
    }

    public void ChargeAbility()
    {
        if (player.shieldKillCounter == shieldKillsToCharge)
        {
            player.shieldKillCounter = 0;
            state = AbilityState.READY;

        }
        //Debug.Log("Ability Shield Ready " + player.shieldKillCounter);
    }

    public void Activated()
    {
        player.shieldIsActive = true;

        state = AbilityState.ACTIVE;

        attachedGameObject.source.Play(IAudioSource.AudioEvent.A_S_ACTIVATE);

        //Debug.Log("Ability Shield Activated");
    }

    public void WhileActive()
    {
        if (activeTimeCounter > 0)
        {
            // update time
            activeTimeCounter -= Time.deltaTime;
        }
        else
        {
            activeTimeCounter = activeTime;
            player.shieldIsActive = false;
            state = AbilityState.COOLDOWN;

            attachedGameObject.source.Play(IAudioSource.AudioEvent.A_S_DEACTIVATE);

            //Debug.Log("Ability Shield on Cooldown");
        }
    }

    public void OnCooldown()
    {
        if (cooldownTimeCounter > 0)
        {
            // update time
            cooldownTimeCounter -= Time.deltaTime;
        }
        else
        {
            player.shieldKillCounter = shieldKillsToCharge;

            cooldownTimeCounter = cooldownTime;
            state = AbilityState.CHARGING;

            //Debug.Log("Ability Shield Charging");
        }
    }
}