using System;
using System.CodeDom;
using System.Collections.Generic;

public class PlayerScript : MonoBehaviour
{
    public enum CurrentWeapon
    {
        MP4,
        IMPACIENTE,
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

    IParticleSystem stepParticles;
    IParticleSystem shotParticles;

    // background music
    public bool isFighting = false;
    float timeAfterCombat = 0;
    readonly float maxTimeAfterCombat = 5.0f;


    // stats
    public float maxLife = 100.0f;
    public float life;
    public bool isDead = false;
    public float totalDamage = 0.0f;
    public float damageIncrease = 0.0f;


    // movement
    public float baseSpeed = 80.0f;
    public float speed;
    public Vector3 movementDirection;
    public Vector3 lastMovementDirection;
    public float movementMagnitude;

    // shooting
    public float damageM4 = 10.0f;
    public float currentWeoponDamage = 0.0f;
    public CurrentWeapon currentWeapon = CurrentWeapon.MP4;

    bool hasShot = false;
    float timeSinceLastShot = 0.0f;
    public float shootingCooldown = 0.15f;
    public float mp4ShootingCd = 0.1f;

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
    public string healAbilityName = "Bandage";

    public bool isRushing = false;

    public int impacienteBullets = 999;
    public int impacienteBulletCounter = 0;

    public Vector3 grenadeInitialVelocity = Vector3.zero;

    // animation states
    bool isRunning;
    public bool isShooting;

    // audio
    public IAudioSource.AudioStateID currentState = 0;
    float timeFromLastStep = 0.3f;

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
        attachedGameObject.animator.Blend = false;
        attachedGameObject.animator.TransitionTime = 0.0f;

        life = maxLife;
        speed = baseSpeed;
        currentWeoponDamage = damageM4;

        attachedGameObject.source.SetState(IAudioSource.AudioStateGroup.GAMEPLAYMODE, IAudioSource.AudioStateID.SHIP);
        attachedGameObject.source.SetSwitch(IAudioSource.AudioSwitchGroup.SURFACETYPE, IAudioSource.AudioSwitchID.SHIP);
        attachedGameObject.source.Play(IAudioSource.AudioEvent.PLAYMUSIC);

        stepParticles = iStepPSGO?.GetComponent<IParticleSystem>();
        shotParticles = iShotPSGO?.GetComponent<IParticleSystem>();
    }

    public override void Update()
    {
        timeFromLastStep += Time.deltaTime;

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

        if (gameManager.GetGameState()) { return; }

        attachedGameObject.animator.UpdateAnimation();

        // Background music
        if (!isFighting && currentState == IAudioSource.AudioStateID.COMBAT)
        {
            attachedGameObject.source.SetState(IAudioSource.AudioStateGroup.GAMEPLAYMODE, IAudioSource.AudioStateID.SHIP);
        }
        if (isFighting && currentState == IAudioSource.AudioStateID.SHIP)
        {
            attachedGameObject.source.SetState(IAudioSource.AudioStateGroup.GAMEPLAYMODE, IAudioSource.AudioStateID.COMBAT);
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

        // update total damage before shooting
        totalDamage = currentWeoponDamage + damageIncrease;

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
        if (isRunning)
        {
            if (timeFromLastStep >= 0.3f)
            {
                Step();
                timeFromLastStep = 0.0f;
            }
        }
        else
        {
            //attachedGameObject.source.Stop(IAudioSource.AudioEvent.P_STEP);
            stepParticles.End();
        }

        // current weapon switch
        switch (currentWeapon)
        {
            case CurrentWeapon.MP4:
                break;
            case CurrentWeapon.IMPACIENTE:
                break;
            case CurrentWeapon.FLAME_THROWER:
                break;
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
        stepParticles.Play();
    }

    //private void Die()
    //{
    //    attachedGameObject.transform.Rotate(Vector3.right * 90.0f);
    //    attachedGameObject.animator.Play("Death");
    //    // play sound (?)
    //}
    private bool SetMoveDirection()
    {
        bool toMove = false;
        Vector2 aimingDirection = Vector2.zero;

        #region CONTROLLER
        Vector2 leftJoystickDirection = Input.GetControllerJoystick(Input.ControllerJoystickCode.JOY_LEFT);

        if (leftJoystickDirection.x != 0.0f || leftJoystickDirection.y != 0.0f)
        {
            movementDirection += new Vector3(leftJoystickDirection.x, 0.0f, leftJoystickDirection.y);
            movementMagnitude = leftJoystickDirection.Magnitude();
            toMove = true;

            aimingDirection += leftJoystickDirection;
        }

        #endregion

        #region KEYBOARD
        if (!isDashing)
        {
            if (Input.GetKeyboardButton(Input.KeyboardCode.W))
            {
                movementDirection -= Vector3.forward;
                movementMagnitude = 1.0f;
                toMove = true;

                aimingDirection -= Vector2.up;
            }

            if (Input.GetKeyboardButton(Input.KeyboardCode.A))
            {
                movementDirection -= Vector3.right;
                movementMagnitude = 1.0f;
                toMove = true;

                aimingDirection -= Vector2.right;
            }

            if (Input.GetKeyboardButton(Input.KeyboardCode.S))
            {
                movementDirection += Vector3.forward;
                movementMagnitude = 1.0f;
                toMove = true;

                aimingDirection += Vector2.up;
            }

            if (Input.GetKeyboardButton(Input.KeyboardCode.D))
            {
                movementDirection += Vector3.right;
                movementMagnitude = 1.0f;
                toMove = true;

                aimingDirection += Vector2.right;
            }
        }
        #endregion

        movementDirection = movementDirection.Normalize();

        if (movementDirection != Vector3.zero)
        {
            lastMovementDirection = movementDirection;
            aimingDirection = aimingDirection.Normalize();
            float characterRotation = (float)Math.Atan2(aimingDirection.x, aimingDirection.y);
            attachedGameObject.transform.Rotation = new Vector3(0.0f, characterRotation, 0.0f);

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
            aimingDirection -= Vector2.up;
            hasAimed = true;
        }

        if (Input.GetKeyboardButton(Input.KeyboardCode.LEFT))
        {
            aimingDirection -= Vector2.right;
            hasAimed = true;
        }

        if (Input.GetKeyboardButton(Input.KeyboardCode.DOWN))
        {
            aimingDirection += Vector2.up;
            hasAimed = true;
        }

        if (Input.GetKeyboardButton(Input.KeyboardCode.RIGHT))
        {
            aimingDirection += Vector2.right;
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
            attachedGameObject.transform.Rotation = new Vector3(0.0f, characterRotation, 0.0f);
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
                InternalCalls.InstantiateBullet(attachedGameObject.transform.Position + attachedGameObject.transform.Forward * 13.5f + height, attachedGameObject.transform.Rotation);
                attachedGameObject.source.Play(IAudioSource.AudioEvent.W_M4_SHOOT);
                hasShot = true;
                shotParticles.Replay();

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