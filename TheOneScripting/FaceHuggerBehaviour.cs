using System;

public class FaceHuggerBehaviour : MonoBehaviour
{
    enum States
    {
        Idle,
        Chase,
        JumpAttack,
        Hug,
        Land,
        Dead
    }

    IGameObject playerGO;
    Vector3 directorVector;
    float playerDistance;

    // Facehugger parameters
    float life = 40.0f;
    float biomass = 0.0f;
    float movementSpeed = 35.0f * 2;
    States currentState = States.Idle;

    // Ranges
    const float detectedRange = 180.0f;
    const float maxAttackRange = 90.0f;
    const float maxChasingRange = 180.0f;
    const float maxRangeStopChasing = 60.0f;

    // Flags
    bool detected = false;
    bool hitPlayer = false;
    bool isDead = false;
    bool isJumping = false;
    bool isHugging = false;
    bool isLanding = false;
    bool fallDead = false;
    bool shotDead = false;

    // Timers
    float jumpAttackTimer = 0.0f;
    const float jumpAttackCooldown = 3.0f;

    PlayerScript player;
    GameManager gameManager;

    IParticleSystem jumpPSGO;
    IParticleSystem deathPSGO;

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        gameManager = IGameObject.Find("GameManager").GetComponent<GameManager>();

        attachedGameObject.animator.Play("Idle");
        attachedGameObject.animator.Blend = true;
        attachedGameObject.animator.TransitionTime = 0.3f;

        jumpPSGO = attachedGameObject.FindInChildren("JumpPS")?.GetComponent<IParticleSystem>();
        deathPSGO = attachedGameObject.FindInChildren("DeathPS")?.GetComponent<IParticleSystem>();
    }

    public override void Update()
    {
        attachedGameObject.animator.UpdateAnimation();

        if (currentState == States.Dead) return;

        if (attachedGameObject.transform.ComponentCheck())
        {
            DebugDraw();

            // Set the director vector and distance to the player
            directorVector = (playerGO.transform.Position - attachedGameObject.transform.Position).Normalize();
            playerDistance = Vector3.Distance(playerGO.transform.Position, attachedGameObject.transform.Position);

            UpdateFSM();
            DoStateBehaviour();
        }
    }

    void UpdateFSM()
    {
        if (life <= 0 || fallDead)
        {
            if (attachedGameObject.animator.CurrentAnimHasFinished || shotDead)
                currentState = States.Dead;
            //Debug.Log("Facehugger switched to Dead");
            return;
        }

        if (!detected && playerDistance < detectedRange)
        {
            detected = true;
            currentState = States.Chase;
            //Debug.Log("Facehugger switched to Chase");
        }

        if (detected)
        {
            if (currentState != States.JumpAttack &&
                currentState != States.Hug && currentState != States.Land)
            {
                jumpAttackTimer += Time.deltaTime;
                if (currentState != States.Idle)
                    attachedGameObject.animator.Play("Walk");
            }

            CheckIsMaxRangeStopChasing();

            if (jumpAttackTimer >= jumpAttackCooldown && currentState != States.JumpAttack)
            {
                currentState = States.JumpAttack;
                //Debug.Log("Facehugger switched to JumpAttack");
            }

            if (playerDistance > maxChasingRange && currentState != States.JumpAttack &&
                currentState != States.Hug && currentState != States.Land)
            {
                detected = false;
                currentState = States.Idle;
                //Debug.Log("Facehugger switched to Idle");
            }
        }
    }

    private void CheckIsMaxRangeStopChasing()
    {
        if (playerDistance <= maxRangeStopChasing && currentState != States.Idle)
        {
            currentState = States.Idle;
            //Debug.Log("Player is INSIDE maxRangeStopChasing");
        }
        else if (playerDistance > maxRangeStopChasing && currentState != States.Chase &&
                 currentState != States.Hug)
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
                attachedGameObject.animator.Play("Idle");
                return;
            case States.Chase:
                player.isFighting = true;
                attachedGameObject.transform.LookAt2D(playerGO.transform.Position);
                attachedGameObject.transform.Translate(attachedGameObject.transform.Forward * movementSpeed * Time.deltaTime);
                break;
            case States.JumpAttack:
                player.isFighting = true;
                attachedGameObject.transform.Translate(attachedGameObject.transform.Forward * movementSpeed * Time.deltaTime);
                JumpAttack();
                break;
            case States.Hug:
                Hug();
                break;
            case States.Land:
                Land();
                break;
            case States.Dead:
                Dead();
                break;
            default:
                break;
        }
    }

    private void JumpAttack()
    {
        if (!isJumping)
        {
            isJumping = true;
            attachedGameObject.animator.Play("Jump");
        }

        if (attachedGameObject.animator.CurrentAnimHasFinished)
        {
            CheckIsHugging();
        }
    }

    private void CheckIsHugging()
    {
        jumpAttackTimer = 0.0f;
        isJumping = false;
        if (hitPlayer)
        {
            currentState = States.Hug;
        }
        else
        {
            currentState = States.Land;
        }
    }

    private void Hug()
    {
        if (!isHugging)
        {
            attachedGameObject.animator.Play("Hug");
            isHugging = true;
        }

        fallDead = true;
        if (attachedGameObject.animator.CurrentAnimHasFinished)
        {
            isHugging = false;
            fallDead = true;
        }
    }

    private void Land()
    {
        if (!isLanding)
        {
            attachedGameObject.animator.Play("Land");
            isLanding = true;
        }

        if (attachedGameObject.animator.CurrentAnimHasFinished)
        {
            currentState = States.Chase;
            isLanding = false;
        }
    }

    private void Dead()
    {
        if (!isDead)
        {
            if (fallDead) attachedGameObject.animator.Play("FallAndDie");

            if (shotDead) attachedGameObject.animator.Play("ShotAndDie");

            if (attachedGameObject.animator.CurrentAnimHasFinished)
            {
                isDead = true;
                player.shieldKillCounter++;
                // add player biomass
                deathPSGO.Play();
            }
        }
    }

    public void CheckJump() //temporary function for the hardcoding of collisions
    {
        if (!hitPlayer)
        {
            if (isJumping)
            {
                playerGO.GetComponent<PlayerScript>().ReduceLife();
                hitPlayer = true;
                fallDead = true;
            }
        }
    }

    public void ReduceLife() //temporary function for the hardcoding of collisions
    {
        life -= player.totalDamage;
        if (life < 0)
        {
            life = 0;
            shotDead = true;
        }
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
                Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 4, maxAttackRange, new Vector3(0.0f, 0.8f, 1.0f)); //Blue
            }
        }
    }
}