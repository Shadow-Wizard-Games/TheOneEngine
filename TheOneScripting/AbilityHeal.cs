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
            name = "Dash";
            playerGO = IGameObject.Find("SK_MainCharacter");
            player = playerGO.GetComponent<PlayerScript>();
        }

        public override void Activate()
        {
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
    } 
}