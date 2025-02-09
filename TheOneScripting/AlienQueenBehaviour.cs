﻿using System;

public class AlienQueenBehaviour : MonoBehaviour
{
    enum States
    {
        Idle,
        Attack,
        Dead
    }

    private enum AlienAttack
    {
        None,
        Tail_Sweep, // close distance
        Head_Charge, // close distance
        Tail_Shot, // far distance
        Acid_Bomb, // far distance
        Acid_Spit, // any distance
        Xeno_Spawn, // any distance
        Giant_Jump_Far, // if player is too close for 20s
        Giant_Jump_Close // if player is too far for 20s
    }

    IGameObject playerGO;
    Vector3 directorVector;

    float playerDistance;

    float life = 1500;

    float dashSpeed = 150.0f;

    States currentState = States.Idle;
    AlienAttack currentAttack = AlienAttack.None;

    bool playerEnteredRoom = false;
    bool isClose = false;

    float farRangeThreshold = 50.0f * 3;

    float distanceTimer = 0.0f;
    const float distanceCooldown = 20.0f;
    float attackTimer = 0.0f;
    float attackCooldown = 4.0f;

    // Attack variables
    // Head Charge
    private Vector3 coordinatesToDash = Vector3.zero;

    // Projectile height
    Vector3 height = new Vector3(0.0f, 30.0f, 0.0f);

    // Acid Spit
    private float delayTimer = 0.0f;
    private float delay = 0.5f;
    private int shotsCounter = 0;
    private int maxShots = 5;

    // Tail Sweep
    private float spinElapsed = 0.0f;
    private float spinTime = 2.0f;
    bool isSpinning = false;

    // Giant Jump Far/Close
    float maxHeight = 550.0f;
    float currentHeight = 0.0f;
    bool up = true;
    float jumpSpeed = 35.0f * 5;

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
        if (currentState == States.Dead)
        {
            SceneManager.LoadScene("L2R1");
            playerGO.source.Stop(IAudioSource.AudioEvent.UI_A_MENU);
            playerGO.source.Play(IAudioSource.AudioEvent.UI_CLICK);
        }

        if (attachedGameObject.transform.ComponentCheck())
        {
            if (gameManager.colliderRender) { DebugDraw(); }

            //Set the director vector and distance to the player
            directorVector = (playerGO.transform.Position - attachedGameObject.transform.Position).Normalize();
            playerDistance = Vector3.Distance(playerGO.transform.Position, attachedGameObject.transform.Position);

            UpdateFSM();
            DoStateBehaviour();
        }
    }

    void UpdateFSM()
    {
        if (life <= 0)
        {
            currentState = States.Dead;
            player.shieldKillCounter++;
            //attachedGameObject.source.PlayAudio(IAudioSource.EventIDs.E_QUEEN_DEATH);
            return;
        }

        if (playerDistance < farRangeThreshold && !isClose)
        {
            isClose = true;
            distanceTimer = 0;
            Debug.Log("Player is now CLOSE");
        }

        if (playerDistance >= farRangeThreshold && isClose)
        {
            isClose = false;
            distanceTimer = 0;
            Debug.Log("Player is now FAR");
        }

        if (playerEnteredRoom)
        {
            distanceTimer += Time.deltaTime;
            attackTimer += Time.deltaTime;
        }

        if (playerEnteredRoom && currentAttack == AlienAttack.None)
        {
            if (attackTimer >= attackCooldown)
            {
                currentState = States.Attack;
            }
        }
    }

    void DoStateBehaviour()
    {
        switch (currentState)
        {
            case States.Idle:
                if (!playerEnteredRoom)
                {
                    // check here if the player has entered the room
                    // maybe access a level manager class o something
                    playerEnteredRoom = true; // this for the moment just for debugging
                }
                else
                {
                    attachedGameObject.transform.LookAt2D(playerGO.transform.Position);
                }
                return;
            case States.Attack:
                //player.isFighting = true;
                ChooseAttack();

                switch (currentAttack)
                {
                    case AlienAttack.Tail_Sweep:
                        TailSweep();
                        break;
                    case AlienAttack.Acid_Bomb:
                        AcidBomb();
                        break;
                    case AlienAttack.Acid_Spit:
                        AcidSpit();
                        break;
                    case AlienAttack.Head_Charge:
                        HeadCharge();
                        break;
                    case AlienAttack.Tail_Shot:
                        TailShot();
                        break;
                    case AlienAttack.Xeno_Spawn:
                        XenoSpawn();
                        break;
                    case AlienAttack.Giant_Jump_Far:
                        GiantJump(true);
                        break;
                    case AlienAttack.Giant_Jump_Close:
                        GiantJump(false);
                        break;
                    default:
                        break;
                }
                break;
            case States.Dead:
                attachedGameObject.transform.Rotate(Vector3.right * 1100.0f);
                break;
            default:
                break;
        }
    }

    public void ReduceLife(float damage) //temporary function for the hardcoding of collisions
    {
        life -= player.totalDamage;
        if (life < 0) life = 0;
    }

    public void ReduceLifeExplosion()
    {
        life -= player.GrenadeLauncher.damage;
        if (life < 0) life = 0;
    }

    // Alien Queen attacks
    private void ChooseAttack()
    {
        AlienAttack[] closeDistanceAttacks = { AlienAttack.Tail_Sweep, AlienAttack.Head_Charge, AlienAttack.Acid_Spit, AlienAttack.Xeno_Spawn };
        AlienAttack[] farDistanceAttacks = { AlienAttack.Tail_Shot, AlienAttack.Acid_Bomb, AlienAttack.Acid_Spit, AlienAttack.Xeno_Spawn };

        if (isClose && distanceTimer >= distanceCooldown)
        {
            distanceTimer = 0;
            currentAttack = AlienAttack.Giant_Jump_Far;
        }
        else if (!isClose && distanceTimer >= distanceCooldown)
        {
            distanceTimer = 0;
            currentAttack = AlienAttack.Giant_Jump_Close;
        }

        if (currentAttack == AlienAttack.None)
        {
            Random rand = new Random();
            if (isClose)
            {
                currentAttack = closeDistanceAttacks[rand.Next(closeDistanceAttacks.Length)];
            }
            else
            {
                currentAttack = farDistanceAttacks[rand.Next(closeDistanceAttacks.Length)];
            }
        }

        Debug.Log("Alien current attack: " + currentAttack);
    }

    private void TailSweep()
    {
        float damage = 15;
        if (!isSpinning) isSpinning = true;

        spinElapsed += Time.deltaTime;
        attachedGameObject.transform.Rotate(Vector3.up * -180 * Time.deltaTime);

        if (spinElapsed >= spinTime)
        {
            spinElapsed = 0.0f;
            isSpinning = false;
            ResetState();
        }
    }

    private bool MoveTo(Vector3 targetPosition)
    {

        if (Vector3.Distance(attachedGameObject.transform.Position, targetPosition) < 0.5f) return true;

        Vector3 dirVector = (targetPosition - attachedGameObject.transform.Position).Normalize();
        attachedGameObject.transform.Translate(dirVector * dashSpeed * Time.deltaTime);

        return false;
    }

    private void HeadCharge()
    {
        Debug.Log("HeadCharge() init");
        if (coordinatesToDash == Vector3.zero)
        {
            coordinatesToDash = playerGO.transform.Position;
        }
        if (MoveTo(coordinatesToDash))
        {
            ResetState();
            coordinatesToDash = Vector3.zero;
        }
        Debug.Log("HeadCharge() end");
    }

    private void TailShot()
    {
        //InternalCalls.InstantiateBullet(attachedGameObject.transform.position + attachedGameObject.transform.forward * (attachedGameObject.GetComponent<ICollider2D>().radius + 12.5f) + height,
        //                                    attachedGameObject.transform.rotation);
        //attachedGameObject.source.PlayAudio(IAudioSource.EventIDs.); // there is no audio for tail shot
        ResetState();
    }

    private void AcidSpit()
    {
        Debug.Log("AcidSpit() init");

        float damage = 10.0f;

        delayTimer += Time.deltaTime;
        attachedGameObject.transform.Rotate(Vector3.up * 20 * Time.deltaTime);

        if (delayTimer >= delay)
        {
            //InternalCalls.InstantiateBullet(attachedGameObject.transform.position + attachedGameObject.transform.forward * (attachedGameObject.GetComponent<ICollider2D>().radius + 12.5f) + height,
            //                                attachedGameObject.transform.rotation);
            attachedGameObject.source.Play(IAudioSource.AudioEvent.E_Q_SPIT);
            delayTimer = 0.0f;
            shotsCounter++;
            Debug.Log("Alien Queen has shot " + shotsCounter + " times");
        }

        if (shotsCounter >= maxShots)
        {
            attackTimer = 0.0f;
            currentAttack = AlienAttack.None;
            currentState = States.Idle;
            shotsCounter = 0;
        }

        Debug.Log("AcidSpit() end");
    }

    private void XenoSpawn()
    {

        Vector3 scale = new Vector3(1, 1, 1);

        //InternalCalls.InstantiateXenomorph(attachedGameObject.transform.position + attachedGameObject.transform.forward * (attachedGameObject.GetComponent<ICollider2D>().radius + 12.5f),
        //                                   attachedGameObject.transform.rotation,
        //                                   scale);
        InternalCalls.CreatePrefab("SK_Facehugger",
                                   attachedGameObject.transform.Position + attachedGameObject.transform.Forward * (attachedGameObject.GetComponent<ICollider2D>().radius + 12.5f),
                                   attachedGameObject.transform.Rotation);
        attachedGameObject.source.Play(IAudioSource.AudioEvent.E_X_ADULT_SPAWN);
        ResetState();
    }

    private void AcidBomb()
    {
        //InternalCalls.InstantiateBullet(attachedGameObject.transform.position + attachedGameObject.transform.forward * (attachedGameObject.GetComponent<ICollider2D>().radius + 12.5f) + height,
        //                                    attachedGameObject.transform.rotation);
        attachedGameObject.source.Play(IAudioSource.AudioEvent.E_Q_BOMB);
        ResetState();
    }

    private void GiantJump(bool isJumpingForward)
    {
        float directionMultiplier = isJumpingForward ? -1.0f : 1.0f;
        attachedGameObject.transform.Translate(attachedGameObject.transform.Forward * directionMultiplier * jumpSpeed * Time.deltaTime);

        if (up)
        {
            if (maxHeight >= currentHeight)
            {
                attachedGameObject.transform.Translate(Vector3.up * currentHeight * Time.deltaTime);
                attachedGameObject.source.Play(IAudioSource.AudioEvent.E_Q_JUMP);
                currentHeight += 10.0f;
            }
            else
            {
                up = false;
            }
        }
        else
        {
            if (currentHeight <= 0)
            {
                currentHeight = 0.0f;
            }
            else
            {
                attachedGameObject.transform.Translate(Vector3.up * -currentHeight * Time.deltaTime);
                attachedGameObject.source.Play(IAudioSource.AudioEvent.E_Q_JUMP);
                currentHeight -= 10.0f;
            }
        }

        if (currentHeight <= 0.0f)
        {
            ResetState();

            attachedGameObject.transform.Position = new Vector3(attachedGameObject.transform.Position.x,
                                                                0.0f,
                                                                attachedGameObject.transform.Position.z);
            currentHeight = 0.0f;
            up = true;
        }
    }

    private void ResetState()
    {
        attackTimer = 0.0f;
        currentAttack = AlienAttack.None;
        currentState = States.Idle;
    }

    private void DebugDraw()
    {
        Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 4, farRangeThreshold, new Vector3(1.0f, 0.0f, 1.0f)); //Purpul jiji
    }
}