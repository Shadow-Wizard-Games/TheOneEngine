using System.Collections;

public class AbilityAdrenalineRush : Ability
{
    IGameObject playerGO;
    PlayerScript player;

    float healAmount = 0.3f; // in %
    float speedAmount = 0.5f; // in %
    float damageAmount = 0.5f; // in %

    float healthRegenTime = 3.0f;
    float intervalTime = 0.2f;
    float timeSinceLastTick = 0.0f;
    float healingInterval = 0.0f;

    int numÍntervals = 0;

    public override void Start()
    {
        name = "Heal";
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        activeTime = 8.0f;
        cooldownTime = 8.0f;

        numÍntervals = (int)(healthRegenTime / intervalTime);
    }

    public override void Update()
    {
        switch (state)
        {
            case AbilityState.CHARGING:
                break;
            case AbilityState.READY:
                if (Input.GetKeyboardButton(Input.KeyboardCode.V)) // change input
                {
                    Activated();
                    state = AbilityState.ACTIVE;
                    break;
                }
                // controller input
                break;
            case AbilityState.ACTIVE:
                WhileActive();
                Debug.Log("Adrenaline active time --> " + activeTime.ToString("F2"));
                break;
            case AbilityState.COOLDOWN:
                OnCooldown();
                Debug.Log("Adrenaline active time" + cooldownTime.ToString("F2"));
                break;
        }
    }

    public override void Activated()
    {
        // Calculate heal amount
        float totalHeal = player.maxLife * healAmount;
        healingInterval = totalHeal / numÍntervals;

        float speedIncrease = player.baseSpeed * speedAmount;
        player.speed += speedIncrease;

        // increase damage
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
            // reset stats
            player.speed = player.baseSpeed;
            healthRegenTime = 3.0f;

            activeTime = 8.0f;
            state = AbilityState.COOLDOWN;
        }

        if (healthRegenTime > 0)
        {
            // update time
            healthRegenTime -= Time.deltaTime;

            timeSinceLastTick += Time.deltaTime;

            // Tegeneration tick
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

    private void RegenerationTick()
    {

    }
}