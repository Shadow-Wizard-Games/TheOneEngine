using System;

class AnarchistBehaviour : MonoBehaviour
{
    enum States
    {
        Patrol,
        Inspect,
        Attack,
        Dead
    }

    enum InspctStates
    {
        Going,
        Inspecting,
        ComingBack
    }

    IGameObject playerGO;
    float playerDistance;

    // Anarchist parameters
    float life = 100.0f;
    float movementSpeed = 50.0f;
    States currentState = States.Patrol;
    States lastState = States.Patrol;
    Vector3 initialPos;

    // Ranges
    private const float rangeToInspect = 200.0f;
    private const float inspectDetectionRadius = 100.0f;
    private const float loseRange = 150.0f;

    // Inspect
    float elapsedTime = 0.0f; //Do not modify
    private const float maxInspectTime = 5.0f;
    InspctStates currentSubstate = InspctStates.Going;
    Vector3 playerLastPosition;

    // Attack
    bool shooting = false;
    float timerBetweenBursts = 0.0f; //Do not modify
    readonly float timeBetweenBursts = 0.5f;
    float timerBetweenBullets = 0.0f; //Do not modify
    readonly float timeBetweenBullets = 0.05f;
    int bulletCounter = 0; //Do not modify
    readonly int burstBulletCount = 3;

    // Patrol
    readonly float patrolRange = 100.0f;
    readonly float patrolSpeed = 30.0f;
    float roundProgress = 0.0f; //Do not modify
    bool goingToRoundPos = false;

    // Flags
    bool isDead = false;

    // Timers
    float destroyTimer = 0.0f;
    const float destroyCooldown = 3.0f;

    PlayerScript player;
    GameManager gameManager;

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();
        initialPos = attachedGameObject.transform.Position;

        gameManager = IGameObject.Find("GameManager").GetComponent<GameManager>();
        attachedGameObject.animator.Play("Scan");

        attachedGameObject.animator.Blend = false;
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

            playerDistance = Vector3.Distance(playerGO.transform.Position, attachedGameObject.transform.Position);

            UpdateFSMStates();
            DoStateBehaviour();
        }
    }

    void UpdateFSMStates()
    {
        if (life <= 0) 
        { 
            currentState = States.Dead;
            player.shieldKillCounter++;
            return; 
        }

        if (playerDistance < rangeToInspect && lastState != States.Inspect)
        {
            currentState = States.Inspect;
            //Debug.Log("Switching state to Inspect");
        }
    }

    void DoStateBehaviour()
    {
        switch (currentState)
        {
            case States.Patrol:
                Patrol();
                break;
            case States.Inspect:
                Inspect();
                break;
            case States.Attack:
                player.isFighting = true;
                Attack();
                break;
            case States.Dead:
                Dead();
                break;
            default:
                break;
        }
    }

    void Patrol()
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

    void Attack()
    {
        attachedGameObject.transform.LookAt2D(playerGO.transform.Position);

        if (playerDistance > loseRange)
        {
            currentState = States.Inspect;
            currentSubstate = InspctStates.ComingBack;
        }

        if (!shooting)
        {
            timerBetweenBursts += Time.deltaTime;
            if (timerBetweenBursts > timeBetweenBursts)
            {
                timerBetweenBursts = 0.0f;
                shooting = true;
            }
        }
        else
        {
            timerBetweenBullets += Time.deltaTime;
            if (timerBetweenBullets > timeBetweenBullets)
            {
                attachedGameObject.animator.Play("Shoot");

                timerBetweenBullets = 0.0f;
                bulletCounter++;
                attachedGameObject.source.Play(IAudioSource.AudioEvent.E_A_SHOOT);
            }

            if (bulletCounter >= burstBulletCount)
            {
                bulletCounter = 0;
                shooting = false;
            }
        }


    }

    void Inspect()
    {
        if (currentState != lastState)
        {
            Debug.Log("Anarchist starting state");
            lastState = currentState;
            playerLastPosition = playerGO.transform.Position;
        }

        switch (currentSubstate)
        {
            case InspctStates.Going:
                attachedGameObject.animator.Play("Walk");
                if (MoveTo(playerLastPosition))
                {
                    currentSubstate = InspctStates.Inspecting;
                }
                attachedGameObject.transform.LookAt2D(playerLastPosition);
                break;
            case InspctStates.Inspecting:
                attachedGameObject.animator.Play("Scan");
                elapsedTime += Time.deltaTime;
                if (elapsedTime > maxInspectTime)
                {
                    elapsedTime = 0.0f;
                    currentSubstate = InspctStates.ComingBack;
                }
                break;
            case InspctStates.ComingBack:
                attachedGameObject.animator.Play("Walk");
                if (MoveTo(initialPos))
                {
                    currentSubstate = InspctStates.Going;
                    currentState = States.Patrol;
                }
                attachedGameObject.transform.LookAt2D(initialPos);
                break;
            default:
                break;
        }

        Vector3 directorVec = (attachedGameObject.transform.Position - playerGO.transform.Position).Normalize();
        float dot = Vector3.Dot(attachedGameObject.transform.Forward, directorVec);

        if (playerDistance < inspectDetectionRadius && dot > 0.7f)
        {
            currentState = States.Attack;
        }
    }

    void Dead()
    {
        if (!isDead)
        {
            attachedGameObject.animator.Play("Death");

            if (attachedGameObject.animator.CurrentAnimHasFinished) isDead = true;

        }
    }

    bool MoveTo(Vector3 targetPosition)
    {
        //Return true if arrived at destination
        if (Vector3.Distance(attachedGameObject.transform.Position, targetPosition) < 0.5f) return true;

        Vector3 dirVector = (targetPosition - attachedGameObject.transform.Position).Normalize();
        attachedGameObject.transform.Translate(dirVector * movementSpeed * Time.deltaTime);
        return false;
    }

    public void ReduceLife() //temporary function for the hardcoding of collisions
    {
        life -= player.totalDamage;
        if (life < 0) life = 0;
    }

    public void ReduceLifeExplosion()
    {
        life -= player.grenadeDamage;
        if (life < 0) life = 0;
    }

    private void DebugDraw()
    {
        if (gameManager.colliderRender)
        {
            Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 3, loseRange, Vector3.right);
            Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 3, rangeToInspect, Vector3.right + Vector3.up);

            switch (currentSubstate)
            {
                case InspctStates.Going:
                    Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 3, inspectDetectionRadius, Vector3.right + Vector3.up * 0.3f);
                    break;
                case InspctStates.Inspecting:
                    Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 3, inspectDetectionRadius, Vector3.right + Vector3.up * 0.5f);
                    break;
                case InspctStates.ComingBack:
                    Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 3, inspectDetectionRadius, Vector3.right + Vector3.up * 0.8f);
                    break;
                default:
                    break;
            }
        }
    }
}
