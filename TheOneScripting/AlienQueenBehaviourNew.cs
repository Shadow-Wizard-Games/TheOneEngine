using System;
using System.Collections.Generic;

public class AlienQueenBehaviourNew : MonoBehaviour
{
    IGameObject playerGO;
    float countDown = 0.0f; //Do not touch
    
    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
    }

    public override void Update()
    {
        DoStateBehaviour();
    }

    #region FSM

    uint currentPhase = 1;

    enum States
    {
        InitialState,
        Zigzag,
        Circle,
        Waiting,
        Attack,
        None
    }

    States lastState = States.InitialState;
    States currentState = States.InitialState;

    void DoStateBehaviour()
    {
        switch (currentState)
        {
            case States.InitialState:
                DoIdle();
                break;
            case States.Zigzag:
                DoZigzag();
                break;
            case States.Circle:
                DoCircle();
                break;
            case States.Waiting:
                DoWait();
                break;
            case States.Attack:
                DoAttack();
                break;
            default:
                attachedGameObject.transform.LookAt(playerGO.transform.position);
                Debug.LogError("Fallen out of FSM");
                break;
        }
    }

    float detectionRange = 400.0f;
    float playerDistance; //Do not touch
    void DoIdle()
    {
        playerDistance = Vector3.Distance(playerGO.transform.position, attachedGameObject.transform.position);
        if (playerDistance < detectionRange)
        {
            currentState = States.Zigzag;
        }

        Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up, detectionRange, Color.asianYellow.ToVector3());
    }

    float alienSpeed = 300;
    float zigzagWidth = 200;
    float xCalcSpeed = 250.0f;
    float yCalcSpeed = 200.0f;
    #region Do not touch variables
    Vector3 xProgress;
    Vector3 yProgress;

    Vector3 lastPlayerPos;
    Vector3 chargeDirectionRightVec;
    Vector3 chargeDirectionForwardVec;

    Vector3 moveToTarget;

    float zigzagPos = 270.0f;

    bool reachedPoint = false;
    #endregion
    void DoZigzag()
    {
        if (currentState != lastState)
        {
            Debug.Log("I am now entering zigzag");

            lastState = currentState;

            lastPlayerPos = playerGO.transform.position;
            attachedGameObject.transform.LookAt(lastPlayerPos);
            chargeDirectionRightVec = attachedGameObject.transform.right;
            chargeDirectionForwardVec = attachedGameObject.transform.forward;
            yProgress = attachedGameObject.transform.position;
            zigzagPos = 270.0f;
            reachedPoint = false;
        }


        zigzagPos += Time.deltaTime * xCalcSpeed;
        if (zigzagPos > 360.0f) zigzagPos -= 360.0f;

        xProgress = chargeDirectionRightVec * (float)Math.Cos(zigzagPos * Math.PI / 180.0f) * zigzagWidth;

        yProgress += chargeDirectionForwardVec * yCalcSpeed * Time.deltaTime;

        moveToTarget = xProgress + yProgress;

        attachedGameObject.transform.LookAt(moveToTarget);
        attachedGameObject.transform.Translate(attachedGameObject.transform.forward * alienSpeed * Time.deltaTime);

        //Debug.DrawWireSphere(xProgress, 10.0f, Color.russianRed.ToVector3());
        //Debug.DrawWireSphere(yProgress, 10.0f, Color.scrumMasterBlue.ToVector3());
        //Debug.DrawWireSphere(moveToTarget, 10.0f, Color.asianYellow.ToVector3());

        if (Vector3.Distance(yProgress, lastPlayerPos) < 5.0f)
        {
            reachedPoint = true;
        }
        if (reachedPoint && ((zigzagPos > 80 && zigzagPos < 100) || (zigzagPos > 260 && zigzagPos < 280)))
        {
            currentState = States.Waiting;
        }
    }

    float circleTime = 5.0f;
    void DoCircle()
    {
        //Implement circle around player
        Debug.Log("Doing Circle");

        countDown += Time.deltaTime;
        if (countDown > circleTime)
        {
            countDown = 0.0f;
            currentState = States.Attack;
        }

    }

    float waitingTime = 3.0f;
    void DoWait()
    {
        if (currentState != lastState)
        {
            Debug.Log("I am now entering wait");
            lastState = currentState;
            countDown = 0.0f;
        }

        attachedGameObject.transform.LookAt(playerGO.transform.position);

        countDown += Time.deltaTime;
        if (countDown > waitingTime)
        {
            countDown = 0.0f;
            currentState = States.Attack;
        }
    }

    enum Attacks
    {
        TailPhase1, //1
        AcidPhase1, //1
        Spawn,      //1 2 3
        TailPhase2, //2 3
        AcidPhase2, //2 3
        Jump,       //3
        Charge,     //3
        None
    }

    Attacks currentAttack = Attacks.None;
    bool attackedFinished = false;
    void DoAttack()
    {
        if (currentState != lastState)
        {
            Debug.Log("I am now entering attack");
            lastState = currentState;
            currentAttack = ChooseAttack();
        }

        switch (currentAttack)
        {
            case Attacks.TailPhase1:
                TailSweep();
                break;
            case Attacks.AcidPhase1:
                AcidBomb();
                break;
            case Attacks.Spawn:
                Spawn();
                break;
            case Attacks.TailPhase2:
                break;
            case Attacks.AcidPhase2:
                break;
            //case Attacks.Jump:
            //    break;
            //case Attacks.Charge:
            //    break;
            default:
                break;
        }

        if (attackedFinished)
        {
            attackedFinished = false;

            if (currentPhase == 1) { currentState = States.Zigzag; }
            else
            {
                currentState = (Convert.ToBoolean(new Random().Next(2)))
                ? States.Zigzag
                : States.Circle;
            }
        }
    }

    #endregion

    #region Attacks

    Dictionary<Attacks, float> chancesPhase1 =
    new Dictionary<Attacks, float>
    {
        { Attacks.TailPhase1, 0.05f },
        { Attacks.AcidPhase1, 0.9f },
        { Attacks.Spawn, 0.05f },
    };

    Dictionary<Attacks, float> chancesPhase2 =
    new Dictionary<Attacks, float>
    {
        { Attacks.TailPhase2, 0.7f },
        { Attacks.AcidPhase2, 0.7f },
        { Attacks.Spawn, 0.3f },
    };

    Attacks ChooseAttack()
    {
        Random rand = new Random();
        float randomNum = (float)rand.NextDouble();

        switch (currentPhase)
        {
            case 1:
                foreach (KeyValuePair<Attacks, float> entry in chancesPhase1)
                {
                    randomNum -= entry.Value;
                    if (randomNum <= 0)
                    {
                        Debug.Log("Doing attack: " + entry.Key.ToString());
                        return entry.Key;
                    }
                }
                Debug.LogError("Queen FSM fell into unintended behaviour");
                return Attacks.None;
            case 2:
                if (currentState == States.Circle)
                {
                    randomNum -= chancesPhase2[Attacks.TailPhase2];
                    if (randomNum <= 0) return Attacks.TailPhase2;
                    else return Attacks.Spawn;
                }
                else if (currentState == States.Zigzag)
                {
                    randomNum -= chancesPhase2[Attacks.AcidPhase2];
                    if (randomNum <= 0) return Attacks.AcidPhase2;
                    else return Attacks.Spawn;
                }
                Debug.LogError("Queen FSM fell into unintended behaviour");
                return Attacks.None;
            case 3:
                Debug.LogWarning("Queen FSM phase 3 pending to implement");
                return Attacks.None;

            default:
                Debug.LogError("Queen FSM current phase is invalid");
                return Attacks.None;
        }
    }

    float spinDuration = 5.0f;
    bool isSpinning = false; //Do not touch
    private void TailSweep()
    {
        //float damage = 15;
        if (!isSpinning) isSpinning = true;

        attachedGameObject.transform.Rotate(Vector3.up * -180 * Time.deltaTime);

        countDown += Time.deltaTime;
        if (countDown >= spinDuration)
        {
            countDown = 0.0f;
            isSpinning = false;

            currentAttack = Attacks.None;
            attackedFinished = true;
        }
    }

    float acidBombDuration = 5.0f;
    Vector3 height = Vector3.up * 30.0f;
    bool shot = false; //Do not touch
    private void AcidBomb()
    {
        if (!shot)
        {
            shot = true;

            InternalCalls.InstantiateBullet(attachedGameObject.transform.position +
                                        attachedGameObject.transform.forward *
                                        (attachedGameObject.GetComponent<ICollider2D>().radius + 12.5f) + height,
                                        attachedGameObject.transform.rotation);
        }

        countDown += Time.deltaTime;
        if (countDown >= acidBombDuration)
        {
            countDown = 0.0f;

            shot = false;
            currentAttack = Attacks.None;
            attackedFinished = true;
        }
        //ResetState();
    }

    float spawnDuration = 5.0f;
    bool spawned = false; //Do not touch
    private void Spawn()
    {
        if (!spawned)
        {
            spawned = true;

            Vector3 scale = new Vector3(1, 1, 1);

            InternalCalls.InstantiateXenomorph(attachedGameObject.transform.position +
                                               attachedGameObject.transform.forward *
                                               (attachedGameObject.GetComponent<ICollider2D>().radius + 12.5f),
                                               attachedGameObject.transform.rotation,
                                               scale);
        }

        countDown += Time.deltaTime;
        if (countDown >= spawnDuration)
        {
            countDown = 0.0f;

            spawned = false;
            currentAttack = Attacks.None;
            attackedFinished = true;
        }
        //ResetState();
    }

    #endregion
}