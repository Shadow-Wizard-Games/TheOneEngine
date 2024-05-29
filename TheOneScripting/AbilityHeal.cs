using System;

public class AbilityHeal : MonoBehaviour
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

    public readonly float healAmount = 0.6f; // in %
    public readonly float slowAmount = 0.4f; // in %

    public int numHeals;

    public override void Start()
    {
        abilityName = "Bandage";

        activeTime = 1.0f;
        activeTimeCounter = activeTime;
        cooldownTime = 8.0f;
        cooldownTimeCounter = cooldownTime;

        numHeals = 2;

        playerGO = attachedGameObject.parent;
        player = playerGO.GetComponent<PlayerScript>();

        state = AbilityState.READY;
    }

    // put update and call the abilityStatUpdate from there or 
    public override void Update()
    {
        switch (state)
        {
            case AbilityState.ACTIVE:
                
                WhileActive();
                
                break;
            case AbilityState.COOLDOWN:
                
                OnCooldown();
                
                break;
        }
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
            float totalHeal;

            if (abilityName == "Bandage")
            {
                totalHeal = player.maxHP * healAmount;
            }
            else
            {
                totalHeal = player.maxHP * healAmount;
            }

            player.HP += totalHeal;

            if (player.HP > player.maxHP)
                player.HP = player.maxHP;

            numHeals--;

            // reset stats
            player.currentSpeed = player.baseSpeed;

            activeTimeCounter = activeTime;
            state = AbilityState.COOLDOWN;

            Debug.Log("Ability Heal on Cooldown");
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
            cooldownTimeCounter = cooldownTime;

            if (numHeals > 0)
            {
                state = AbilityState.READY;
            }

            Debug.Log("Ability Heal Ready");
        }
    }
}