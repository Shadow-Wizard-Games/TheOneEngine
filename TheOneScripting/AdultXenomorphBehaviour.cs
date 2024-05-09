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
    float movementSpeed = 0.0f * 3;
    States currentState = States.Idle;
    AdultXenomorphAttacks currentAttack = AdultXenomorphAttacks.None;

    // Ranges
    const float detectedRange = 35.0f * 3;
    const float isCloseRange = 20.0f * 3;
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

        //attachedGameObject.animator.Play("Idle");
        //attachedGameObject.animator.blend = false;
        //attachedGameObject.animator.transitionTime = 0.0f;
    }

    public override void Update()
    {
        //attachedGameObject.animator.UpdateAnimation();

        if (currentState == States.Dead) return;

        if (attachedGameObject.transform.ComponentCheck())
        {
            DebugDraw();

            //Set the director vector and distance to the player
            directorVector = (playerGO.transform.position - attachedGameObject.transform.position).Normalize();
            playerDistance = Vector3.Distance(playerGO.transform.position, attachedGameObject.transform.position);

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
        }

        if (detected)
        {
            attachedGameObject.transform.LookAt2D(playerGO.transform.position);
            if (playerDistance < isCloseRange && !isClose)
            {
                isClose = true;
                Debug.Log("Player is now CLOSE");
            }

            if (playerDistance >= isCloseRange && isClose)
            {
                isClose = false;
                Debug.Log("Player is now FAR");
            }

            if (playerDistance > maxChasingRange)
            {
                detected = false;
                currentState = States.Patrol;
            }

            if (currentAttack == AdultXenomorphAttacks.None)
            {
                //attachedGameObject.transform.Translate(attachedGameObject.transform.forward * movementSpeed * Time.deltaTime);
                attackTimer += Time.deltaTime;
            }

            if (currentAttack == AdultXenomorphAttacks.None && attackTimer >= attackCooldown)
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
                attachedGameObject.transform.Translate(attachedGameObject.transform.forward * movementSpeed * Time.deltaTime);
                break;
            case States.Patrol:
                break;
            case States.Dead:
                attachedGameObject.transform.Rotate(Vector3.right * 1100.0f); //80 degrees??
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
                //attachedGameObject.animator.Play("TailAttack");
            }
            else
            {
                currentAttack = AdultXenomorphAttacks.AcidSpit;
                //attachedGameObject.animator.Play("AcidSpit");
            }
            Debug.Log("Chestburster current attack: " + currentAttack);
        }
    }

    private void AcidSpit()
    {
        //InternalCalls.InstantiateBullet(attachedGameObject.transform.position + attachedGameObject.transform.forward * 12.5f, attachedGameObject.transform.rotation);
        //attachedGameObject.source.PlayAudio(IAudioSource.EventIDs.E_X_ADULT_SPIT);
        ResetState();
        //if (attachedGameObject.animator.currentAnimHasFinished)
        //{
        //    ResetState();
        //}
    }

    private void TailAttack()
    {
        ResetState();
        //if (attachedGameObject.animator.currentAnimHasFinished)
        //{
        //    ResetState();
        //}
    }

    private void ResetState()
    {
        attackTimer = 0.0f;
        currentAttack = AdultXenomorphAttacks.None;
        currentState = States.Chase;
    }

    public void ReduceLife() //temporary function for the hardcoding of collisions
    {
        life -= 10.0f;
    }

    private void DebugDraw()
    {
        //Draw debug ranges
        //if (gameManager.colliderRender)
        //{
        if (!detected)
        {
            Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, detectedRange, new Vector3(1.0f, 0.8f, 0.0f)); //Yellow
        }
        else
        {
            Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, isCloseRange, new Vector3(1.0f, 0.0f, 0.0f)); //Red
            Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, maxChasingRange, new Vector3(1.0f, 0.0f, 1.0f)); //Purple
        }
        //}
    }
}