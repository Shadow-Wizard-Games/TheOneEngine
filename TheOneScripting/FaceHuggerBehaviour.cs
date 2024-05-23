﻿using System;

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

    readonly float life = 50;

    float movementSpeed = 35.0f * 2;

    States currentState = States.Idle;
    bool detected = false;

    readonly float enemyDetectedRange = 180.0f;
    readonly float maxAttackRange = 90.0f;
    readonly float maxChasingRange = 180.0f;

    bool isJumping = false;

    PlayerScript player;
    GameManager gameManager;

    IGameObject jumpPSGO;
    IGameObject deathPSGO;

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        gameManager = IGameObject.Find("GameManager").GetComponent<GameManager>();

        jumpPSGO = attachedGameObject.FindInChildren("JumpPS");
        deathPSGO = attachedGameObject.FindInChildren("DeathPS");
    }

    public override void Update()
    {
        if (currentState == States.Dead) return;

        if (attachedGameObject.transform.ComponentCheck())
        {
            //Draw debug ranges
            if (gameManager.colliderRender)
            {
                if (!detected)
                {
                    Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 4, enemyDetectedRange, new Vector3(1.0f, 0.8f, 0.0f)); //Yellow
                }
                else
                {
                    Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 4, maxChasingRange, new Vector3(0.9f, 0.0f, 0.9f)); //Purple
                    Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 4, maxAttackRange, new Vector3(0.0f, 0.8f, 1.0f)); //Blue
                }
            }

            //Set the director vector and distance to the player
            directorVector = (playerGO.transform.Position - attachedGameObject.transform.Position).Normalize();
            playerDistance = Vector3.Distance(playerGO.transform.Position, attachedGameObject.transform.Position);

            UpdateFSMStates();
            DoStateBehaviour();
        }

        if (isJumping)
        {
            attachedGameObject.source.Play(IAudioSource.AudioEvent.E_FH_JUMP);
            isJumping = false;
        }
    }

    void UpdateFSMStates()
    {
        if (life <= 0) 
        { 
            currentState = States.Dead;
            player.shieldKillCounter++;
            return; 
        }

        if (!detected && playerDistance < enemyDetectedRange) detected = true;

        if (detected)
        {
            if (currentState == States.Jump)
            {
                if (playerDistance > maxAttackRange && playerDistance < maxChasingRange)
                {
                    currentState = States.Dead;
                    attachedGameObject.transform.Position = new Vector3(attachedGameObject.transform.Position.x,
                                                                        0.0f,
                                                                        attachedGameObject.transform.Position.z);

                    attachedGameObject.source.Play(IAudioSource.AudioEvent.E_FH_DEATH);
                    detected = false;
                    //deathPSGO.GetComponent<IParticleSystem>().Replay();
                }
            }
            else
            {
                if (playerDistance < maxAttackRange)
                {
                    currentState = States.Jump;
                    if (jumpPSGO != null)
                    {
                        jumpPSGO.GetComponent<IParticleSystem>().Replay();
                    }
                    isJumping = true;
                }
                else if (playerDistance > maxAttackRange && playerDistance < maxChasingRange)
                {
                    currentState = States.Attack;
                }
                else if (playerDistance > maxChasingRange)
                {
                    detected = false;
                    currentState = States.Idle;
                }
            }
        }
    }

    readonly float maxHeight = 275.0f;
    float height = 0.0f;
    bool up = true;
    void DoStateBehaviour()
    {
        switch (currentState)
        {
            case States.Idle:
                return;
            case States.Attack:
                player.isFighting = true;
                attachedGameObject.transform.LookAt2D(playerGO.transform.Position);
                attachedGameObject.transform.Translate(attachedGameObject.transform.Forward * movementSpeed * Time.deltaTime);
                break;
            case States.Jump:
                player.isFighting = true;
                attachedGameObject.transform.Translate(attachedGameObject.transform.Forward * movementSpeed * 3.0f * Time.deltaTime);
                if (up)
                {
                    if (maxHeight > height)
                    {
                        attachedGameObject.transform.Translate(Vector3.up * height * Time.deltaTime);
                        height = height + 10.0f;
                    }
                    else if (maxHeight <= height)
                    {
                        up = false;
                    }
                }
                else
                {
                    if (height <= 0)
                    {
                        height = 0.0f;

                    }
                    else
                    {
                        attachedGameObject.transform.Translate(Vector3.up * -height * Time.deltaTime);
                        height = height - 20.0f;
                    }
                }
                break;
            case States.Dead:
                attachedGameObject.transform.Rotate(Vector3.right * 1100.0f); //80 degrees??
                attachedGameObject.transform.Translate(Vector3.up * -1.0f * Time.deltaTime);

                break;
            default:
                break;
        }
    }

    bool hitPlayer = false;
    public void CheckJump() //temporary function for the hardcoding of collisions
    {
        if (!hitPlayer)
        {
            playerGO.GetComponent<PlayerScript>().ReduceLife();
            hitPlayer = true;
        }
    }
}