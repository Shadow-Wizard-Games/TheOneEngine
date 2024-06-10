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
    Vector3 directorVector;
    float playerDistance;

    // Chestburster parameters
    float life = 15.0f;
    float biomass = 10.0f;
    float movementSpeed = 35.0f * 3;
    States currentState = States.Idle;
    ChestbursterAttack currentAttack = ChestbursterAttack.None;

    // Ranges
    const float detectedRange = 200.0f;
    const float isCloseRange = 60.0f;
    const float maxChasingRange = 200.0f;
    const float maxRangeStopChasing = 25.0f;

    // Flags
    bool isClose = false;
    bool detected = false;
    bool isDead = false;
    bool isAttacking = false;

    // Timers
    float attackTimer = 0.0f;
    const float attackCooldown = 4.0f;
    float destroyTimer = 0.0f;
    const float destroyCooldown = 3.0f;
    float tailPunchTimer = 0.0f;
    const float tailPunchDelay = 0.5f; // 0.0f ~ 1.0f
    float tailTripTimer = 0.0f;
    const float tailTripDelay = 0.5f; // 0.0f ~ 1.0f

    PlayerScript player;

    // Particles
    IParticleSystem tailPunchPSGO;
    IParticleSystem tailTripPSGO;
    IParticleSystem deathPSGO;
    IParticleSystem hitPSGO;

    public override void Start()
    {
        managers.Start();

        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        attachedGameObject.animator.Play("Move");
        attachedGameObject.animator.Blend = true;
        attachedGameObject.animator.TransitionTime = 0.3f;

        tailPunchPSGO = attachedGameObject.FindInChildren("TailPunchPS")?.GetComponent<IParticleSystem>();
        tailTripPSGO = attachedGameObject.FindInChildren("TailTripPS")?.GetComponent<IParticleSystem>();
        deathPSGO = attachedGameObject.FindInChildren("DeathPS")?.GetComponent<IParticleSystem>();
        hitPSGO = attachedGameObject.FindInChildren("HitPS")?.GetComponent<IParticleSystem>();
    }

    public override void Update()
    {
        if (managers.gameManager.GetGameState() != GameManager.GameStates.RUNNING) return;

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
            //Debug.Log("Chestburster switched to Dead");
            return;
        }

        if (!detected && playerDistance < detectedRange)
        {
            detected = true;
            currentState = States.Chase;
            //Debug.Log("Chestburster switched to Chase");
        }

        if (detected)
        {
            CheckIsClose();

            if (currentAttack == ChestbursterAttack.None)
            {
                attackTimer += Time.deltaTime;
                attachedGameObject.animator.Play("Run");
            }

            CheckIsMaxRangeStopChasing();

            if (currentAttack == ChestbursterAttack.None && attackTimer >= attackCooldown)
            {
                currentState = States.Attack;
                //Debug.Log("Chestburster switched to Attack");
            }

            if (playerDistance > maxChasingRange && currentState != States.Attack)
            {
                detected = false;
                currentState = States.Idle;
                //Debug.Log("Chestburster switched to Idle");
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

        if (playerDistance >= isCloseRange && isClose)
        {
            isClose = false;
            //Debug.Log("Player is now FAR");
        }
    }

    private void CheckIsMaxRangeStopChasing()
    {
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
    }

    void DoStateBehaviour()
    {
        switch (currentState)
        {
            case States.Idle:
                if (currentAttack == ChestbursterAttack.None && detected)
                    attachedGameObject.transform.LookAt2D(playerGO.transform.Position);

                break;
            case States.Chase:
                player.isFighting = true;
                attachedGameObject.transform.Translate(attachedGameObject.transform.Forward * movementSpeed * Time.deltaTime);
                attachedGameObject.transform.LookAt2D(playerGO.transform.Position);
                break;
            case States.Attack:
                player.isFighting = true;
                attachedGameObject.transform.LookAt2D(playerGO.transform.Position);
                ChooseAttack();
                switch (currentAttack)
                {
                    case ChestbursterAttack.TailPunch:
                        TailPunch();
                        break;
                    case ChestbursterAttack.TailTrip:
                        TailTrip();
                        break;
                    default:
                        break;
                }
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
        if (currentAttack == ChestbursterAttack.None)
        {
            if (isClose)
            {
                currentAttack = ChestbursterAttack.TailPunch;
                attachedGameObject.animator.Play("TailPunch");
                tailPunchPSGO.Play();
            }
            else
            {
                currentAttack = ChestbursterAttack.TailTrip;
                attachedGameObject.animator.Play("TailTrip");
                tailTripPSGO.Play();
            }
            //Debug.Log("Chestburster current attack: " + currentAttack);
        }
    }

    private void TailPunch()
    {
        //Debug.Log("Attempt to do TailPunch");
        tailPunchTimer += Time.deltaTime;
        if (tailPunchTimer >= tailPunchDelay && !isAttacking)
        {
            InternalCalls.InstantiateAlienMeleeAttack(attachedGameObject.transform.Position + attachedGameObject.transform.Forward * 1.5f,
                                                      10.0f);
            isAttacking = true;
        }

        if (attachedGameObject.animator.CurrentAnimHasFinished)
        {
            ResetState();
            tailPunchTimer = 0.0f;
        }
    }

    private void TailTrip()
    {
        //Debug.Log("Attempt to do TailTrip");
        tailTripTimer += Time.deltaTime;
        if (tailTripTimer >= tailTripDelay && !isAttacking)
        {
            InternalCalls.InstantiateAlienMeleeAttack(attachedGameObject.transform.Position + attachedGameObject.transform.Forward * 1.5f,
                                                      10.0f);
            isAttacking = true;
        }

        if (attachedGameObject.animator.CurrentAnimHasFinished)
        {
            ResetState();
            tailTripTimer = 0.0f;
        }
    }

    private void Dead()
    {
        if (!isDead)
        {
            attachedGameObject.animator.Play("Die");

            isDead = true;
            player.shieldKillCounter++;
            attachedGameObject.GetComponent<ICollider2D>().radius = 0.0f;
            // add player biomass
            managers.gameManager.currency += (int)this.biomass;
            deathPSGO?.Play();
            attachedGameObject.source.Play(IAudioSource.AudioEvent.E_CB_DEATH);
        }
    }

    private void ResetState()
    {
        //Debug.Log("Reset State");
        isAttacking = false;
        attackTimer = 0.0f;
        currentAttack = ChestbursterAttack.None;
        currentState = States.Chase;
        attachedGameObject.animator.Play("Run");
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
                Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 4, maxChasingRange, new Vector3(0.9f, 0.0f, 0.9f)); //Purple
                Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 4, isCloseRange, new Vector3(0.0f, 0.8f, 1.0f)); //Blue
            }
        }
    }
}