using System;

class AnarchistBehaviour : MonoBehaviour
{
    enum States
    {
        Patrol,
        Inspect,
        Attack,
        Dying,
        Dead
    }

    IGameObject playerGO;
    float playerDistance;

    readonly float rangeToInspect = 200;
    readonly float inspectDetectionRadius = 100;
    readonly float loseRange = 150;

    States lastState = States.Patrol;
    States currentState = States.Patrol;
    Vector3 initialPos;

    float life = 100;
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

        if (currentState == States.Dying)
        {
            if (attachedGameObject.animator.CurrentAnimHasFinished)
            {
                currentState = States.Dead;
                return;
            }

            attachedGameObject.animator.Play("Death");

            return;
        }
        else if (currentState == States.Dead)
        {
            attachedGameObject.animator.Play("Dead Pose");

            return;
        }

        playerDistance = Vector3.Distance(playerGO.transform.Position, attachedGameObject.transform.Position);

        UpdateFSMStates();
        DoStateBehaviour();
    }

    void UpdateFSMStates()
    {
        if (life <= 0) { currentState = States.Dead; return; }

        if (playerDistance < rangeToInspect && lastState != States.Inspect)
        {
            currentState = States.Inspect;
        }
    }

    void DoStateBehaviour()
    {
        switch (currentState)
        {
            case States.Patrol:
                PatrolState();
                break;
            case States.Inspect:
                InspectState();
                break;
            case States.Attack:
                player.isFighting = true;
                AttackState();
                break;
            case States.Dying:
                //attachedGameObject.transform.Rotate(Vector3.right * 90.0f);
                break;
            case States.Dead:
                break;
            default:
                break;
        }
    }

    readonly float patrolRange = 100;
    readonly float patrolSpeed = 30.0f;
    float roundProgress = 0.0f; //Do not modify
    bool goingToRoundPos = false;

    void PatrolState()
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

        if (gameManager.colliderRender) { Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 3, rangeToInspect, Vector3.right + Vector3.up); }
    }

    enum InspctStates
    {
        Going,
        Inspecting,
        ComingBack
    }

    Vector3 playerLastPosition;
    InspctStates currentSubstate = InspctStates.Going;
    readonly float maxInspectTime = 5.0f;
    float elapsedTime = 0.0f; //Do not modify

    void InspectState()
    {
        if (currentState != lastState)
        {
            Debug.Log("StartingState");
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
                if (gameManager.colliderRender) { Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 3, inspectDetectionRadius, Vector3.right + Vector3.up * 0.3f); }
                break;
            case InspctStates.Inspecting:
                attachedGameObject.animator.Play("Scan");
                elapsedTime += Time.deltaTime;
                if (elapsedTime > maxInspectTime)
                {
                    elapsedTime = 0.0f;
                    currentSubstate = InspctStates.ComingBack;
                }
                attachedGameObject.transform.Rotate(Vector3.up * 150.0f * Time.deltaTime);
                if (gameManager.colliderRender) { Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 3, inspectDetectionRadius, Vector3.right + Vector3.up * 0.5f); }
                break;
            case InspctStates.ComingBack:
                attachedGameObject.animator.Play("Walk");
                if (MoveTo(initialPos))
                {
                    currentSubstate = InspctStates.Going;
                    currentState = States.Patrol;
                }
                attachedGameObject.transform.LookAt2D(initialPos);
                if (gameManager.colliderRender) { Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 3, inspectDetectionRadius, Vector3.right + Vector3.up * 0.8f); }
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

    bool shooting = false;
    float timerBetweenBursts = 0.0f; //Do not modify
    readonly float timeBetweenBursts = 0.5f;
    float timerBetweenBullets = 0.0f; //Do not modify
    readonly float timeBetweenBullets = 0.05f;
    int bulletCounter = 0; //Do not modify
    readonly int burstBulletCount = 3;

    void AttackState()
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

        if (gameManager.colliderRender) { Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 3, loseRange, Vector3.right); }
    }

    readonly float movementSpeed = 50.0f;

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
        life -= 10.0f;
    }
}
