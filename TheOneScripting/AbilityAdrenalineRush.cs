using System.Collections;

public class AbilityAdrenalineRush : Ability
{
    IGameObject playerGO;
    PlayerScript player;

    float healAmount = 0.3f; // in %
    float speedAmount = 0.5f; // in %
    float damageAmount = 0.5f; // in %

    float healthRegenTime = 3.0f;
    float healthRegenTimeCounter = 3.0f;
    float intervalTime = 0.2f;
    float timeSinceLastTick = 0.0f;
    float healingInterval = 0.0f;

    int numÍntervals = 0;

    public override void Start()
    {
        abilityName = "AdrenalineRush";
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        activeTime = 8.0f;
        activeTimeCounter = activeTime;
        cooldownTime = 8.0f;
        cooldownTimeCounter = cooldownTime;

        healthRegenTimeCounter = healthRegenTime;

        numÍntervals = (int)(healthRegenTime / intervalTime);
    }

    public override void Update()
    {
        switch (state)
        {
            case AbilityState.CHARGING:
                break;
            case AbilityState.READY:
                if (Input.GetKeyboardButton(Input.KeyboardCode.THREE)) // change input
                {
                    Activated();
                    break;
                }
                // controller input
                break;
            case AbilityState.ACTIVE:
                WhileActive();
                Debug.Log("AdrenalineRush active time -> " + activeTimeCounter.ToString("F2"));
                break;
            case AbilityState.COOLDOWN:
                OnCooldown();
                Debug.Log("AdrenalineRush cooldown time -> " + cooldownTimeCounter.ToString("F2"));
                break;
        }
    }

    public override void Activated()
    {
        player.isRushing = true;

        // Calculate heal amount
        float totalHeal = player.maxLife * healAmount;
        healingInterval = totalHeal / numÍntervals;

        float speedIncrease = player.baseSpeed * speedAmount;
        player.speed += speedIncrease;

        // increase damage
        player.damageIncrease = player.currentWeoponDamage * damageAmount;

        state = AbilityState.ACTIVE;

        Debug.Log("Ability AdrenalineRush Activated");
    }

    public override void WhileActive()
    {
        if (activeTimeCounter > 0)
        {
            // update time
            activeTimeCounter -= Time.deltaTime;

            // update increase 
            player.damageIncrease = player.currentWeoponDamage * damageAmount;
        }
        else
        {
            // reset stats
            player.isRushing = false;
            player.speed = player.baseSpeed;
            healthRegenTimeCounter = healthRegenTime;

            player.damageIncrease = 0.0f;

            activeTimeCounter = activeTime;
            state = AbilityState.COOLDOWN;
            
            Debug.Log("Ability AdrenalineRush on Cooldown");
        }

        if (healthRegenTimeCounter > 0)
        {
            // update time
            healthRegenTimeCounter -= Time.deltaTime;

            timeSinceLastTick += Time.deltaTime;

            // Regeneration tick
            if (timeSinceLastTick >= intervalTime)
            {
                if ((player.life + healingInterval) < player.maxLife)
                    player.life += healingInterval;
                else
                    player.life = player.maxLife;

                timeSinceLastTick = 0.0f;
            }
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

            Debug.Log("Ability AdrenalineRush Ready");
        }
    }
}