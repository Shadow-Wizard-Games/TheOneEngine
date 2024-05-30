internal class ShoulderLaserBehaviour : MonoBehaviour
{
    State SLState;
    PlayerScript player;
    ITransform playerTransform;

    enum State
    {
        IDLE,
        RUN,
        SHOOT,
        RUNSHOOT,
        NOTM4
    }

    public override void Start()
    {
        player = IGameObject.Find("SK_MainCharacter")?.GetComponent<PlayerScript>();
        playerTransform = player.attachedGameObject.GetComponent<ITransform>();

        attachedGameObject.animator.Blend = true;
        attachedGameObject.animator.TransitionTime = 0.1f;

        SLState = State.NOTM4;
    }

    public override void Update()
    {
        attachedGameObject.transform.Position = playerTransform.Position;
        attachedGameObject.transform.Rotation = playerTransform.Rotation;

        ChangeWeaponState();

        switch (SLState)
        {
            case State.IDLE:
                attachedGameObject.animator.Play("Idle");
                break;
            case State.RUN:
                attachedGameObject.animator.Play("Run");
                break;
            case State.SHOOT:
                attachedGameObject.animator.Play("Shoot");
                break;
            case State.RUNSHOOT:
                attachedGameObject.animator.Play("Run and Shoot");
                break;
        }

        attachedGameObject.animator.UpdateAnimation();
    }

    private void ChangeWeaponState()
    {
        if (player.currentWeaponType == PlayerScript.CurrentWeapon.SHOULDERLASER)
        {
            switch (player.currentAction)
            {
                case PlayerScript.CurrentAction.IDLE:
                    SLState = State.IDLE;
                    break;
                case PlayerScript.CurrentAction.RUN:
                    SLState = State.RUN;
                    break;
                case PlayerScript.CurrentAction.SHOOT:
                    SLState = State.SHOOT;
                    break;
                case PlayerScript.CurrentAction.RUNSHOOT:
                    SLState = State.RUNSHOOT;
                    break;
            }
        }
    }
}

