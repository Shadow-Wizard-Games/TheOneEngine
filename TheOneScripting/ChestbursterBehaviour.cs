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
    bool isDead = false;

    // Timers
    float attackTimer = 0.0f;
    const float attackCooldown = 4.0f;
    float destroyTimer = 0.0f;
    const float destroyCooldown = 3.0f;

    PlayerScript player;
    GameManager gameManager;

    // Particles
    IParticleSystem tailPunchPSGO;
    IParticleSystem tailTripPSGO;
    IParticleSystem deathPSGO;

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        gameManager = IGameObject.Find("GameManager").GetComponent<GameManager>();

        attachedGameObject.animator.Play("Move");
        attachedGameObject.animator.Blend = true;
        attachedGameObject.animator.TransitionTime = 0.3f;

        tailPunchPSGO = attachedGameObject.FindInChildren("TailPunchPS")?.GetComponent<IParticleSystem>();
        tailTripPSGO = attachedGameObject.FindInChildren("TailTripPS")?.GetComponent<IParticleSystem>();
        deathPSGO = attachedGameObject.FindInChildren("DeathPS")?.GetComponent<IParticleSystem>();
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

        if (attachedGameObject.animator.CurrentAnimHasFinished)
        {
            ResetState();
        }

    }

    private void TailTrip()
    {
        //Debug.Log("Attempt to do TailTrip");

        if (attachedGameObject.animator.CurrentAnimHasFinished)
        {
            ResetState();
        }

    }

    private void Dead()
    {
        if (!isDead)
        {
            attachedGameObject.animator.Play("Die");

            if (attachedGameObject.animator.CurrentAnimHasFinished)
            {
                isDead = true;
                player.shieldKillCounter++;
                deathPSGO.Play();
            }
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
        life -= player.totalDamage;
        if (life < 0) life = 0;
        Debug.Log("Chestburster health: " + life.ToString());
    }

    public void ReduceLifeExplosion()
    {
        life -= player.grenadeDamage;
        if (life < 0) life = 0;
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
                Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 4, maxChasingRange, new Vector3(0.9f, 0.0f, 0.9f)); //Purple
                Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 4, isCloseRange, new Vector3(0.0f, 0.8f, 1.0f)); //Blue
            }
        }
    }
}