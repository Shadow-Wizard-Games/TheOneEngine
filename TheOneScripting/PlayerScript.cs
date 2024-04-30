using System;
using System.Collections.Generic;

public class PlayerScript : MonoBehaviour
{
    public float speed = 80.0f;
    bool lastFrameToMove = false;
    ItemManager itemManager;
    IGameObject iManagerGO;

    IGameObject iShotPSGO;
    IGameObject iStepPSGO;

    GameManager gameManager;
    IGameObject gManagerGO;

    public bool isFighting = false;

    // Bckg music
    float enemyTimer = 0;
    float combatThreshold = 5.0f;

    bool hasShot = false;
    float currentTimer = 0.0f;
    float attackCooldown = .10f;

    public bool isDead = false;
    public bool onPause = false;
    float life = 100.0f;

    private Vector3 movementDirection;
    private float movementMagnitude;

    private void Step()
    {
        attachedGameObject.source.PlayAudio(IAudioSource.EventIDs.P_STEP);
        if (iStepPSGO != null)
        {
            iStepPSGO.GetComponent<IParticleSystem>().Play();
        }
    }

    private void Die()
    {
        attachedGameObject.transform.Rotate(Vector3.right * 90.0f);
        attachedGameObject.animator.Play("Death");
        // play sound (?)
    }

    private void Dash()
    {

    }

    private bool SetMoveDirection()
    {
        bool toMove = false;

        #region CONTROLLER
        Vector2 leftJoystickDirection = Input.GetControllerJoystick(Input.ControllerJoystickCode.JOY_LEFT);

        if (leftJoystickDirection.x != 0.0f || leftJoystickDirection.y != 0.0f)
        {
            movementDirection += new Vector3(-leftJoystickDirection.x, 0.0f, -leftJoystickDirection.y);
            movementMagnitude = leftJoystickDirection.Magnitude();
            toMove = true;
        }

        #endregion

        #region KEYBOARD
        if (Input.GetKeyboardButton(Input.KeyboardCode.W))
        {
            movementDirection += Vector3.zero - Vector3.right - Vector3.forward;
            movementMagnitude = 1.0f;
            toMove = true;
        }

        if (Input.GetKeyboardButton(Input.KeyboardCode.A))
        {
            movementDirection += Vector3.zero - Vector3.right + Vector3.forward;
            movementMagnitude = 1.0f;
            toMove = true;
        }

        if (Input.GetKeyboardButton(Input.KeyboardCode.S))
        {
            movementDirection += Vector3.zero + Vector3.right + Vector3.forward;
            movementMagnitude = 1.0f;
            toMove = true;
        }

        if (Input.GetKeyboardButton(Input.KeyboardCode.D))
        {
            movementDirection += Vector3.zero + Vector3.right - Vector3.forward;
            movementMagnitude = 1.0f;
            toMove = true;
        }

        #endregion

        movementDirection = movementDirection.Normalize();

        return toMove;
    }

    private void SetShootDirection()
    {
        bool hasAimed = false;
        Vector2 aimingDirection = Vector2.zero;

        #region KEYBOARD
        if (Input.GetKeyboardButton(Input.KeyboardCode.UP))
        {
            aimingDirection += Vector2.zero - Vector2.right - Vector2.up;
            hasAimed = true;
        }

        if (Input.GetKeyboardButton(Input.KeyboardCode.LEFT))
        {
            aimingDirection += Vector2.zero - Vector2.right + Vector2.up;
            hasAimed = true;
        }

        if (Input.GetKeyboardButton(Input.KeyboardCode.DOWN))
        {
            aimingDirection += Vector2.zero + Vector2.right + Vector2.up;
            hasAimed = true;
        }

        if (Input.GetKeyboardButton(Input.KeyboardCode.RIGHT))
        {
            aimingDirection += Vector2.zero + Vector2.right - Vector2.up;
            hasAimed = true;
        }

        #endregion

        #region CONTROLLER
        Vector2 lookVector = Input.GetControllerJoystick(Input.ControllerJoystickCode.JOY_RIGHT);

        if (lookVector.x != 0.0f || lookVector.y != 0.0f)
        {
            aimingDirection += lookVector;
            hasAimed = true;
        }

        #endregion

        if (hasAimed)
        {
            aimingDirection = aimingDirection.Normalize();
            float characterRotation = (float)Math.Atan2(aimingDirection.x, aimingDirection.y);
            attachedGameObject.transform.rotation = new Vector3(0.0f, characterRotation, 0.0f);
        }
    }

    public override void Start()
    {
        iManagerGO = IGameObject.Find("ItemManager");
        itemManager = iManagerGO.GetComponent<ItemManager>();

        gManagerGO = IGameObject.Find("GameManager");
        gameManager = gManagerGO.GetComponent<GameManager>();
        //itemManager.AddItem(1, 1);

        iShotPSGO = attachedGameObject.FindInChildren("ShotPlayerPS");
        iStepPSGO = attachedGameObject.FindInChildren("StepsPS");

        attachedGameObject.animator.Play("Idle");
        attachedGameObject.animator.blend = false;
        attachedGameObject.animator.time = 0.0f;
    }

    public override void Update()
    {
        if (isDead)
        {
            attachedGameObject.animator.Play("Death");
            return;
        }
      
        if (onPause) return;

        attachedGameObject.animator.UpdateAnimation();

        // Background music
        if (!isFighting)
        {
            if (attachedGameObject.source.currentID != IAudioSource.EventIDs.A_AMBIENT_1)
            {
                attachedGameObject.source.PlayAudio(IAudioSource.EventIDs.A_AMBIENT_1);
                attachedGameObject.source.StopAudio(IAudioSource.EventIDs.A_COMBAT_1);
                attachedGameObject.source.currentID = IAudioSource.EventIDs.A_AMBIENT_1;
            }
        }
        else
        {
            if (attachedGameObject.source.currentID != IAudioSource.EventIDs.A_COMBAT_1)
            {
                attachedGameObject.source.PlayAudio(IAudioSource.EventIDs.A_COMBAT_1);
                attachedGameObject.source.StopAudio(IAudioSource.EventIDs.A_AMBIENT_1);
                attachedGameObject.source.currentID = IAudioSource.EventIDs.A_COMBAT_1;
            }
        }

        if (isFighting)
        {
            enemyTimer += Time.deltaTime;
            if (enemyTimer >= combatThreshold)
            {
                enemyTimer = 0;
                isFighting = false;
            }
        }


        float currentSpeed = speed;
        if (gameManager.extraSpeed) { currentSpeed = 200.0f; }

        // set movement
        movementDirection = Vector3.zero;
        movementMagnitude = 0;
        bool toMove = SetMoveDirection();
        if (toMove)
        {
            attachedGameObject.transform.Translate(movementDirection * movementMagnitude * currentSpeed * Time.deltaTime);
            attachedGameObject.animator.Play("Run");
        }
        else
        {
            attachedGameObject.animator.Play("Idle");
        }

        // set shoot direction
        SetShootDirection();

        if (itemManager != null)
        {
            if (itemManager.hasInitial)
            {
                if (Input.GetKeyboardButton(Input.KeyboardCode.SPACEBAR))
                {
                    //attachedGameObject.animator.Play("ShootM4");
                    Vector3 height = new Vector3(0.0f, 30.0f, 0.0f);
                    if (currentTimer < attackCooldown)
                    {
                        currentTimer += Time.deltaTime;
                        if (!hasShot && currentTimer > attackCooldown / 2)
                        {
                            //InternalCalls.InstantiateBullet(attachedGameObject.transform.position + attachedGameObject.transform.forward * 13.5f + height, attachedGameObject.transform.rotation);
                            attachedGameObject.source.PlayAudio(IAudioSource.EventIDs.DEBUG_GUNSHOT);
                            hasShot = true;
                            //if (iShotPSGO != null) iShotPSGO.GetComponent<IParticleSystem>().Replay();
                        }
                    }
                    else
                    {
                        currentTimer = 0.0f;
                        hasShot = false;
                    }
                }
            }
        }

        if (Input.GetKeyboardButton(Input.KeyboardCode.LSHIFT))
        {
            //Here Dash
        }



        if (Input.GetControllerButton(Input.ControllerButtonCode.R1))
        {
            Vector3 height = new Vector3(0.0f, 30.0f, 0.0f);

            if (currentTimer < attackCooldown)
            {
                currentTimer += Time.deltaTime;
                if (!hasShot && currentTimer > attackCooldown / 2)
                {
                    //InternalCalls.InstantiateBullet(attachedGameObject.transform.position + attachedGameObject.transform.forward * 13.5f + height, attachedGameObject.transform.rotation);
                    attachedGameObject.source.PlayAudio(IAudioSource.EventIDs.DEBUG_GUNSHOT);
                    hasShot = true;
                    attachedGameObject.animator.Play("Shoot M4");
                }
            }
            else
            {
                currentTimer = 0.0f;
                hasShot = false;
            }
            // call particleSystem.Replay()
        }

        // Play steps
        if (lastFrameToMove != toMove)
        {
            if (toMove)
            {
                attachedGameObject.source.PlayAudio(IAudioSource.EventIDs.P_STEP);
                if (iStepPSGO != null)
                {
                    iStepPSGO.GetComponent<IParticleSystem>().Play();
                }
            }
            else
            {
                attachedGameObject.source.StopAudio(IAudioSource.EventIDs.P_STEP);
                if (iStepPSGO != null)
                {
                    iStepPSGO.GetComponent<IParticleSystem>().Stop();
                }
            }
            lastFrameToMove = toMove;

        }

    }

    public void ReduceLife() // temporary function for the hardcoding of collisions
    {
        if (isDead || gameManager.godMode) return;

        life -= 10.0f;
        Debug.Log("Player took damage! Current life is: " + life.ToString());

        if (life <= 0)
        {
            isDead = true;
            attachedGameObject.transform.Rotate(Vector3.right * 90.0f);
        }
    }

    public float CurrentLife()
    {
        if (isDead) return 0;

        return life;
    }
}
