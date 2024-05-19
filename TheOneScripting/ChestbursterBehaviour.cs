using System;

public class ChestbursterBehaviour : MonoBehaviour
{
    enum States
    {
        Idle,
        Chase,
        Attack,
        Dead
    }

    private enum ChestbursterAttack
    {
        None,
        TailPunch,
        TailTrip
    }

    IGameObject playerGO;
    //Vector3 directorVector;
    float playerDistance;

    // Chestburster parameters
    float life = 80.0f;
    float movementSpeed = 30.0f * 2;
    States currentState = States.Idle;
    ChestbursterAttack currentAttack = ChestbursterAttack.None;

    // Ranges
    const float detectedRange = 35.0f * 1.5f;
    const float isCloseRange = 60.0f;
    const float maxChasingRange = 150.0f;
    const float maxRangeStopChasing = 25.0f;

    // Flags
    bool isClose = false;
    bool detected = false;

    // Timers
    float attackTimer = 0.0f;
    const float attackCooldown = 4.0f;

    PlayerScript player;
    GameManager gameManager;

    // particles
    IGameObject tailPunchPSGO;
    IGameObject tailTripPSGO;
    IGameObject deathPSGO;

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        gameManager = IGameObject.Find("GameManager").GetComponent<GameManager>();

        attachedGameObject.animator.Play("Move");
        attachedGameObject.animator.blend = false;
        attachedGameObject.animator.transitionTime = 0.0f;

        tailPunchPSGO = attachedGameObject.FindInChildren("TailPunchPS");
        tailTripPSGO = attachedGameObject.FindInChildren("TailTripPS");
        deathPSGO = attachedGameObject.FindInChildren("DeathPS");
    }

    public override void Update()
    {
        attachedGameObject.animator.UpdateAnimation();

        if (currentState == States.Dead) return;

        if (attachedGameObject.transform.ComponentCheck())
        {
            DebugDraw();

            //directorVector = (playerGO.transform.position - attachedGameObject.transform.position).Normalize();
            playerDistance = Vector3.Distance(playerGO.transform.position, attachedGameObject.transform.position);

            UpdateFSM();
            DoStateBehaviour();
        }
    }

    void UpdateFSM()
    {
        if (life <= 0) { currentState = States.Dead; return; }

        if (!detected && playerDistance < detectedRange)
        {
            detected = true;
            currentState = States.Chase;
        }

        if (detected)
        {
            if (playerDistance < isCloseRange && !isClose)
            {
                isClose = true;
                //Debug.Log("Player is now CLOSE");
            }

            if (playerDistance >= isCloseRange && isClose)
            {
                isClose = false;
                //Debug.Log("Player is now FAR");
            }

            if (playerDistance > maxChasingRange && detected)
            {
                detected = false;
                currentState = States.Idle;
                //Debug.Log("Player is NOT DETECTED ANYMORE");
            }

            if (currentAttack == ChestbursterAttack.None)
            {
                attackTimer += Time.deltaTime;
                attachedGameObject.animator.Play("Run");
            }


            if (playerDistance <= maxRangeStopChasing && currentAttack == ChestbursterAttack.None &&
                currentState != States.Idle)
            {
                currentState = States.Idle;
                //Debug.Log("Player is INSIDE maxRangeStopChasing");
            }

            if (playerDistance > maxRangeStopChasing && currentAttack == ChestbursterAttack.None &&
                currentState != States.Chase)
            {
                currentState = States.Chase;
                //Debug.Log("Player is OUTSIDE maxRangeStopChasing");
            }

            if (currentAttack == ChestbursterAttack.None && attackTimer >= attackCooldown)
            {
                currentState = States.Attack;
                //Debug.Log("Chestburster attempt to attack");
            }
        }
    }

    void DoStateBehaviour()
    {
        switch (currentState)
        {
            case States.Idle:
                if (currentAttack == ChestbursterAttack.None && detected)
                    attachedGameObject.transform.LookAt2D(playerGO.transform.position);

                break;
            case States.Chase:
                player.isFighting = true;
                attachedGameObject.transform.Translate(attachedGameObject.transform.forward * movementSpeed * Time.deltaTime);
                attachedGameObject.transform.LookAt2D(playerGO.transform.position);
                break;
            case States.Attack:
                player.isFighting = true;
                attachedGameObject.transform.LookAt2D(playerGO.transform.position);

                ChooseAttack();

                switch (currentAttack)
                {
                    case ChestbursterAttack.TailPunch:
                        TailPunch();
                        //Debug.Log("TAIL PUNCH");
                        break;
                    case ChestbursterAttack.TailTrip:
                        TailTrip();
                        //Debug.Log("TAIL TRIP");
                        break;
                    default:
                        break;
                }
                break;
            case States.Dead:
                attachedGameObject.animator.Play("Dead");
                if (deathPSGO != null) deathPSGO.GetComponent<IParticleSystem>().Play();
                break;
            default:
                break;
        }
    }

    private void ChooseAttack()
    {
        if (currentAttack == ChestbursterAttack.None)
        {
            if (isClose)
            {
                currentAttack = ChestbursterAttack.TailPunch;
                attachedGameObject.animator.Play("TailPunch");
                if (tailPunchPSGO != null) tailPunchPSGO.GetComponent<IParticleSystem>().Play();
            }
            else
            {
                currentAttack = ChestbursterAttack.TailTrip;
                attachedGameObject.animator.Play("TailTrip");
                if (tailTripPSGO != null) tailTripPSGO.GetComponent<IParticleSystem>().Play();
            }
            //Debug.Log("Chestburster current attack: " + currentAttack);
        }
    }

    private void TailPunch()
    {
        //Debug.Log("Attempt to do TailPunch");

        if (attachedGameObject.animator.currentAnimHasFinished)
        {
            ResetState();
        }

    }

    private void TailTrip()
    {
        //Debug.Log("Attempt to do TailTrip");

        if (attachedGameObject.animator.currentAnimHasFinished)
        {
            ResetState();
        }

    }

    private void ResetState()
    {
        //Debug.Log("Reset State");
        attackTimer = 0.0f;
        currentAttack = ChestbursterAttack.None;
        currentState = States.Chase;
        attachedGameObject.animator.Play("Run");
    }

    public void ReduceLife() //temporary function for the hardcoding of collisions
    {
        life -= 10.0f;
    }

    private void DebugDraw()
    {
        //Draw debug ranges
        if (gameManager.colliderRender)
        {
            if (!detected)
            {
                Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, detectedRange, new Vector3(1.0f, 0.8f, 0.0f)); //Yellow
            }
            else
            {
                Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, maxChasingRange, new Vector3(0.9f, 0.0f, 0.9f)); //Purple
                Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, isCloseRange, new Vector3(0.0f, 0.8f, 1.0f)); //Blue
            }
        }
    }
}