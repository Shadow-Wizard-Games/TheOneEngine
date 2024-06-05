using System;

public class AbilityDash : MonoBehaviour
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

    GameManager gameManager;

    readonly float rollPotency = 2.0f;
    readonly float dashPotency = 3.0f;


    public override void Start()
    {
        abilityName = "Roll";

        activeTime = 0.3f;
        activeTimeCounter = activeTime;
        cooldownTime = 4.0f;
        cooldownTimeCounter = cooldownTime;

        playerGO = attachedGameObject.parent;
        player = playerGO?.GetComponent<PlayerScript>();

        gameManager = IGameObject.Find("GameManager").GetComponent<GameManager>();

        state = AbilityState.READY;
    }

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
            if (abilityName == "Roll")
            {
                playerGO.transform.Translate(player.lastMovementDirection * rollPotency * gameManager.GetSpeed() * Time.deltaTime);
            }
            else
            {
                playerGO.transform.Translate(player.lastMovementDirection * dashPotency * gameManager.GetSpeed() * Time.deltaTime);
            }
        }
        else
        {
            activeTimeCounter = activeTime;

            state = AbilityState.COOLDOWN;

            Debug.Log("Ability " + abilityName + " on Cooldown");
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
            state = AbilityState.READY;

            Debug.Log("Ability " + abilityName + " Ready");
        }
    }
}