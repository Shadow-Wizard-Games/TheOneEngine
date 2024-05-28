using System.Collections;

public class AbilityAdrenalineRush : MonoBehaviour
{
    PlayerScript player;

    public enum AbilityState
    {
        CHARGING,
        READY,
        ACTIVE,
        COOLDOWN,
    }
    public AbilityState state;
    public string abilityName;
    public float activeTime;
    public float activeTimeCounter;
    public float cooldownTime;
    public float cooldownTimeCounter;

    readonly float healAmount = 0.3f; // in %
    readonly float speedAmount = 0.5f; // in %
    readonly float damageAmount = 0.5f; // in %

    readonly float healthRegenTime = 3.0f;
    float healthRegenTimeCounter = 3.0f;
    readonly float intervalTime = 0.2f;
    float timeSinceLastTick = 0.0f;
    float healingInterval = 0.0f;

    int numIntervals = 0;

    public override void Start()
    {
        abilityName = "AdrenalineRush";

        activeTime = 8.0f;
        activeTimeCounter = activeTime;
        cooldownTime = 8.0f;
        cooldownTimeCounter = cooldownTime;

        healthRegenTimeCounter = healthRegenTime;

        numIntervals = (int)(healthRegenTime / intervalTime);

        player = attachedGameObject.parent?.GetComponent<PlayerScript>();
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

    public void Activated()
    {
        player.currentAction = PlayerScript.CurrentAction.ADRENALINERUSH;

        // Calculate heal amount
        float totalHeal = player.maxLife * healAmount;
        healingInterval = totalHeal / numIntervals;

        float speedIncrease = player.baseSpeed * speedAmount;
        player.speed += speedIncrease;

        // increase damage
        player.damageIncrease = player.currentWeaponDamage * damageAmount;

        state = AbilityState.ACTIVE;

        Debug.Log("Ability AdrenalineRush Activated");
    }

    public void WhileActive()
    {
        if (activeTimeCounter > 0)
        {
            // update time
            activeTimeCounter -= Time.deltaTime;

            // update increase 
            player.damageIncrease = player.currentWeaponDamage * damageAmount;
        }
        else
        {
            // reset stats
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

            Debug.Log("Ability AdrenalineRush Ready");
        }
    }
}