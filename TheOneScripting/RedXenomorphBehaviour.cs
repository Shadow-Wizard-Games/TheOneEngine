using System;

public class RedXenomorphBehaviour : MonoBehaviour
{
    enum States
    {
        Idle,
        Attack,
        Chase,
        Patrol,
        Dead
    }

    private enum RedXenomorphAttacks
    {
        None,
        SpikeThrow,
        TailStab
    }

    IGameObject playerGO;
    Vector3 directorVector;
    float playerDistance;

    // Red Xenomorph parameters
    float life = 250.0f;
    float movementSpeed = 15.0f * 3;
    States currentState = States.Idle;
    States lastState = States.Idle;
    RedXenomorphAttacks currentAttack = RedXenomorphAttacks.None;
    Vector3 initialPos;

    // Patrol
    float patrolRange = 100;
    float patrolSpeed = 20.0f;
    float roundProgress = 0.0f; //Do not modify
    bool goingToRoundPos = false;

    // Ranges
    const float detectedRange = 35.0f * 3;
    const float isCloseRange = 20.0f * 3;
    const float maxChasingRange = 180.0f;
    const float maxRangeStopChasing = 25.0f;

    // Flags
    bool detected = false;
    bool isClose = false;

    // Timers
    float attackTimer = 0.0f;
    const float attackCooldown = 2.0f;

    PlayerScript player;
    GameManager gameManager;

    // Particles
    IParticleSystem acidSpitPSGO;
    IParticleSystem tailAttackPSGO;
    IParticleSystem deathPSGO;

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();
        initialPos = attachedGameObject.transform.position;

        gameManager = IGameObject.Find("GameManager").GetComponent<GameManager>();

        attachedGameObject.animator.Play("Walk");
        attachedGameObject.animator.Blend = false;
        attachedGameObject.animator.TransitionTime = 0.0f;

        acidSpitPSGO = attachedGameObject.FindInChildren("AcidSpitPS")?.GetComponent<IParticleSystem>();
        tailAttackPSGO = attachedGameObject.FindInChildren("TailAttackPS")?.GetComponent<IParticleSystem>();
        deathPSGO = attachedGameObject.FindInChildren("DeathPS")?.GetComponent<IParticleSystem>();
    }

    public override void Update()
    {
        attachedGameObject.animator.UpdateAnimation();

        if (currentState == States.Dead) return;

        if (attachedGameObject.transform.ComponentCheck())
        {
            DebugDraw();

            //Set the director vector and distance to the player
            directorVector = (playerGO.transform.Position - attachedGameObject.transform.Position).Normalize();
            playerDistance = Vector3.Distance(playerGO.transform.Position, attachedGameObject.transform.Position);

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
            //Debug.Log("Red Xenomorph switched to Chase");
        }

        if (detected)
        {
            CheckIsClose();

            if (currentAttack == RedXenomorphAttacks.None)
            {
                attackTimer += Time.deltaTime;
                attachedGameObject.animator.Play("Walk");
            }

            CheckIsMaxRangeStopChasing();

            if (currentAttack == RedXenomorphAttacks.None && attackTimer >= attackCooldown)
            {
                currentState = States.Attack;
                //Debug.Log("Red Xenomorph switched to Attack");
            }

            if (playerDistance > maxChasingRange && currentState != States.Attack)
            {
                detected = false;
                currentState = States.Patrol;
                //Debug.Log("Red Xenomorph switched to Patrol");
            }
        }
    }

    private void CheckIsClose()
    {
        if (playerDistance < isCloseRange && !isClose)
        {
            isClose = true;
            //Debug.Log("Player is now CLOSE");
        }
        else if (playerDistance >= isCloseRange && isClose)
        {
            isClose = false;
            //Debug.Log("Player is now FAR");
        }
    }

    private void CheckIsMaxRangeStopChasing()
    {
        if (playerDistance <= maxRangeStopChasing && currentAttack == RedXenomorphAttacks.None &&
            currentState != States.Idle)
        {
            currentState = States.Idle;
            //Debug.Log("Player is INSIDE maxRangeStopChasing");
        }
        else if (playerDistance > maxRangeStopChasing && currentAttack == RedXenomorphAttacks.None &&
                 currentState != States.Chase)
        {
            currentState = States.Chase;
            //Debug.Log("Player is OUTSIDE maxRangeStopChasing");
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
                    case RedXenomorphAttacks.SpikeThrow:
                        SpikeThrow();
                        break;
                    case RedXenomorphAttacks.TailStab:
                        TailStab();
                        break;
                    default:
                        break;
                }
                break;
            case States.Chase:
                player.isFighting = true;
                attachedGameObject.transform.Translate(attachedGameObject.transform.forward * movementSpeed * Time.deltaTime);
                attachedGameObject.transform.LookAt2D(playerGO.transform.position);
                break;
            case States.Patrol:
                Patrol();
                break;
            case States.Dead:
                attachedGameObject.animator.Play("Death");
                deathPSGO.Play();
                break;
            default:
                break;
        }
    }

    private void ChooseAttack()
    {
        if (currentAttack == RedXenomorphAttacks.None)
        {
            if (isClose)
            {
                currentAttack = RedXenomorphAttacks.TailStab;
                attachedGameObject.animator.Play("TailAttack");
                tailAttackPSGO.Play();
            }
            else
            {
                currentAttack = RedXenomorphAttacks.SpikeThrow;
                attachedGameObject.animator.Play("Spit");
                acidSpitPSGO.Play();
            }
            //Debug.Log("RedXenomorph current attack: " + currentAttack);
        }
    }

    private void SpikeThrow()
    {
        //InternalCalls.InstantiateBullet(attachedGameObject.transform.position + attachedGameObject.transform.forward * 12.5f, attachedGameObject.transform.rotation);
        //attachedGameObject.source.PlayAudio(IAudioSource.EventIDs.E_X_ADULT_SPIT);

        if (attachedGameObject.animator.CurrentAnimHasFinished)
        {
            ResetState();
        }
    }

    private void TailStab()
    {
        if (attachedGameObject.animator.CurrentAnimHasFinished)
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
        currentAttack = RedXenomorphAttacks.None;
        currentState = States.Chase;
    }

    public void ReduceLife() //temporary function for the hardcoding of collisions
    {
        life -= 10.0f;
    }

    private bool MoveTo(Vector3 targetPosition)
    {
        //Return true if arrived at destination
        if (Vector3.Distance(attachedGameObject.transform.Position, targetPosition) < 0.5f) return true;

        Vector3 dirVector = (targetPosition - attachedGameObject.transform.Position).Normalize();
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
                Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 4, detectedRange, new Vector3(1.0f, 0.8f, 0.0f)); //Yellow
            }
            else
            {
                Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 4, isCloseRange, new Vector3(1.0f, 0.0f, 0.0f)); //Red
                Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 4, maxChasingRange, new Vector3(1.0f, 0.0f, 1.0f)); //Purple
            }
        }
    }
}