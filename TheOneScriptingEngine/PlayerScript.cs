using System;
using System.Collections.Generic;

public class PlayerScript : MonoBehaviour
{
    float speed = 40.0f;
    bool lastFrameToMove = false;
    ItemManager itemManager;
    IGameObject iManagerGO;

    public bool isFighting = false;

    // Bckg music
    public AudioManager.EventIDs currentID = 0;
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
        iManagerGO = IGameObject.Find("Manager");
        itemManager = iManagerGO.GetComponent<ItemManager>();
        itemManager.AddItem(1, 1);
    }

    public override void Update()
    {
        if (isDead) return;
        if (onPause) return;

        bool toMove = false;
        Vector3 movement = Vector3.zero;

        // Background music
        if (!isFighting)
        {
            if (currentID != AudioManager.EventIDs.A_AMBIENT_1)
            {
                attachedGameObject.source.PlayAudio(AudioManager.EventIDs.A_AMBIENT_1);
                attachedGameObject.source.StopAudio(AudioManager.EventIDs.A_COMBAT_1);
                currentID = AudioManager.EventIDs.A_AMBIENT_1;
            }
        }
        else
        {
            if (currentID != AudioManager.EventIDs.A_COMBAT_1)
            {
                attachedGameObject.source.PlayAudio(AudioManager.EventIDs.A_COMBAT_1);
                attachedGameObject.source.StopAudio(AudioManager.EventIDs.A_AMBIENT_1);
                currentID = AudioManager.EventIDs.A_COMBAT_1;
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

        if (Input.GetKeyboardButton(Input.KeyboardCode.K))
        {
            if (itemManager != null)
            {
                itemManager.AddItem(1, 1);
            }

        }

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
                    Vector3 height = new Vector3(0.0f, 30.0f, 0.0f);
                    if (currentTimer < attackCooldown)
                    {
                        currentTimer += Time.deltaTime;
                        if (!hasShot && currentTimer > attackCooldown / 2)
                        {
                            InternalCalls.InstantiateBullet(attachedGameObject.transform.position + attachedGameObject.transform.forward * 13.5f + height, attachedGameObject.transform.rotation);
                            attachedGameObject.source.PlayAudio(AudioManager.EventIDs.DEBUG_GUNSHOT);
                            hasShot = true;
                        }
                    }
                    else
                    {
                        currentTimer = 0.0f;
                        hasShot = false;
                    }
                    // call particleSystem.Replay()
                }
            }
        }

        if (Input.GetKeyboardButton(Input.KeyboardCode.LSHIFT)) { speed = 80.0f; }
        else { speed = 40.0f; }

        if (toMove)
        {
            attachedGameObject.transform.Translate(movement.Normalize() * speed * Time.deltaTime);
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
                    InternalCalls.InstantiateBullet(attachedGameObject.transform.position + attachedGameObject.transform.forward * 13.5f + height, attachedGameObject.transform.rotation);
                    attachedGameObject.source.PlayAudio(AudioManager.EventIDs.DEBUG_GUNSHOT);
                    hasShot = true;
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
                attachedGameObject.source.PlayAudio(AudioManager.EventIDs.P_STEP);
            }
            else
            {
                attachedGameObject.source.StopAudio(AudioManager.EventIDs.P_STEP);
            }
            lastFrameToMove = toMove;
        }

    }

    public void ReduceLife() // temporary function for the hardcoding of collisions
    {
        if (isDead) return;

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