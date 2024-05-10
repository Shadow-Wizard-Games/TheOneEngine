using System;

public class AbilityDash : Ability
{
    IGameObject playerGO;
    PlayerScript player;

    float dashSpeed = 50.0f;

    Vector3 moveDir = Vector3.zero;
    
    public override void Start()
    {
        name = "Dash";
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        activeTime = 3f;
        cooldownTime = 4.0f;
    }

    public override void Update()
    {
        switch (state)
        {
            case AbilityState.CHARGING:
                break;
            case AbilityState.READY:
                if (Input.GetKeyboardButton(Input.KeyboardCode.SPACEBAR)) // change input
                {
                    Activated();
                    state = AbilityState.ACTIVE;
                    break;
                }
                // controller input
                break;
            case AbilityState.ACTIVE:
                WhileActive();
                Debug.Log("Dash active time" + activeTime.ToString("F2"));
                break;
            case AbilityState.COOLDOWN:
                OnCooldown();
                Debug.Log("Dash active time" + cooldownTime.ToString("F2"));
                break;
        }
    }

    public override void Activated()
    {
        player.isDashing = true;
        player.speed += dashSpeed;
        moveDir = player.movementDirection + Vector3.zero + Vector3.right + Vector3.forward;
        //attachedGameObject.transform.Translate(movementDirection * movementMagnitude * currentSpeed * Time.deltaTime);

    }

    public override void WhileActive()
    {
        if (activeTime > 0)
        {
            // update time
            activeTime -= Time.deltaTime;
            player.attachedGameObject.transform.Translate(moveDir * player.movementMagnitude * player.speed * Time.deltaTime);
        }
        else
        {
            player.speed = player.baseSpeed;
            player.isDashing = false;
            activeTime = 3f;
            state = AbilityState.COOLDOWN;
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