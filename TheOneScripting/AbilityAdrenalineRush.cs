using System.Collections;

public class AbilityAdrenalineRush : MonoBehaviour
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

    public float healAmount = 0.3f; // in %
    public float speedAmount = 0.5f; // in %
    public float damageAmount = 0.5f; // in %

    public float healthRegenTime = 3.0f;
    public float healthRegenTimeCounter = 3.0f;
    public float intervalTime = 0.2f;
    public float timeSinceLastTick = 0.0f;
    public float healingInterval = 0.0f;

    public int numIntervals = 0;

    public override void Start()
    {
        abilityName = "AdrenalineRush";

        activeTime = 8.0f;
        activeTimeCounter = activeTime;
        cooldownTime = 8.0f;
        cooldownTimeCounter = cooldownTime;

        healthRegenTimeCounter = healthRegenTime;

        numIntervals = (int)(healthRegenTime / intervalTime);

        playerGO = attachedGameObject.parent;
        player = playerGO.GetComponent<PlayerScript>();

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

            if (healthRegenTimeCounter > 0)
            {
                // update time
                healthRegenTimeCounter -= Time.deltaTime;

                timeSinceLastTick += Time.deltaTime;

                // Regeneration tick
                if (timeSinceLastTick >= intervalTime)
                {
                    if ((gameManager.health + healingInterval) < gameManager.GetMaxHealth())
                        gameManager.health += healingInterval;
                    else
                        gameManager.health = gameManager.GetMaxHealth();

                    timeSinceLastTick = 0.0f;
                }
            }
        }
        else
        {
            // reset stats
            player.currentSpeed = gameManager.GetSpeed();
            healthRegenTimeCounter = healthRegenTime;

            player.damageIncrease = 0.0f;

            activeTimeCounter = activeTime;
            state = AbilityState.COOLDOWN;

            Debug.Log("Resetted player stats");
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
        }
    }
}