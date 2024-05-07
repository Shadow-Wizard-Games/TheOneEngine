using System.Collections;

namespace TheOneScripting
{
    public class AbilityDash : Ability
    {
        IGameObject playerGO;
        PlayerScript player;

        float dashSpeed;
        
        public override void Start()
        {
            name = "Dash";
            playerGO = IGameObject.Find("SK_MainCharacter");
            player = playerGO.GetComponent<PlayerScript>();

            dashSpeed = 50.0f;
        }
        public override void Activate()
        {
            player.speed += dashSpeed;
        }
    }
}