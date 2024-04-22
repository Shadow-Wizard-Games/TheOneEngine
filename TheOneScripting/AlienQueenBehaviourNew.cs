using System;

public class AlienQueenBehaviourNew : MonoBehaviour
{
    enum States
    {
        Waiting,
        Zigzag,
        None
    }

    States lastState = States.Waiting;
    States currentState = States.Waiting;

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
            case States.Waiting:
                DoIdle();
                break;
            case States.Zigzag:
                DoZigzag();
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
            currentState = States.None;
        }
    }
}