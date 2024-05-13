﻿using System;

public class ChestbursterBehaviour : MonoBehaviour
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
        TailPunch,
        TailTrip
    }

    IGameObject playerGO;
    //Vector3 directorVector;
    float playerDistance;

    // Chestburster parameters
    float life = 80.0f;
    float movementSpeed = 30.0f * 2;
    States currentState = States.Idle;
    ChestbursterAttack currentAttack = ChestbursterAttack.None;

    // Tail Trip, might remove this variables due to the animations
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

        attachedGameObject.animator.Play("Move");
        attachedGameObject.animator.blend = false;
        attachedGameObject.animator.transitionTime = 0.0f;
    }

    public override void Update()
    {
        attachedGameObject.animator.UpdateAnimation();

        if (currentState == States.Dead) return;

        if (attachedGameObject.transform.ComponentCheck())
        {
            DebugDraw();

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
                //Debug.Log("Player is now CLOSE");
            }

            if (playerDistance >= farRangeThreshold && isClose)
            {
                isClose = false;
                //Debug.Log("Player is now FAR");
            }

            if (playerDistance > maxChasingRange && detected)
            {
                detected = false;
                currentState = States.Idle;
                //Debug.Log("Player is NOT DETECTED ANYMORE");
            }

            if (currentAttack == ChestbursterAttack.None)
            {
                attackTimer += Time.deltaTime;
                attachedGameObject.animator.Play("Run");
            }

            if (currentAttack == ChestbursterAttack.None && attackTimer >= attackCooldown)
            {
                //Debug.Log("Attempt to attack");
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
                    attachedGameObject.transform.LookAt2D(playerGO.transform.position);

                break;
            case States.ChaseAttack:
                player.isFighting = true;
                attachedGameObject.transform.LookAt2D(playerGO.transform.position);

                ChooseAttack();

                switch (currentAttack)
                {
                    case ChestbursterAttack.TailPunch:
                        //Debug.Log("TAIL PUNCH");
                        TailPunch();
                        break;
                    case ChestbursterAttack.TailTrip:
                        //Debug.Log("TAIL TRIP");
                        TailTrip();
                        break;
                    default:
                        break;
                }
                break;
            case States.Dead:
                attachedGameObject.animator.Play("Dead");
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
                currentAttack = ChestbursterAttack.TailPunch;
                attachedGameObject.animator.Play("TailPunch");
            }
            else
            {
                currentAttack = ChestbursterAttack.TailTrip;
                attachedGameObject.animator.Play("TailTrip");
            }
            //Debug.Log("Chestburster current attack: " + currentAttack);
        }
    }

    private void TailPunch()
    {
        //Debug.Log("Attempt to do TailPunch");

        if (attachedGameObject.animator.currentAnimHasFinished)
        {
            ResetState();
        }

    }

    private void TailTrip()
    {
        //Debug.Log("Attempt to do TailTrip");

        if (attachedGameObject.animator.currentAnimHasFinished)
        {
            ResetState();
        }

    }

    private void ResetState()
    {
        //Debug.Log("Reset State");
        attackTimer = 0.0f;
        currentAttack = ChestbursterAttack.None;
        currentState = States.Idle;
        attachedGameObject.animator.Play("Run");
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
                Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, farRangeThreshold, new Vector3(0.0f, 0.8f, 1.0f)); //Blue
            }
        }
    }
}