using System;

public class AdultXenomorphBehaviour : MonoBehaviour
{
    enum States
    {
        Idle,
        Attack,
        Chase,
        Patrol,
        Dead
    }

    private enum AdultXenomorphAttacks
    {
        None,
        AcidSpit,
        TailAttack
    }

    IGameObject playerGO;
    Vector3 directorVector;
    float playerDistance;

    // Adult Xenomorph parameters
    float life = 200.0f;
    float biomass = 20.0f;
    float movementSpeed = 22.0f * 3;
    
    States currentState = States.Idle;
    States lastState = States.Idle;
    AdultXenomorphAttacks currentAttack = AdultXenomorphAttacks.None;
    Vector3 initialPos;

    // Patrol
    readonly float patrolRange = 100;
    readonly float patrolSpeed = 20.0f;
    float roundProgress = 0.0f; //Do not modify
    bool goingToRoundPos = false;

    // Ranges
    const float detectedRange = 200.0f;
    const float isCloseRange = 20.0f * 3;
    const float maxChasingRange = 200.0f;
    const float maxRangeStopChasing = 25.0f;

    // Flags
    bool detected = false;
    bool isClose = false;
    bool isDead = false;
    bool hasShot = false;
    bool isTailAttacking = false;

    // Timers
    float attackTimer = 0.0f;
    const float attackCooldown = 2.0f;
    float destroyTimer = 0.0f;
    const float destroyCooldown = 3.0f;
    float delayTimer = 0.0f;
    const float delayCooldown = 1.2f;
    float tailAttackTimer = 0.0f;
    const float tailAttackDelay = 1.4f;

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
        if (managers.gameManager.GetGameState() != GameManager.GameStates.RUNNING)
            return;

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
        if (life <= 0)
        {
            currentState = States.Dead;
            return;
        }

        if (!detected && playerDistance < detectedRange)
        {
            detected = true;
            currentState = States.Chase;
            //Debug.Log("Adult Xenomorph switched to Chase");
        }

        if (detected)
        {
            CheckIsClose();

            if (currentAttack == AdultXenomorphAttacks.None)
            {
                attackTimer += Time.deltaTime;
                attachedGameObject.animator.Play("Walk");
            }

            CheckIsMaxRangeStopChasing();

            if (currentAttack == AdultXenomorphAttacks.None && attackTimer >= attackCooldown)
            {
                currentState = States.Attack;
                //Debug.Log("Adult Xenomorph switched to Attack");
            }

            if (playerDistance > maxChasingRange && currentState != States.Attack)
            {
                detected = false;
                currentState = States.Patrol;
                //Debug.Log("Adult Xenomorph switched to Patrol");
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
        if (playerDistance <= maxRangeStopChasing && currentAttack == AdultXenomorphAttacks.None &&
            currentState != States.Idle)
        {
            currentState = States.Idle;
            //Debug.Log("Player is INSIDE maxRangeStopChasing");
        }
        else if (playerDistance > maxRangeStopChasing && currentAttack == AdultXenomorphAttacks.None &&
                 currentState != States.Chase)
        {
            currentState = States.Chase;
            //Debug.Log("Player is OUTSIDE maxRangeStopChasing");
        }
    }

    private void DoStateBehaviour()
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
                    case AdultXenomorphAttacks.AcidSpit:
                        AcidSpit();
                        break;
                    case AdultXenomorphAttacks.TailAttack:
                        TailAttack();
                        break;
                    default:
                        break;
                }
                break;
            case States.Chase:
                player.isFighting = true;
                attachedGameObject.transform.Translate(attachedGameObject.transform.Forward * movementSpeed * Time.deltaTime);
                attachedGameObject.transform.LookAt2D(playerGO.transform.Position);
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
        if (currentAttack == AdultXenomorphAttacks.None)
        {
            if (isClose)
            {
                currentAttack = AdultXenomorphAttacks.TailAttack;
                attachedGameObject.animator.Play("TailAttack");
                tailAttackPSGO.Play();
            }
            else
            {
                currentAttack = AdultXenomorphAttacks.AcidSpit;
                attachedGameObject.animator.Play("Spit");
                acidSpitPSGO.Play();
            }
            //Debug.Log("AdultXenomorph current attack: " + currentAttack);
        }
    }

    private void AcidSpit()
    {
        delayTimer += Time.deltaTime;

        if (!hasShot && delayTimer >= delayCooldown)
        {
            hasShot = true;
            Vector3 height = new Vector3(0.0f, 38.0f, 0.0f);
            InternalCalls.InstantiateBullet(attachedGameObject.transform.Position + attachedGameObject.transform.Forward * 13.5f + height, attachedGameObject.transform.Rotation);
            // SFX Goes here
        }

        if (attachedGameObject.animator.CurrentAnimHasFinished)
        {
            hasShot = false;
            delayTimer = 0.0f;
            ResetState();
        }
    }

    private void TailAttack()
    {
        tailAttackTimer += Time.deltaTime;
        if (tailAttackTimer >= tailAttackDelay && !isTailAttacking)
        {
            InternalCalls.InstantiateAlienMeleeAttack(attachedGameObject.transform.Position,
                                                      25.0f);
            isTailAttacking = true;
        }

        if (attachedGameObject.animator.CurrentAnimHasFinished)
        {
            isTailAttacking = false;
            tailAttackTimer = 0.0f;
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
            attachedGameObject.GetComponent<ICollider2D>().radius = 0.0f;
            // add player biomass
            managers.gameManager.currency += (int)this.biomass;
            deathPSGO?.Play();
        }
    }

    private void ResetState()
    {
        attackTimer = 0.0f;
        currentAttack = AdultXenomorphAttacks.None;
        currentState = States.Chase;
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
        //attachedGameObject.source.PlayAudio(AudioManager.EventIDs.E_REBEL_STEP);
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