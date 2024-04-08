﻿using System;

class AnarchistBehaviour : MonoBehaviour
{
    enum States
    {
        Patrol,
        Inspect,
        Attack,
        Dead
    }

    IGameObject playerGO;
    float playerDistance;
    bool playerDetected = false;

    float rangeToInspect = 200;
    float inspectDetectionRadius = 100;
    float loseRange = 150;

    States lastState = States.Patrol;
    States currentState = States.Patrol;
    Vector3 initialPos;

    float life = 200;
    ICollider2D collider;

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
        initialPos = attachedGameObject.transform.position;

        collider = attachedGameObject.GetComponent<ICollider2D>();
    }

    public override void Update()
    {
        if (currentState == States.Dead) return;

        playerDistance = Vector3.Distance(playerGO.transform.position, attachedGameObject.transform.position);

        UpdateFSMStates();
        DoStateBehaviour();
    }

    void UpdateFSMStates()
    {
        if (life <= 0) { currentState = States.Dead; return; }

        if (playerDistance < rangeToInspect && lastState != States.Inspect)
        {
            currentState = States.Inspect;
        }
    }

    void DoStateBehaviour()
    {
        switch (currentState)
        {
            case States.Patrol:
                PatrolState();
                break;
            case States.Inspect:
                InspectState();
                break;
            case States.Attack:
                AttackState();
                break;
            case States.Dead:
                attachedGameObject.transform.Rotate(Vector3.right * 1100.0f); //80 degrees??
                break;
            default:
                break;
        }
    }

    float patrolRange = 100;
    float patrolSpeed = 20.0f;
    float roundProgress = 0.0f; //Do not modify
    void PatrolState()
    {
        if (currentState != lastState)
        {
            lastState = currentState;
        }

        roundProgress += Time.deltaTime * patrolSpeed;
        if (roundProgress > 360.0f) roundProgress -= 360.0f;

        attachedGameObject.transform.position = initialPos +
                                                Vector3.right * (float)Math.Cos(roundProgress * Math.PI / 180.0f) * patrolRange +
                                                Vector3.forward * (float)Math.Sin(roundProgress * Math.PI / 180.0f) * patrolRange;

        Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 3, rangeToInspect, Vector3.right + Vector3.up);
    }


    enum InspctStates
    {
        Going,
        Inspecting,
        ComingBack
    }
    Vector3 playerLastPosition;
    InspctStates currentSubstate = InspctStates.Going;
    float maxInspectTime = 5.0f;
    float elapsedTime = 0.0f; //Do not modify
    void InspectState()
    {
        if (currentState != lastState)
        {
            Debug.Log("StartingState");
            lastState = currentState;
            playerLastPosition = playerGO.transform.position;
        }

        switch (currentSubstate)
        {
            case InspctStates.Going:
                if (MoveTo(playerLastPosition))
                {
                    currentSubstate = InspctStates.Inspecting;
                }
                attachedGameObject.transform.LookAt(playerLastPosition);
                Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 3, inspectDetectionRadius, Vector3.right + Vector3.up * 0.3f);
                break;
            case InspctStates.Inspecting:
                elapsedTime += Time.deltaTime;
                if (elapsedTime > maxInspectTime)
                {
                    elapsedTime = 0.0f;
                    currentSubstate = InspctStates.ComingBack;
                }
                attachedGameObject.transform.Rotate(Vector3.up * 150.0f * Time.deltaTime);
                Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 3, inspectDetectionRadius, Vector3.right + Vector3.up * 0.5f);
                break;
            case InspctStates.ComingBack:
                if (MoveTo(initialPos))
                {
                    currentSubstate = InspctStates.Going;
                    currentState = States.Patrol;
                }
                attachedGameObject.transform.LookAt(initialPos);
                Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 3, inspectDetectionRadius, Vector3.right + Vector3.up * 0.8f);
                return;
            default:
                break;
        }


        Vector3 directorVec = (attachedGameObject.transform.position - playerGO.transform.position).Normalize();
        float dot = Vector3.Dot(attachedGameObject.transform.forward, directorVec);

        if (playerDistance < inspectDetectionRadius
            && dot > 0.7f)
        {
            currentState = States.Attack;
        }
    }

    float movementSpeed = 50.0f;
    bool MoveTo(Vector3 targetPosition)
    {
        //Return true if arrived at destination
        if (Vector3.Distance(attachedGameObject.transform.position, targetPosition) < 0.5f) return true;

        Vector3 dirVector = (targetPosition - attachedGameObject.transform.position).Normalize();
        attachedGameObject.transform.Translate(dirVector * movementSpeed * Time.deltaTime);

        return false;
    }

    bool shooting = false;
    float timerBetweenBursts = 0.0f; //Do not modify
    float timeBetweenBursts = 0.5f;
    float timerBetweenBullets = 0.0f; //Do not modify
    float timeBetweenBullets = 0.05f;
    int bulletCounter = 0; //Do not modify
    int burstBulletCount = 3;
    void AttackState()
    {
        attachedGameObject.transform.LookAt(playerGO.transform.position);

        if (playerDistance > loseRange)
        {
            currentState = States.Inspect;
            currentSubstate = InspctStates.ComingBack;
        }

        if (!shooting)
        {
            timerBetweenBursts += Time.deltaTime;
            if (timerBetweenBursts > timeBetweenBursts)
            {
                timerBetweenBursts = 0.0f;
                shooting = true;
            }
        }
        else
        {
            timerBetweenBullets += Time.deltaTime;
            if (timerBetweenBullets > timeBetweenBullets)
            {
                InternalCalls.InstantiateBullet(attachedGameObject.transform.position +
                                                attachedGameObject.transform.forward * (collider.radius + 0.5f),
                                                attachedGameObject.transform.rotation);
                timerBetweenBullets = 0.0f;
                bulletCounter++;
            }

            if (bulletCounter >= burstBulletCount)
            {
                bulletCounter = 0;
                shooting = false;
            }
        }

        Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 3, loseRange, Vector3.right);
    }
}
