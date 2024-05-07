using System.Collections;

namespace TheOneScripting
{
    public class AbilityHeal : Ability
    {
        IGameObject playerGO;
        PlayerScript player;

        float healAmount = 0.6f; // in %
        float slowAmount = 0.4f; // in %
        public override void Start()
        {
            name = "Heal";
            playerGO = IGameObject.Find("SK_MainCharacter");
            player = playerGO.GetComponent<PlayerScript>();

            activeTime = 1.0f;
            cooldownTime = 8.0f;
        }

        // put update and call the abilityStatUpdate from there or 
        public override void UpdateAbilityState()
        {
            switch (state)
            {
                case AbilityState.READY:
                    if (Input.GetKeyboardButton(Input.KeyboardCode.W)) // change input
                    {
                        state = AbilityState.ACTIVE;    
                        break;
                    }
                    // controller input
                    break;
                case AbilityState.ACTIVE:
                    Activated();
                    break;
                case AbilityState.COOLDOWN:
                    OnCooldown();
                    break;
            }
        }

        public override void Activated()
        {
            if (activeTime > 0)
            {
                // update time
                activeTime -= Time.deltaTime;

                // Calculate heal amount
                float healing = player.maxLife * healAmount;
                healing += player.life;

                // heal
                if (healing > player.life)
                    player.life = player.maxLife;
                else
                    player.life = healing;

                float speedReduce = player.baseSpeed * slowAmount;
                player.speed -= speedReduce;
            }
            else
            {
                activeTime = 1.0f;
                state = AbilityState.COOLDOWN;
            }
        }

        public override void OnCooldown()
        {
            if(cooldownTime > 0)
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
}