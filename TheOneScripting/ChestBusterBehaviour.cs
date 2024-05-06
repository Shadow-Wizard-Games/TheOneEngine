using System;

public class ChestBusterBehaviour : MonoBehaviour
{
    enum States
    {
        Idle,
        ChaseAttack,
        Dead
    }

    private enum ChestbursterAttack
    {
        None,
        Tail_Punch,
        Tail_Trip
    }

    IGameObject playerGO;
    //Vector3 directorVector;
    float playerDistance;

    // Chestburster parameters
    float life = 80.0f;
    float movementSpeed = 30.0f * 2;
    States currentState = States.Idle;
    ChestbursterAttack currentAttack = ChestbursterAttack.None;

    // Tail Trip
    private float spinElapsed = 0.0f;
    private const float spinTime = 2.0f;

    // Ranges
    const float enemyDetectedRange = 35.0f * 1.5f;
    const float farRangeThreshold = 60.0f;
    const float maxChasingRange = 150.0f;

    // Flags
    bool isClose = false;
    bool detected = false;

    // Timers
    float attackTimer = 0.0f;
    const float attackCooldown = 4.0f;

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
            //Draw debug ranges
            //if (gameManager.colliderRender)
            //{
            if (!detected)
            {
                Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, enemyDetectedRange, new Vector3(1.0f, 0.8f, 0.0f)); //Yellow
            }
            else
            {
                Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, maxChasingRange, new Vector3(0.9f, 0.0f, 0.9f)); //Purple
                Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, farRangeThreshold, new Vector3(0.0f, 0.8f, 1.0f)); //Blue
            }
            //}

            //Set the director vector and distance to the player
            //directorVector = (playerGO.transform.position - attachedGameObject.transform.position).Normalize();
            playerDistance = Vector3.Distance(playerGO.transform.position, attachedGameObject.transform.position);

            UpdateFSM();
            DoStateBehaviour();
        }
    }

    void UpdateFSM()
    {
        if (life <= 0) { currentState = States.Dead; return; }

        if (!detected && playerDistance < enemyDetectedRange) detected = true;

        if (detected)
        {

            attachedGameObject.transform.Translate(attachedGameObject.transform.forward * movementSpeed * Time.deltaTime);

            if (playerDistance < farRangeThreshold && !isClose)
            {
                isClose = true;
                Debug.Log("Player is now CLOSE");
            }

            if (playerDistance >= farRangeThreshold && isClose)
            {
                isClose = false;
                Debug.Log("Player is now FAR");
            }

            if (playerDistance > maxChasingRange && detected)
            {
                detected = false;
                currentState = States.Idle;
                Debug.Log("Player is NOT DETECTED ANYMORE");
            }

            if (currentAttack == ChestbursterAttack.None)
            {
                attackTimer += Time.deltaTime;
            }

            if (currentAttack == ChestbursterAttack.None && attackTimer >= attackCooldown)
            {
                Debug.Log("Attempt to attack");
                currentState = States.ChaseAttack;
            }
        }
    }

    void DoStateBehaviour()
    {
        switch (currentState)
        {
            case States.Idle:
                if (currentAttack == ChestbursterAttack.None && detected)
                    attachedGameObject.transform.LookAt(playerGO.transform.position);

                if (!detected) ResetState();
                
                break;
            case States.ChaseAttack:
                player.isFighting = true;

                ChooseAttack();

                switch (currentAttack)
                {
                    case ChestbursterAttack.Tail_Punch:
                        Debug.Log("TAIL PUNCH");
                        TailPunch();
                        break;
                    case ChestbursterAttack.Tail_Trip:
                        Debug.Log("TAIL TRIP");
                        TailTrip();
                        break;
                    default:
                        break;
                }
                break;
            case States.Dead:
                attachedGameObject.transform.Rotate(Vector3.right * 1100.0f);
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
                currentAttack = ChestbursterAttack.Tail_Punch;
            }
            else
            {
                currentAttack = ChestbursterAttack.Tail_Trip;
            }
            Debug.Log("Chestburster current attack: " + currentAttack);
        }
    }

    private void TailPunch()
    {
        Debug.Log("Attempt to do TailPunch");
        ResetState();
    }

    private void TailTrip()
    {
        float damage = 5.0f;
        spinElapsed += Time.deltaTime;
        attachedGameObject.transform.Rotate(Vector3.up * -180 * Time.deltaTime);
        Debug.Log("Attempt to do TailTrip");
        if (spinElapsed >= spinTime)
        {
            spinElapsed = 0.0f;
            ResetState();
        }
    }

    private void ResetState()
    {
        attackTimer = 0.0f;
        currentAttack = ChestbursterAttack.None;
        currentState = States.Idle;
    }

    public void ReduceLife() //temporary function for the hardcoding of collisions
    {
        life -= 10.0f;
    }
}