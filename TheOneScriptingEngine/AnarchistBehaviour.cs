﻿using System;

internal class AnarchistBehaviour : MonoBehaviour
{
    enum States
    {
        Idle,
        Attack,
        Chase,
        Patrol,
        Dead
    }

    IGameObject playerGO;
    Vector3 directorVector;
    float playerDistance;

    float life = 200;

    float movementSpeed = 35.0f * 3;

    States currentState = States.Idle;
    bool detected = false;

    float enemyDetectedRange = 35.0f * 3;
    float maxAttackRange = 90.0f;
    float maxChasingRange = 180.0f;

    bool shooting = false;
    bool hasShot = false;
    float currentTimer = 0.0f;
    float attackCooldown = 2.0f;

    bool lastFrameToMove = false;
    bool toMove = false;

    PlayerScript player;

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();
    }

    public override void Update()
    {
        if (currentState == States.Dead) return;

        toMove = false;

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

        // Play steps
        if (lastFrameToMove != toMove)
        {
            if (toMove)
            {
                attachedGameObject.source.PlayAudio(AudioManager.EventIDs.P_STEP);
            }
            else
            {
                attachedGameObject.source.StopAudio(AudioManager.EventIDs.P_STEP);
            }
            lastFrameToMove = toMove;
        }
    }

    void UpdateFSMStates()
    {
        if (life <= 0) { currentState = States.Dead; return; }

        if (!detected && playerDistance < enemyDetectedRange) detected = true;

        if (detected && !shooting)
        {
            if (playerDistance < maxAttackRange)
            {
                shooting = true;
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
                attachedGameObject.transform.LookAt(playerGO.transform.position);
                if (currentTimer < attackCooldown)
                {
                    currentTimer += Time.deltaTime;
                    if (!hasShot && currentTimer > attackCooldown / 2)
                    {
                        InternalCalls.InstantiateBullet(attachedGameObject.transform.position + attachedGameObject.transform.forward * 12.5f, attachedGameObject.transform.rotation);
                        attachedGameObject.source.PlayAudio(AudioManager.EventIDs.E_REBEL_SHOOT);
                        hasShot = true;
                    }
                    break;
                }
                currentTimer = 0.0f;
                hasShot = false;
                shooting = false;
                break;
            case States.Chase:
                player.isFighting = true;
                attachedGameObject.transform.LookAt(playerGO.transform.position);
                attachedGameObject.transform.Translate(attachedGameObject.transform.forward * movementSpeed * Time.deltaTime);
                toMove = true;
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

    public void ReduceLife() //temporary function for the hardcoding of collisions
    {
        life -= 10.0f;
    }
}