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
    float playerDistance;

    // White Xenomorph parameters
    float life = 350.0f;
    float biomass = 30.0f;
    float movementSpeed = 24.0f * 3;
    States currentState = States.Idle;
    States lastState = States.Idle;
    WhiteXenomorphAttacks currentAttack = WhiteXenomorphAttacks.None;
    Vector3 initialPos;

    // Patrol
    readonly float patrolRange = 100;
    readonly float patrolSpeed = 20.0f;
    float roundProgress = 0.0f; //Do not modify
    bool goingToRoundPos = false;

    // Ranges
    const float detectedRange = 180.0f;
    const float isCloseRange = 20.0f * 3;
    const float maxChasingRange = 180.0f;
    const float maxRangeStopChasing = 25.0f;

    // Flags
    bool detected = false;
    bool isClose = false;
    bool isDead = false;
    bool isAttacking = false;

    // Timers
    float attackTimer = 0.0f;
    const float attackCooldown = 2.0f;
    float destroyTimer = 0.0f;
    const float destroyCooldown = 3.0f;
    float clawAttackTimer = 0.0f;
    const float clawAttackDelay = 1.45f;
    float tailTripTimer = 0.0f;
    const float tailTripDelay = 1.5f;

    PlayerScript player;

    // Particles
    IParticleSystem acidSpitPSGO;
    IParticleSystem tailAttackPSGO;
    IParticleSystem deathPSGO;
    IParticleSystem hitPSGO;

    public override void Start()
    {
        managers.Start();

        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();
        initialPos = attachedGameObject.transform.Position;

        attachedGameObject.animator.Play("Walk");
        attachedGameObject.animator.Blend = true;
        attachedGameObject.animator.TransitionTime = 0.3f;

        acidSpitPSGO = attachedGameObject.FindInChildren("AcidSpitPS")?.GetComponent<IParticleSystem>();
        tailAttackPSGO = attachedGameObject.FindInChildren("TailAttackPS")?.GetComponent<IParticleSystem>();
        deathPSGO = attachedGameObject.FindInChildren("DeathPS")?.GetComponent<IParticleSystem>();
        hitPSGO = attachedGameObject.FindInChildren("HitPS")?.GetComponent<IParticleSystem>();
    }

    public override void Update()
    {
        attachedGameObject.animator.UpdateAnimation();

        if (currentState == States.Dead)
        {
            //destroyTimer += Time.deltaTime;
            //if (destroyTimer >= destroyCooldown)
            //    attachedGameObject.Destroy();

            return;
        }

        if (attachedGameObject.transform.ComponentCheck())
        {
            //Set the director vector and distance to the player
            playerDistance = Vector3.Distance(playerGO.transform.Position, attachedGameObject.transform.Position);

            UpdateFSM();
            DoStateBehaviour();
        }

        DebugDraw();
    }

    void UpdateFSM()
    {
        if (life <= 0)
        {
            currentState = States.Dead;
            return;
        }

        if (!detected && playerDistance < detectedRange)
        {
            detected = true;
            currentState = States.Chase;
            //Debug.Log("White Xenomorph switched to Chase");
        }

        if (detected)
        {
            CheckIsClose();

            if (currentAttack == WhiteXenomorphAttacks.None)
            {
                attackTimer += Time.deltaTime;
                attachedGameObject.animator.Play("Walk");
            }

            CheckIsMaxRangeStopChasing();

            if (currentAttack == WhiteXenomorphAttacks.None && attackTimer >= attackCooldown)
            {
                currentState = States.Attack;
                //Debug.Log("White Xenomorph switched to Attack");
            }

            if (playerDistance > maxChasingRange && currentState != States.Attack)
            {
                detected = false;
                currentState = States.Patrol;
                //Debug.Log("White Xenomorph switched to Patrol");
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
        if (playerDistance <= maxRangeStopChasing && currentAttack == WhiteXenomorphAttacks.None &&
            currentState != States.Idle)
        {
            currentState = States.Idle;
            //Debug.Log("Player is INSIDE maxRangeStopChasing");
        }
        else if (playerDistance > maxRangeStopChasing && currentAttack == WhiteXenomorphAttacks.None &&
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
                attachedGameObject.transform.LookAt2D(playerGO.transform.Position);
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
                attachedGameObject.transform.LookAt2D(playerGO.transform.Position);
                attachedGameObject.transform.Translate(attachedGameObject.transform.Forward * movementSpeed * Time.deltaTime);
                break;
            case States.Patrol:
                Patrol();
                break;
            case States.Dead:
                Dead();
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
                attachedGameObject.animator.Play("ClawAttack");

                tailAttackPSGO.Play();
            }
            else
            {
                currentAttack = WhiteXenomorphAttacks.TailTrip;
                attachedGameObject.animator.Play("TailTrip");

                acidSpitPSGO.Play();
            }
            //Debug.Log("WhiteXenomorph current attack: " + currentAttack);
        }
    }

    private void ClawAttack()
    {
        clawAttackTimer += Time.deltaTime;
        if (clawAttackTimer >= clawAttackDelay && !isAttacking)
        {
            InternalCalls.InstantiateAlienMeleeAttack(attachedGameObject.transform.Position + attachedGameObject.transform.Forward * 1.75f,
                                                      22.0f);
            isAttacking = true;
        }

        if (attachedGameObject.animator.CurrentAnimHasFinished)
        {
            clawAttackTimer = 0.0f;
            ResetState();
        }
    }

    private void TailTrip()
    {
        tailTripTimer += Time.deltaTime;
        if (tailTripTimer >= tailTripDelay && !isAttacking)
        {
            InternalCalls.InstantiateAlienMeleeAttack(attachedGameObject.transform.Position + attachedGameObject.transform.Forward * 1.75f,
                                                      25.0f);
            isAttacking = true;
        }

        if (attachedGameObject.animator.CurrentAnimHasFinished)
        {
            tailTripTimer = 0.0f;
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

    private void Dead()
    {
        if (!isDead)
        {
            attachedGameObject.animator.Play("Death");
            isDead = true;
            player.shieldKillCounter++;
            // add player biomass
            deathPSGO?.Play();
        }
    }

    private void ResetState()
    {
        attackTimer = 0.0f;
        currentAttack = WhiteXenomorphAttacks.None;
        currentState = States.Chase;
        isAttacking = false;
    }

    public void ReduceLife() //temporary function for the hardcoding of collisions
    {
        life -= player.totalDamage;
        if (life < 0) life = 0;
        else hitPSGO?.Replay();
    }

    public void ReduceLifeExplosion()
    {
        life -= player.GrenadeLauncher.damage;
        if (life < 0) life = 0;
        else hitPSGO?.Replay();
    }

    private bool MoveTo(Vector3 targetPosition)
    {
        //Return true if arrived at destination
        if (Vector3.Distance(attachedGameObject.transform.Position, targetPosition) < 0.5f) return true;

        Vector3 dirVector = (targetPosition - attachedGameObject.transform.Position).Normalize();
        attachedGameObject.transform.Translate(dirVector * movementSpeed * Time.deltaTime);

        return false;
    }

    private void DebugDraw()
    {
        //Draw debug ranges
        if (managers.gameManager.colliderRender)
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