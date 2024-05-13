using System;

public class WhiteXenomorphBehaviour : MonoBehaviour
{
    enum States
    {
        Idle,
        Attack,
        Chase,
        Patrol,
        Dead
    }

    private enum WhiteXenomorphAttacks
    {
        None,
        ClawAttack,
        TailTrip
    }

    IGameObject playerGO;
    Vector3 directorVector;
    float playerDistance;

    // White Xenomorph parameters
    float life = 200.0f;
    float movementSpeed = 40.0f * 3;
    States currentState = States.Idle;
    States lastState = States.Idle;
    WhiteXenomorphAttacks currentAttack = WhiteXenomorphAttacks.None;
    Vector3 initialPos;

    // Patrol
    float patrolRange = 100;
    float patrolSpeed = 20.0f;
    float roundProgress = 0.0f; //Do not modify
    bool goingToRoundPos = false;

    // Ranges
    const float farRangeThreshold = 50.0f * 3;
    const float enemyDetectedRange = 35.0f * 3;
    const float maxAttackRange = 90.0f;
    const float maxChasingRange = 180.0f;

    // Flags
    bool detected = false;
    bool isClose = false;

    // Timers
    float attackTimer = 0.0f;
    const float attackCooldown = 2.0f;

    PlayerScript player;
    GameManager gameManager;

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        gameManager = IGameObject.Find("GameManager").GetComponent<GameManager>();

        attachedGameObject.animator.Play("Walk");
        attachedGameObject.animator.blend = false;
        attachedGameObject.animator.transitionTime = 0.0f;
    }

    public override void Update()
    {
        attachedGameObject.animator.UpdateAnimation();

        if (currentState == States.Dead) return;

        if (attachedGameObject.transform.ComponentCheck())
        {
            //Set the director vector and distance to the player
            directorVector = (playerGO.transform.position - attachedGameObject.transform.position).Normalize();
            playerDistance = Vector3.Distance(playerGO.transform.position, attachedGameObject.transform.position);

            UpdateFSMStates();
            DoStateBehaviour();
        }

        DebugDraw();
    }

    void UpdateFSMStates()
    {
        if (life <= 0) { currentState = States.Dead; return; }

        if (!detected && playerDistance < enemyDetectedRange) detected = true;

        if (detected)
        {
            if (playerDistance < maxAttackRange)
            {
                currentState = States.Attack;
            }
            else if (playerDistance > maxAttackRange && playerDistance < maxChasingRange)
            {
                currentState = States.Chase;
            }
            else if (playerDistance > maxChasingRange)
            {
                detected = false;
                currentState = States.Idle;
            }

            if (currentAttack == WhiteXenomorphAttacks.None)
            {
                attackTimer += Time.deltaTime;
            }

            if (currentAttack == WhiteXenomorphAttacks.None && attackTimer >= attackCooldown)
            {
                //Debug.Log("Attempt to attack");
                currentState = States.Attack;
            }
        }
    }

    void DoStateBehaviour()
    {
        switch (currentState)
        {
            case States.Idle:
                return;
            case States.Attack:
                player.isFighting = true;
                attachedGameObject.transform.LookAt2D(playerGO.transform.position);
                ChooseAttack();

                switch (currentAttack)
                {
                    case WhiteXenomorphAttacks.ClawAttack:
                        ClawAttack();
                        break;
                    case WhiteXenomorphAttacks.TailTrip:
                        TailTrip();
                        break;
                    default:
                        break;
                }

                break;
            case States.Chase:
                player.isFighting = true;
                attachedGameObject.transform.LookAt2D(playerGO.transform.position);
                attachedGameObject.transform.Translate(attachedGameObject.transform.forward * movementSpeed * Time.deltaTime);
                break;
            case States.Patrol:
                Patrol();
                break;
            case States.Dead:
                attachedGameObject.animator.Play("Death");
                break;
            default:
                break;
        }
    }

    private void ChooseAttack()
    {
        if (currentAttack == WhiteXenomorphAttacks.None)
        {
            if (isClose)
            {
                currentAttack = WhiteXenomorphAttacks.ClawAttack;
                attachedGameObject.animator.Play("TailAttack");
            }
            else
            {
                currentAttack = WhiteXenomorphAttacks.TailTrip;
                attachedGameObject.animator.Play("Spit");
            }
            //Debug.Log("Chestburster current attack: " + currentAttack);
        }
    }

    private void ClawAttack()
    {
        //InternalCalls.InstantiateBullet(attachedGameObject.transform.position + attachedGameObject.transform.forward * 12.5f, attachedGameObject.transform.rotation);
        attachedGameObject.source.Play(IAudioSource.AudioEvent.E_X_ADULT_SPIT);
        if (attachedGameObject.animator.currentAnimHasFinished)
        {
            ResetState();
        }

    }

    private void TailTrip()
    {
        if (attachedGameObject.animator.currentAnimHasFinished)
        {
            ResetState();
        }
    }

    private void Patrol()
    {
        attachedGameObject.animator.Play("Walk");

        if (currentState != lastState)
        {
            lastState = currentState;
            goingToRoundPos = true;
        }

        roundProgress += Time.deltaTime * patrolSpeed;
        if (roundProgress > 360.0f) roundProgress -= 360.0f;

        Vector3 roundPos = initialPos +
                           Vector3.right * (float)Math.Cos(roundProgress * Math.PI / 180.0f) * patrolRange +
                           Vector3.forward * (float)Math.Sin(roundProgress * Math.PI / 180.0f) * patrolRange;

        attachedGameObject.transform.LookAt2D(roundPos);
        if (!goingToRoundPos)
        {
            MoveTo(roundPos);
        }
        else
        {
            goingToRoundPos = !MoveTo(roundPos);
        }
    }

    private void ResetState()
    {
        attackTimer = 0.0f;
    }

    public void ReduceLife() //temporary function for the hardcoding of collisions
    {
        life -= 10.0f;
    }

    private bool MoveTo(Vector3 targetPosition)
    {
        //Return true if arrived at destination
        if (Vector3.Distance(attachedGameObject.transform.position, targetPosition) < 0.5f) return true;

        Vector3 dirVector = (targetPosition - attachedGameObject.transform.position).Normalize();
        attachedGameObject.transform.Translate(dirVector * movementSpeed * Time.deltaTime);
        //attachedGameObject.source.PlayAudio(AudioManager.EventIDs.E_REBEL_STEP);
        return false;
    }

    private void DebugDraw()
    {
        //Draw debug ranges
        if (gameManager.colliderRender)
        {
            if (!detected)
            {
                Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, enemyDetectedRange, new Vector3(1.0f, 0.8f, 0.0f)); //Yellow
            }
            else
            {
                Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, maxChasingRange, new Vector3(0.9f, 0.0f, 0.9f)); //Purple
                Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, maxAttackRange, new Vector3(0.0f, 0.8f, 1.0f)); //Blue
            }
            Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, farRangeThreshold, new Vector3(1.0f, 0.0f, 1.0f)); //Purple
        }
    }
}