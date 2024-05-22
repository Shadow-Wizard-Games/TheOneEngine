using System;
using System.Collections.Generic;

public class AlienQueenBehaviourNew : MonoBehaviour
{
    float countDown = 0.0f; //Do not touch

    IGameObject playerGO;

    float maxLife = 1500;
    float currentLife;
    bool isDead = false;

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
        currentLife = maxLife;

        attachedGameObject.animator.Play("Idle");
    }

    public override void Update()
    {
        attachedGameObject.animator.UpdateAnimation();

        //THIS IF IS DEBUG ONLY. DELETE ON COMPLETION
        if (Input.GetKeyboardButton(Input.KeyboardCode.K))
        {
            currentLife -= 10;
            Debug.Log("Current life is: " + currentLife.ToString());
        }

        if (isDead) { return; }

        DoStateBehaviour();

        if (currentLife < 0) { isDead = true; attachedGameObject.animator.Play("Death"); }
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
        ChangingPhase,
        None
    }

    States lastState = States.InitialState;
    States currentState = States.InitialState;

    bool CheckPhaseChange()
    {
        if (currentPhase < 2 && currentLife <= maxLife / 1.5f ||
            currentPhase < 3 && currentLife <= maxLife / 3.0f)
        {
            Debug.LogWarning("Phase changing from " + currentPhase.ToString() + " to " + (currentPhase + 1).ToString());
            currentState = States.ChangingPhase;
            return true;
        }
        return false;
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
            case States.Circle:
                DoCircle();
                break;
            case States.Waiting:
                DoWait();
                break;
            case States.Attack:
                DoAttack();
                break;
            case States.ChangingPhase:
                DoPhaseChange();
                break;
            default:
                attachedGameObject.transform.LookAt2D(playerGO.transform.Position);
                Debug.LogError("Fallen out of FSM");
                break;
        }
        if (rainActive) { AcidRain(); }
    }

    float detectionRange = 400.0f;
    float playerDistance; //Do not touch
    void DoIdle()
    {
        playerDistance = Vector3.Distance(playerGO.transform.Position, attachedGameObject.transform.Position);
        if (playerDistance < detectionRange)
        {
            currentState = States.Zigzag;
        }

        Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up, detectionRange, Color.asianYellow.ToVector3());
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

            attachedGameObject.animator.Play("Run");

            lastPlayerPos = playerGO.transform.Position;
            attachedGameObject.transform.LookAt2D(lastPlayerPos);
            chargeDirectionRightVec = attachedGameObject.transform.Right;
            chargeDirectionForwardVec = attachedGameObject.transform.Forward;
            yProgress = attachedGameObject.transform.Position;
            zigzagPos = 270.0f;
            reachedPoint = false;
        }


        zigzagPos += Time.deltaTime * xCalcSpeed;
        if (zigzagPos > 360.0f) zigzagPos -= 360.0f;

        xProgress = chargeDirectionRightVec * (float)Math.Cos(zigzagPos * Math.PI / 180.0f) * zigzagWidth;

        yProgress += chargeDirectionForwardVec * yCalcSpeed * Time.deltaTime;

        moveToTarget = xProgress + yProgress;

        attachedGameObject.transform.LookAt2D(moveToTarget);
        attachedGameObject.transform.Translate(attachedGameObject.transform.Forward * alienSpeed * Time.deltaTime);

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

    float alienCircleSpeed = 420.0f;
    float circleTime = 5.0f;
    float circleSize = 200.0f;
    float cicleCalcSpeed = 150.0f;
    #region Do not touch variables
    float counterPos;
    Vector3 progressPos;
    bool circleStarted = false;
    #endregion
    void DoCircle()
    {
        if (currentState != lastState)
        {
            Debug.Log("I am now entering circle");
            lastState = currentState;

            attachedGameObject.animator.Play("Run");

            countDown = 0.0f;
            circleStarted = false;
        }

        //Moving towards the player until reaching circle size
        if (!circleStarted &&
            Vector3.Distance(playerGO.transform.Position, attachedGameObject.transform.Position) > circleSize + circleSize / 2)
        {
            MoveTo(playerGO.transform.Position, alienCircleSpeed / 1.5f);
            attachedGameObject.transform.LookAt2D(playerGO.transform.Position);

            //Debug.DrawWireCircle(playerGO.transform.position, circleSize, Color.pitufoBlue.ToVector3());

            return;
        }
        else
        {
            circleStarted = true;

            Vector3 directorVec = playerGO.transform.Position - attachedGameObject.transform.Position;
            counterPos = (float)Math.Atan2(directorVec.z, directorVec.x) * 180.0f / (float)Math.PI + 180.0f;

            if (counterPos < 0) { counterPos += 360.0f; }
        }

        //Running around the player
        counterPos += Time.deltaTime * cicleCalcSpeed;
        if (counterPos > 360.0f) counterPos -= 360.0f;

        progressPos = Vector3.zero;
        progressPos += Vector3.right * (float)Math.Cos(-counterPos * Math.PI / 180.0f) * circleSize;
        progressPos += Vector3.forward * (float)Math.Sin(counterPos * Math.PI / 180.0f) * circleSize;
        progressPos += playerGO.transform.Position;

        //Debug.DrawWireSphere(progressPos, 10.0f, Color.chernobylGreen.ToVector3());

        attachedGameObject.transform.LookAt2D(progressPos);
        attachedGameObject.transform.Translate(attachedGameObject.transform.Forward * alienCircleSpeed * Time.deltaTime);

        countDown += Time.deltaTime;
        if (countDown > circleTime)
        {
            countDown = 0.0f;
            currentState = States.Waiting;
        }
    }

    float waitingTime = 2.0f;
    void DoWait()
    {
        if (currentState != lastState)
        {
            Debug.Log("I am now entering wait");
            lastState = currentState;

            attachedGameObject.animator.Play("Idle");

            countDown = 0.0f;
        }

        attachedGameObject.transform.LookAt2D(playerGO.transform.Position);

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
    bool attackFirstFrame = true;
    bool combinedFirstAttack = true;
    void DoAttack()
    {
        if (currentState != lastState)
        {
            Debug.Log("I am now entering attack");
            currentAttack = ChooseAttack();
            lastState = currentState;
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

                if (combinedFirstAttack)
                {
                    combinedFirstAttack = TailSweep();
                }
                else
                {
                    TailShot();
                }

                break;
            case Attacks.AcidPhase2:
                if (combinedFirstAttack)
                {
                    combinedFirstAttack = AcidBomb();
                }
                else
                {
                    AcidSpit();
                }
                break;
            case Attacks.Jump:
                Jump();
                break;
            case Attacks.Charge:
                HeadCharge();
                break;
            default:
                break;
        }

        if (attackedFinished)
        {
            if (CheckPhaseChange()) { return; }

            attackedFinished = false;

            if (currentPhase == 1) { currentState = States.Zigzag; }
            else if (currentPhase == 2)
            {
                //currentState = (Convert.ToBoolean(new Random().Next(2)))
                //? States.Zigzag
                //: States.Circle;

                if (Convert.ToBoolean(new Random().Next(2)))
                {
                    currentState = States.Zigzag;
                    pathPhase2 = States.Zigzag;
                }
                else
                {
                    currentState = States.Circle;
                    pathPhase2 = States.Circle;
                }
            }
            else
            {
                stagePhase3++;
                if (stagePhase3 > 4) { stagePhase3 = 0; }
                currentAttack = ChooseAttack();
            }

            attackFirstFrame = true;
        }
    }

    void DoPhaseChange()
    {
        if (currentState != lastState)
        {
            Debug.Log("I am now entering PhaseChange");
            lastState = currentState;

            attachedGameObject.animator.Play("Facehugger_Spawn");
        }

        if (attachedGameObject.animator.CurrentAnimHasFinished)
        {
            currentPhase++;
            currentState = States.Attack;
        }
    }

    #endregion

    #region Attacks

    Dictionary<Attacks, float> chancesPhase1 =
    new Dictionary<Attacks, float>
    {
        { Attacks.TailPhase1, 0.3f },
        { Attacks.AcidPhase1, 0.3f },
        { Attacks.Spawn, 0.4f },
    };

    Dictionary<Attacks, float> chancesPhase2 =
    new Dictionary<Attacks, float>
    {
        { Attacks.TailPhase2, 0.7f },
        { Attacks.AcidPhase2, 0.7f },
        { Attacks.Spawn, 0.3f },
    };

    States pathPhase2 = States.Zigzag;
    int stagePhase3 = 0;
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
                Debug.LogError("Queen FSM fell into unintended behaviour when choosing attack (phase 1)");
                return Attacks.None;
            case 2:
                Debug.Log("Branching path was: " + pathPhase2.ToString());
                if (pathPhase2 == States.Circle)
                {
                    randomNum -= chancesPhase2[Attacks.TailPhase2];
                    if (randomNum <= 0) return Attacks.TailPhase2;
                    else return Attacks.Spawn;
                }
                else if (pathPhase2 == States.Zigzag)
                {
                    randomNum -= chancesPhase2[Attacks.AcidPhase2];
                    if (randomNum <= 0) return Attacks.AcidPhase2;
                    else return Attacks.Spawn;
                }
                Debug.LogError("Queen FSM fell into unintended behaviour when choosing attack (phase 2)");
                return Attacks.None;
            case 3:
                switch (stagePhase3)
                {
                    case 0: return Attacks.AcidPhase2;
                    case 1: return Attacks.Jump;
                    case 2: return Attacks.TailPhase2;
                    case 3: return Attacks.Charge;
                    case 4: return Attacks.Spawn;
                    default:
                        Debug.LogError("Queen FSM fell into unintended behaviour when choosing attack (phase 3)");
                        return Attacks.None;
                }

            default:
                Debug.LogError("Queen FSM current phase is invalid");
                return Attacks.None;
        }
    }

    private bool TailSweep()
    {
        if (attackFirstFrame)
        {
            attachedGameObject.animator.Play("Tail_Sweep");
            attackFirstFrame = false;
        }

        if (attachedGameObject.animator.CurrentAnimHasFinished)
        {
            if (currentPhase == 1)
            {
                attackedFinished = true;
            }
            else
            {
                attackFirstFrame = true;
                return false;
            }
        }

        return true;
    }

    private void TailShot()
    {
        if (attackFirstFrame)
        {
            attachedGameObject.animator.Play("Tail_Shot");
            attackFirstFrame = false;
        }

        if (attachedGameObject.animator.CurrentAnimHasFinished)
        {
            combinedFirstAttack = true;
            attackedFinished = true;
        }
    }


    private bool AcidBomb()
    {
        if (attackFirstFrame)
        {
            attachedGameObject.animator.Play("Acid_Bombs");
            attackFirstFrame = false;
        }

        if (attachedGameObject.animator.CurrentAnimHasFinished)
        {
            rainActive = true;
            acidRainCenter = playerGO.transform.position;

            if (currentPhase == 1)
            {
                attackedFinished = true;
            }
            else
            {
                attackFirstFrame = true;
                return false;
            }
        }

        return true;
    }

    //float acidBombDuration = 5.0f;
    //Vector3 height = Vector3.up * 30.0f;
    //bool shot = false; //Do not touch
    private void AcidSpit()
    {
        if (attackFirstFrame)
        {
            attachedGameObject.animator.Play("Acid_Spit");
            attackFirstFrame = false;
        }

        if (attachedGameObject.animator.CurrentAnimHasFinished)
        {
            combinedFirstAttack = true;
            attackedFinished = true;
        }

        //if (!shot)
        //{
        //    shot = true;

        //    InternalCalls.InstantiateBullet(attachedGameObject.transform.position +
        //                                attachedGameObject.transform.forward *
        //                                (attachedGameObject.GetComponent<ICollider2D>().radius + 12.5f) + height,
        //                                attachedGameObject.transform.rotation);
        //}

        //attachedGameObject.transform.LookAt2D(playerGO.transform.position);

        //countDown += Time.deltaTime;
        //if (countDown >= acidBombDuration)
        //{
        //    countDown = 0.0f;

        //    shot = false;
        //    currentAttack = Attacks.None;
        //    attackedFinished = true;
        //}
    }


    float rainArea = 170.0f;
    float timeBetweenRaindrops = 0.4f;
    float rainDuration = 5.0f;
    #region Do not touch variables
    bool rainActive = false;
    float dropsCounter = 0.0f;
    float rainTimeCounter = 0.0f;
    Vector3 acidRainCenter;
    #endregion
    private void AcidRain()
    {
        dropsCounter += Time.deltaTime;
        if (dropsCounter >= timeBetweenRaindrops)
        {
            dropsCounter -= timeBetweenRaindrops;

            Random rand = new Random();
            float angle = (float)(rand.NextDouble() * 2 * Math.PI);

            float radius = rainArea * (float)Math.Sqrt(rand.NextDouble());

            Vector3 localPos = new Vector3(radius * (float)Math.Cos(angle),
                                           0.0f,
                                           radius * (float)Math.Sin(angle));

            InternalCalls.CreatePrefab("Acid_Raindrop", acidRainCenter + localPos, Vector3.zero);
        }

        rainTimeCounter += Time.deltaTime;
        if (rainTimeCounter >= rainDuration) { rainTimeCounter = 0.0f; rainActive = false; }

        Debug.DrawWireCircle(acidRainCenter, rainArea, Color.chernobylGreen.ToVector3());
    }

    float spawnDuration = 5.0f;
    bool spawned = false; //Do not touch
    private void Spawn()
    {
        if (attackFirstFrame)
        {
            attackFirstFrame = false;
        }

        if (!spawned)
        {
            spawned = true;

            Vector3 scale = new Vector3(1, 1, 1);

            InternalCalls.InstantiateXenomorph(attachedGameObject.transform.Position +
                                               attachedGameObject.transform.Forward *
                                               (attachedGameObject.GetComponent<ICollider2D>().radius + 12.5f),
                                               attachedGameObject.transform.Rotation,
                                               scale);
        }

        attachedGameObject.transform.LookAt2D(playerGO.transform.Position);

        countDown += Time.deltaTime;
        if (countDown >= spawnDuration)
        {
            countDown = 0.0f;

            spawned = false;
            currentAttack = Attacks.None;
            attackedFinished = true;
        }
    }

    bool onAir = false;
    bool posCalc = true;
    int jumpNum = 0;
    private void Jump()
    {
        if (attackFirstFrame)
        {
            attachedGameObject.animator.Play("Giant_Stomp");
            attackFirstFrame = false;

            stopCountDown = 0;
            jumpNum = 0;
            posCalc = true;
        }

        stopCountDown += Time.deltaTime;

        if (stopCountDown > 2.43f) { onAir = false; }
        else if (stopCountDown > 1.2f)
        {
            if (posCalc)
            {
                posCalc = false;
                attachedGameObject.transform.Position = new Vector3(playerGO.transform.Position.x,
                                                                    attachedGameObject.transform.Position.y,
                                                                    playerGO.transform.Position.z);
            }
        }
        else if (stopCountDown > 0.9f) { onAir = true; }

        if (onAir) { attachedGameObject.GetComponent<ICollider2D>().radius = 0.0f; }
        else { attachedGameObject.GetComponent<ICollider2D>().radius = 35.0f; }

        if (attachedGameObject.animator.CurrentAnimHasFinished)
        {
            if (jumpNum >= 2)
            {
                attackedFinished = true;
                attachedGameObject.transform.LookAt2D(playerGO.transform.Position);
            }
            else
            {
                jumpNum++;
                posCalc = true;
                stopCountDown = 0;
                attachedGameObject.animator.Play("Giant_Stomp");
            }
        }
    }

    float headChargeDuration = 3.0f;
    float chargeSpeed = 250.0f;
    float stopCountDown = 0; //Do not touch
    private void HeadCharge()
    {
        if (attackFirstFrame)
        {
            attachedGameObject.animator.Play("Head_Charge");
            attackFirstFrame = false;

            countDown = 0;
            stopCountDown = 0;
            attachedGameObject.transform.LookAt2D(playerGO.transform.Position);
        }

        stopCountDown += Time.deltaTime;
        if (stopCountDown > 1.05f && stopCountDown < 3.15f)
        {
            attachedGameObject.transform.Translate(attachedGameObject.transform.Forward * chargeSpeed * Time.deltaTime);
        }

        countDown += Time.deltaTime;
        if (countDown >= headChargeDuration)
        {
            attachedGameObject.transform.LookAt2D(playerGO.transform.Position);
            attackedFinished = true;
        }
    }

    #endregion

    bool MoveTo(Vector3 targetPosition, float speed)
    {
        //Return true if arrived at destination
        if (Vector3.Distance(attachedGameObject.transform.Position, targetPosition) < 0.5f) return true;

        Vector3 dirVector = (targetPosition - attachedGameObject.transform.Position).Normalize();
        attachedGameObject.transform.Translate(dirVector * speed * Time.deltaTime);
        return false;
    }
}