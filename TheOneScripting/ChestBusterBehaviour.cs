using System;

public class ChestBusterBehaviour : MonoBehaviour
{
    enum States
    {
        Roam,
        Attack,
        Chase,
        Dead
    }

    private enum ChestbursterAttack
    {
        None,
        Tail_Punch,
        Tail_Trip
    }

    IGameObject playerGO;
    Vector3 directorVector;
    float playerDistance;

    // Chestburster parameters
    float life = 80.0f;
    float movementSpeed = 50.0f * 2;
    States currentState = States.Roam;
    ChestbursterAttack currentAttack = ChestbursterAttack.None;

    // Roaming parameters
    private Vector3 roamTarget = new Vector3(0.0f, 0.0f, 0.0f);

    // Ranges
    const float enemyDetectedRange = 35.0f * 1.5f;
    const float maxAttackRange = 60.0f;
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
                    Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, maxAttackRange, new Vector3(0.0f, 0.8f, 1.0f)); //Blue
                }
            //}

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

        if (!detected && playerDistance < enemyDetectedRange) detected = true;

        if (detected)
        //  && !shooting
        {
            if (playerDistance < maxAttackRange)
            {
                //shooting = true;
                currentState = States.Attack;
                //attachedGameObject.source.PlayAudio(AudioManager.EventIDs.ENEMYATTACK);
            }
            else if (playerDistance > maxAttackRange && playerDistance < maxChasingRange)
            {
                currentState = States.Chase;
            }
            else if (playerDistance > maxChasingRange)
            {
                detected = false;
                currentState = States.Roam;
            }
        } 
    }

    void DoStateBehaviour()
    {
        switch (currentState)
        {
            case States.Roam:
                return;
            case States.Attack:
                player.isFighting = true;
                attachedGameObject.transform.LookAt(playerGO.transform.position);
                ChooseAttack();
                if (attackTimer < attackCooldown)
                {
                    attackTimer += Time.deltaTime;
                    if (attackTimer > attackCooldown / 2)
                    {
                       
                        
                    }
                    break;
                }
                attackTimer = 0.0f;
               
                break;
            case States.Chase:
                player.isFighting = true;
                attachedGameObject.transform.LookAt(playerGO.transform.position);
                attachedGameObject.transform.Translate(attachedGameObject.transform.forward * movementSpeed * Time.deltaTime);
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
        
    }

    private void ResetState()
    {

    }

    public void ReduceLife() //temporary function for the hardcoding of collisions
    {
        life -= 10.0f;
    }
}