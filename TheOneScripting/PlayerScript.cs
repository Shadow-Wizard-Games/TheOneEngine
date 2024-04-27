using System;
using System.Collections.Generic;

public class PlayerScript : MonoBehaviour
{
    public float speed = 40.0f;
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

    public Vector3 movement;
    public override void Update()
    {
        if (isDead)
        {
            attachedGameObject.animator.Play("Death");
            return;
        }
      
        if (onPause) return;

        bool toMove = false;
        movement = Vector3.zero;

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

        if (gameManager.extraSpeed) { speed = 200.0f; }
        else { speed = 80.0f; }

        if (Input.GetKeyboardButton(Input.KeyboardCode.W))
        {
            movement = movement - Vector3.forward;
            toMove = true;
        }
        if (Input.GetKeyboardButton(Input.KeyboardCode.D))
        {
            movement = movement + Vector3.right;
            toMove = true;
        }
        if (Input.GetKeyboardButton(Input.KeyboardCode.S))
        {
            movement = movement + Vector3.forward;
            toMove = true;
        }
        if (Input.GetKeyboardButton(Input.KeyboardCode.A))
        {
            movement = movement - Vector3.right;
            toMove = true;
        }

        if (Input.GetKeyboardButton(Input.KeyboardCode.UP))
        {
            attachedGameObject.transform.rotation = new Vector3(0, 3.14f, 0);
        }
        if (Input.GetKeyboardButton(Input.KeyboardCode.LEFT))
        {
            attachedGameObject.transform.rotation = new Vector3(0, 4.71f, 0);
        }
        if (Input.GetKeyboardButton(Input.KeyboardCode.RIGHT))
        {
            attachedGameObject.transform.rotation = new Vector3(0, 1.57f, 0);
        }
        if (Input.GetKeyboardButton(Input.KeyboardCode.DOWN))
        {
            attachedGameObject.transform.rotation = Vector3.zero;
        }

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

        if (toMove)
        {
            attachedGameObject.transform.Translate(movement.Normalize() * speed * Time.deltaTime);
            attachedGameObject.animator.Play("Run");
        }
        else
        {
            attachedGameObject.animator.Play("Idle");
        }

        //Controller
        Vector2 movementVector = Input.GetControllerJoystick(Input.ControllerJoystickCode.JOY_LEFT);

        if (movementVector.x != 0.0f || movementVector.y != 0.0f)
        {
            movement = new Vector3(-movementVector.x, 0.0f, -movementVector.y);

            attachedGameObject.transform.Translate(movement * speed * Time.deltaTime);

            toMove = true;
        }

        Vector2 lookVector = Input.GetControllerJoystick(Input.ControllerJoystickCode.JOY_RIGHT);

        if (lookVector.x != 0.0f || lookVector.y != 0.0f)
        {
            float characterRotation = (float)Math.Atan2(-lookVector.x, -lookVector.y);
            attachedGameObject.transform.rotation = new Vector3(0.0f, characterRotation, 0.0f);
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

    public override void OnCollision(IntPtr collidedGO)
    {
        IGameObject colGO = new IGameObject(collidedGO);
        if(colGO.tag == "Bullet")
        {
            ReduceLife();
            colGO.Destroy();
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