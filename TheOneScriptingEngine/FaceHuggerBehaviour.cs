using System;

public class FaceHuggerBehaviour : MonoBehaviour
{
    enum States
    {
        Idle,
        Attack,
        Jump,
        Dead
    }

    IGameObject playerGO;
    Vector3 directorVector;
    float playerDistance;

    float life = 50;

    float movementSpeed = 35.0f * 2;

    States currentState = States.Idle;
    bool detected = false;

    float enemyDetectedRange = 180.0f;
    float maxAttackRange = 90.0f;
    float maxChasingRange = 180.0f;
    float maxHeight = 20.0f;

    float currentTimer = 0.0f;
    float attackCooldown = 2.0f;

    bool isExploring = true;
    AudioManager.EventIDs currentID = 0;

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
    }

    public override void Update()
    {
        if (currentState == States.Dead) return;

        if (attachedGameObject.transform.ComponentCheck())
        {
            //Draw debug ranges
            if (!detected)
            {
                Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, enemyDetectedRange, new Vector3(1.0f, 0.8f, 0.0f)); //Yellow
            }
            else
            {
                Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, maxChasingRange, new Vector3(0.9f, 0.0f, 0.9f)); //Purple
                Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, maxAttackRange, new Vector3(0.0f, 0.8f, 1.0f)); //Blue
            }

            //Set the director vector and distance to the player
            directorVector = (playerGO.transform.position - attachedGameObject.transform.position).Normalize();
            playerDistance = Vector3.Distance(playerGO.transform.position, attachedGameObject.transform.position);

            UpdateFSMStates();
            DoStateBehaviour();
        }
    }

    void UpdateFSMStates()
    {
        if (life <= 0) { currentState = States.Dead; return; }

        if (!detected && playerDistance < enemyDetectedRange) detected = true;

        if (detected)
        {
            if (playerDistance < maxAttackRange)
            {
                currentState = States.Jump;
            }
            else if (playerDistance > maxAttackRange && playerDistance < maxChasingRange)
            {
                currentState = States.Attack;
                //isExploring = false;
            }
            else if (playerDistance > maxChasingRange)
            {
                detected = false;
                currentState = States.Idle;
                //isExploring = true;
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
                attachedGameObject.transform.LookAt(playerGO.transform.position);
                attachedGameObject.transform.Translate(attachedGameObject.transform.forward * movementSpeed * Time.deltaTime);
                currentTimer = 0.0f;
                break;
            case States.Jump:

                break;
            case States.Dead:
                attachedGameObject.transform.Rotate(Vector3.right * 1100.0f); //80 degrees??
                break;
            default:
                break;
        }
    }

    public void ReduceLife() //temporary function for the hardcoding of collisions
    {
        life -= 10.0f;
    }
}