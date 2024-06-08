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
    IGameObject M4GO;
    float playerDistance;

    // Anarchist parameters
    float life = 100.0f;
    float biomass = 15.0f;
    float movementSpeed = 13.0f * 3;
    States currentState = States.Patrol;
    States lastState = States.Patrol;
    Vector3 initialPos;

    // Ranges
    const float rangeToInspect = 200.0f;
    const float inspectDetectionRadius = 100.0f;
    const float loseRange = 150.0f;

    // Inspect
    float elapsedTime = 0.0f; //Do not modify
    const float maxInspectTime = 5.0f;
    InspctStates currentSubstate = InspctStates.Going;
    Vector3 playerLastPosition;

    // Attack
    float burstTimer = 0.0f; 
    const float burstCooldown = 1.0f;
    float bulletsTimer = 0.0f; 
    const float bulletsCooldown = 0.05f;
    int bulletCounter = 0; 
    const int burstBulletCount = 3;

    // Patrol
    const float patrolRange = 100.0f;
    const float patrolSpeed = 30.0f;
    float roundProgress = 0.0f; //Do not modify
    bool goingToRoundPos = false;

    // Flags
    bool isDead = false;
    bool isShooting = false;

    // Timers
    float destroyTimer = 0.0f;
    const float destroyCooldown = 3.0f;

    PlayerScript player;

    IParticleSystem deathPSGO;
    IParticleSystem hitPSGO;

    public override void Start()
    {
        managers.Start();

        playerGO = IGameObject.Find("SK_MainCharacter");
        M4GO = attachedGameObject.FindInChildren("WP_CarabinaM4");

        player = playerGO.GetComponent<PlayerScript>();
        initialPos = attachedGameObject.transform.Position;
        
        attachedGameObject.animator.Play("Scan");
        M4GO.animator.Play("AnarchistWalk");
        attachedGameObject.animator.Blend = true;
        attachedGameObject.animator.TransitionTime = 0.3f;

        deathPSGO = attachedGameObject.FindInChildren("DeathPS")?.GetComponent<IParticleSystem>();
        hitPSGO = attachedGameObject.FindInChildren("HitPS")?.GetComponent<IParticleSystem>();
    }

    public override void Update()
    {
        attachedGameObject.animator.UpdateAnimation();
        M4GO.animator.UpdateAnimation();

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
        M4GO.animator.Play("AnarchistWalk");

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

        if (!isShooting)
        {
            burstTimer += Time.deltaTime;
            if (burstTimer > burstCooldown)
            {
                burstTimer = 0.0f;
                isShooting = true;
            }
        }
        else
        {
            bulletsTimer += Time.deltaTime;
            if (bulletsTimer > bulletsCooldown)
            {
                attachedGameObject.animator.Play("Shoot");
                M4GO.animator.Play("AnarchistShoot");
                Vector3 height = new Vector3(0.0f, 30.0f, 0.0f);
                InternalCalls.InstantiateBullet(attachedGameObject.transform.Position + attachedGameObject.transform.Forward * 13.5f + height, attachedGameObject.transform.Rotation);
                bulletsTimer = 0.0f;
                bulletCounter++;
                attachedGameObject.source.Play(IAudioSource.AudioEvent.E_A_SHOOT);
            }

            if (bulletCounter >= burstBulletCount)
            {
                bulletCounter = 0;
                isShooting = false;
            }
        }
    }

    void Inspect()
    {
        if (currentState != lastState)
        {
            //Debug.Log("Anarchist starting state");
            lastState = currentState;
            playerLastPosition = playerGO.transform.Position;
        }

        switch (currentSubstate)
        {
            case InspctStates.Going:
                M4GO.Enable();
                attachedGameObject.animator.Play("Walk");
                M4GO.animator.Play("AnarchistWalk");
                if (MoveTo(playerLastPosition))
                {
                    currentSubstate = InspctStates.Inspecting;
                }
                attachedGameObject.transform.LookAt2D(playerLastPosition);
                break;
            case InspctStates.Inspecting:
                M4GO.Disable();
                attachedGameObject.animator.Play("Scan");
                elapsedTime += Time.deltaTime;
                if (elapsedTime > maxInspectTime)
                {
                    elapsedTime = 0.0f;
                    currentSubstate = InspctStates.ComingBack;
                }
                break;
            case InspctStates.ComingBack:
                M4GO.Enable();
                attachedGameObject.animator.Play("Walk");
                M4GO.animator.Play("AnarchistWalk");
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
            M4GO.Disable();
            attachedGameObject.animator.Play("Death");
            isDead = true;
            player.shieldKillCounter++;
            // add player biomass
            deathPSGO?.Play();
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
        else hitPSGO?.Replay();
        Debug.Log("Total damage " + player.totalDamage);
        //Debug.Log("Total life " + life);
    }

    public void ReduceLifeExplosion()
    {
        life -= player.GrenadeLauncher.damage;
        if (life < 0) life = 0;
        else hitPSGO?.Replay();
    }

    private void DebugDraw()
    {
        if (managers.gameManager.colliderRender)
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
