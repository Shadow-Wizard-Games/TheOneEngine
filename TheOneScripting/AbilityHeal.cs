using System.Collections;

namespace TheOneScripting
{
    public class AbilityHeal : Ability
    {
        IGameObject playerGO;
        PlayerScript player;

        float healAmount = 60; // in %
        float slowAmount = 40; // in %
        public override void Start()
        {
            name = "Dash";
            playerGO = IGameObject.Find("SK_MainCharacter");
            player = playerGO.GetComponent<PlayerScript>();
        }

        public override void Activate()
        {
            float healing = player.maxLife * healAmount / 100;
            healing += player.life;

            if (healing > player.life)
                player.life = player.maxLife;
            else
                player.life = healing;


        }
    } 
}