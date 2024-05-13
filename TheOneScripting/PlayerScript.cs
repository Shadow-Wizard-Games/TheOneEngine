using System;
using System.CodeDom;
using System.Collections.Generic;

public class PlayerScript : MonoBehaviour
{
    public enum CurrentWeapon
    {
        MP4,
        IMPACIENTE,
        GRENADE_LAUNCHER,
        FLAME_THROWER,
    }

    // managers
    ItemManager itemManager;
    IGameObject iManagerGO;

    GameManager gameManager;
    IGameObject gManagerGO;


    // particles
    IGameObject iShotPSGO;
    IGameObject iStepPSGO;


    // background music
    public bool isFighting = false;
    public bool onPause = false;
    float timeAfterCombat = 0;
    float maxTimeAfterCombat = 5.0f;


    // stats
    public float maxLife = 100.0f;
    public float life;
    public bool isDead = false;


    // movement
    public float baseSpeed = 80.0f;
    public float speed;
    public Vector3 movementDirection;
    public Vector3 lastMovementDirection;
    public float movementMagnitude;
    bool lastFrameRunned = false;


    // shooting
    public CurrentWeapon currentWeapon = CurrentWeapon.MP4;

    bool hasShot = false;
    float timeSinceLastShot = 0.0f;
    public float shootingCooldown = 0.15f;
    public float mp4ShootingCd = 0.15f;

    // Abilities
    public bool impacienteUsed;
    public bool grenadeLauncherUsed;
    public bool flamethrowerUsed;
    public bool adrenalineRushUsed;

    public bool isDashing = false;
    public string dashAbilityName = "Roll";

    public int shieldKillCounter = 0;
    public bool shieldIsActive = false;

    public bool isHealing = false;

    public bool isRushing = false;

    public int impacienteBullets = 999;
    public int impacienteBulletCounter = 0;

    public Vector3 grenadeInitialVelocity = Vector3.zero;

    // animation states
    bool isRunning;
    bool isShooting;

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
        attachedGameObject.animator.transitionTime = 0.0f;

        life = maxLife;
        speed = baseSpeed;
    }

    public override void Update()
    {
        isRunning = false;
        isShooting = false;


        //to delete just test
        if (Input.GetKeyboardButton(Input.KeyboardCode.SIX))
        {
            dashAbilityName = "Dash";
        }

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
            //if (attachedGameObject.source.currentID != IAudioSource.AudioEvent.A_AMBIENT_1)
            //{
            //    attachedGameObject.source.Play(IAudioSource.AudioEvent.A_AMBIENT_1);
            //    attachedGameObject.source.Stop(IAudioSource.AudioEvent.A_COMBAT_1);
            //    attachedGameObject.source.currentID = IAudioSource.AudioEvent.A_AMBIENT_1;
            //}
        }
        else
        {
            //if (attachedGameObject.source.currentID != IAudioSource.AudioEvent.A_COMBAT_1)
            //{
            //    attachedGameObject.source.Play(IAudioSource.AudioEvent.A_COMBAT_1);
            //    attachedGameObject.source.Stop(IAudioSource.AudioEvent.A_AMBIENT_1);
            //    attachedGameObject.source.currentID = IAudioSource.AudioEvent.A_COMBAT_1;
            //}
        }

        if (isFighting)
        {
            timeAfterCombat += Time.deltaTime;
            if (timeAfterCombat >= maxTimeAfterCombat)
            {
                timeAfterCombat = 0;
                isFighting = false;
            }
        }


        float currentSpeed = speed;
        if (gameManager.extraSpeed) { currentSpeed = 200.0f; }

        // set movement
        movementDirection = Vector3.zero;
        movementMagnitude = 0;
        isRunning = SetMoveDirection();
        if (isRunning)
        {
            attachedGameObject.transform.Translate(movementDirection * movementMagnitude * currentSpeed * Time.deltaTime);
        }

        // set shoot direction
        SetShootDirection();

        if (itemManager != null)
        {
            if (itemManager.hasInitial)
            {
                if (Input.GetKeyboardButton(Input.KeyboardCode.SPACEBAR) || Input.GetControllerButton(Input.ControllerButtonCode.R1))
                {
                    Shoot();
                }
            }
        }

        // Play steps
        if (lastFrameRunned != isRunning)
        {
            if (isRunning)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.P_STEP);
                if (iStepPSGO != null)
                {
                    iStepPSGO.GetComponent<IParticleSystem>().Play();
                }
            }
            else
            {
                attachedGameObject.source.Stop(IAudioSource.AudioEvent.P_STEP);
                if (iStepPSGO != null)
                {
                    iStepPSGO.GetComponent<IParticleSystem>().End();
                }
            }
            lastFrameRunned = isRunning;

        }

        // animations
        if (isRunning)
        {
            if (isShooting)
            {
                attachedGameObject.animator.Play("Run M4");
            }
            else
            {
                attachedGameObject.animator.Play("Run");
            }
        }
        else if (isShooting)
        {
            attachedGameObject.animator.Play("Shoot M4");
        }
        else if (isDashing)
        {
            if (dashAbilityName == "Roll")
            {
                attachedGameObject.animator.Play("Roll");
            }
            else if (dashAbilityName == "Dash")
            {

            }
        }
        else if (isRushing)
        {
            attachedGameObject.animator.Play("Adrenaline Rush Static");
        }
        else
        {
            attachedGameObject.animator.Play("Idle");
        }
    }

    private void Step()
    {
        attachedGameObject.source.Play(IAudioSource.AudioEvent.P_STEP);
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
        if (!isDashing)
        {
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
        }
        #endregion

        movementDirection = movementDirection.Normalize();

        if (movementDirection != Vector3.zero)
        {
            lastMovementDirection = movementDirection;
        }

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

    private void Shoot()
    {
        Vector3 height = new Vector3(0.0f, 30.0f, 0.0f);
        isShooting = true;
        if (timeSinceLastShot < shootingCooldown)
        {
            timeSinceLastShot += Time.deltaTime;
            if (!hasShot && timeSinceLastShot > shootingCooldown / 2)
            {
                InternalCalls.InstantiateBullet(attachedGameObject.transform.position + attachedGameObject.transform.forward * 13.5f + height, attachedGameObject.transform.rotation);
                attachedGameObject.source.Play(IAudioSource.AudioEvent.DEBUG_GUNSHOT);
                hasShot = true;
                if (iShotPSGO != null) iShotPSGO.GetComponent<IParticleSystem>().Replay();

                if (currentWeapon == CurrentWeapon.IMPACIENTE)
                    impacienteBulletCounter++;
            }
        }
        else
        {
            timeSinceLastShot = 0.0f;
            hasShot = false;
        }
    }

    public void ReduceLife() // temporary function for the hardcoding of collisions
    {
        if (isDead || gameManager.godMode || shieldIsActive || isDashing)
            return;

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