using System;
using System.Collections.Generic;

public class AlienQueenBehaviourNew : MonoBehaviour
{
    uint currentPhase = 1;

    enum States
    {
        InitialState,
        Zigzag,
        Waiting,
        None
    }

    States lastState = States.InitialState;
    States currentState = States.InitialState;

    IGameObject playerGO;
    float playerDistance;

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
    }

    public override void Update()
    {
        playerDistance = Vector3.Distance(playerGO.transform.position, attachedGameObject.transform.position);
        DoStateBehaviour();
    }

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
            case States.Waiting:
                DoWait();
                break;
            default:
                attachedGameObject.transform.LookAt(playerGO.transform.position);
                break;
        }
    }

    float detectionRange = 400.0f;
    void DoIdle()
    {
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
            lastState = currentState;

            lastPlayerPos = playerGO.transform.position;
            attachedGameObject.transform.LookAt(lastPlayerPos);
            chargeDirectionRightVec = attachedGameObject.transform.right;
            chargeDirectionForwardVec = attachedGameObject.transform.forward;
            yProgress = attachedGameObject.transform.position;
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

    float waitingTime = 3.0f;
    float elapsedTime = 0.0f; //Do not touch
    void DoWait()
    {
        if (currentState != lastState)
        {
            elapsedTime = 0;
        }

        elapsedTime += Time.deltaTime;
        if (elapsedTime > waitingTime)
        {
            elapsedTime = 0.0f;
            currentState = nextState;
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
    void DoAttack()
    {
        switch (currentAttack)
        {
            case Attacks.TailPhase1:
                break;
            case Attacks.AcidPhase1:
                break;
            case Attacks.Spawn:
                break;
            //case Attacks.TailPhase2:
            //    break;
            //case Attacks.AcidPhase2:
            //    break;
            //case Attacks.Jump:
            //    break;
            //case Attacks.Charge:
            //    break;
            default:
                break;
        }
    }

    Dictionary<Attacks, float> chancesPhase1 =
    new Dictionary<Attacks, float>
    {
        { Attacks.TailPhase1, 0.3f },
        { Attacks.AcidPhase1, 0.3f },
        { Attacks.Spawn, 0.4f },
    };

    Attacks ChooseAttack()
    {
        //Random rand = new Random();
        //return (Attacks)rand.Next(3); //Phase 1
        //return (Attacks)rand.Next(2, 5); //Phase 2
        //return (Attacks)rand.Next(2, 7); //Phase 3

        if (currentPhase > 2)
        {
            //choose next attack in phase 3
            //return attack;
        }


        Random rand = new Random();
        float randomNum = (float)rand.NextDouble();

        Dictionary<Attacks, float> currentPhaseChances;

        (currentPhase == 1) ? currentPhaseChances = chancesPhase1 : currentPhaseChances = chancesPhase2;

        foreach (KeyValuePair<Attacks, float> entry in chancesPhase1)
        {

        }
    }
}