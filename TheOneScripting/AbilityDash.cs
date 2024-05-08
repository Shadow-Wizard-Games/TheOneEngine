using System.Collections;

namespace TheOneScripting
{
    public class AbilityDash : Ability
    {
        IGameObject playerGO;
        PlayerScript player;

        float dashSpeed = 50.0f;
        
        public override void Start()
        {
            name = "Dash";
            playerGO = IGameObject.Find("SK_MainCharacter");
            player = playerGO.GetComponent<PlayerScript>();

            activeTime = 0.3f;
            cooldownTime = 4.0f;
        }

        public override void UpdateAbilityState()
        {
            switch (state)
            {
                case AbilityState.CHARGING:
                    break;
                case AbilityState.READY:
                    if (Input.GetKeyboardButton(Input.KeyboardCode.SPACEBAR)) // change input
                    {
                        state = AbilityState.ACTIVE;
                        break;
                    }
                    // controller input
                    break;
                case AbilityState.ACTIVE:
                    Activated();
                    Debug.Log("Dash active time" + activeTime);
                    break;
                case AbilityState.COOLDOWN:
                    OnCooldown();
                    Debug.Log("Dash active time" + cooldownTime);
                    break;
            }
        }

        public override void Activated()
        {
            if (activeTime > 0)
            {
                // update time
                activeTime -= Time.deltaTime;

                player.speed += dashSpeed;
                //player.attachedGameObject.transform.Translate(attachedGameObject.transform.position);
            }
            else
            {
                activeTime = 0.3f;
                state = AbilityState.READY;
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
    }
}