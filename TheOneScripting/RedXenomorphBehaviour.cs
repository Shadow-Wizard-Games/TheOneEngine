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

    // Adult Xenomorph parameters
    float life = 200.0f;
    float movementSpeed = 40.0f * 3;
    States currentState = States.Idle;
    RedXenomorphAttacks currentAttack = RedXenomorphAttacks.None;

    // Ranges
    const float farRangeThreshold = 50.0f * 3;
    const float enemyDetectedRange = 35.0f * 3;
    const float maxAttackRange = 90.0f;
    const float maxChasingRange = 180.0f;

    // Flags
    bool detected = false;

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
    }

    public override void Update()
    {
        if (currentState == States.Dead) return;

        if (attachedGameObject.transform.ComponentCheck())
        {
            //Set the director vector and distance to the player
            directorVector = (playerGO.transform.position - attachedGameObject.transform.position).Normalize();
            playerDistance = Vector3.Distance(playerGO.transform.position, attachedGameObject.transform.position);

            UpdateFSMStates();
            DoStateBehaviour();
        }

        DebugDraw();
    }

    void UpdateFSMStates()
    {
        if (life <= 0) { currentState = States.Dead; return; }

        if (!detected && playerDistance < enemyDetectedRange) detected = true;

        if (detected)
        {
            if (playerDistance < maxAttackRange)
            {
                currentState = States.Attack;
            }
            else if (playerDistance > maxAttackRange && playerDistance < maxChasingRange)
            {
                currentState = States.Chase;
            }
            else if (playerDistance > maxChasingRange)
            {
                detected = false;
                currentState = States.Idle;
            }

            if (currentAttack == RedXenomorphAttacks.None)
            {
                attackTimer += Time.deltaTime;
            }

            if (currentAttack == RedXenomorphAttacks.None && attackTimer >= attackCooldown)
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
                attachedGameObject.transform.LookAt2D(playerGO.transform.position);
                ChooseAttack();

                switch (currentAttack)
                {
                    case RedXenomorphAttacks.SpikeThrow:
                        AcidSpit();
                        break;
                    case RedXenomorphAttacks.TailStab:
                        TailAttack();
                        break;
                    default:
                        break;
                }

                break;
            case States.Chase:
                player.isFighting = true;
                attachedGameObject.transform.LookAt2D(playerGO.transform.position);
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

    }

    private void AcidSpit()
    {
        //InternalCalls.InstantiateBullet(attachedGameObject.transform.position + attachedGameObject.transform.forward * 12.5f, attachedGameObject.transform.rotation);
        attachedGameObject.source.PlayAudio(IAudioSource.EventIDs.E_X_ADULT_SPIT);
        ResetState();
    }

    private void TailAttack()
    {

    }

    private void ResetState()
    {
        attackTimer = 0.0f;
    }

    public void ReduceLife() //temporary function for the hardcoding of collisions
    {
        life -= 10.0f;
    }

    private void DebugDraw()
    {
        //Draw debug ranges
        if (gameManager.colliderRender)
        {
            if (!detected)
            {
                Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, enemyDetectedRange, new Vector3(1.0f, 0.8f, 0.0f)); //Yellow
            }
            else
            {
                Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, maxChasingRange, new Vector3(0.9f, 0.0f, 0.9f)); //Purple
                Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, maxAttackRange, new Vector3(0.0f, 0.8f, 1.0f)); //Blue
            }
            Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, farRangeThreshold, new Vector3(1.0f, 0.0f, 1.0f)); //Purple
        }
    }
}