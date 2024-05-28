using System;

public class PlayerScript : MonoBehaviour
{
    public enum CurrentWeapon
    {
        NONE,
        M4,
        IMPACIENTE,
        FLAMETHROWER,
        GRENADELAUNCHER,
    }

    public enum CurrentAction
    {
        IDLE,
        RUN,
        SHOOT,
        RUNSHOOT,
        #region CONSUMABLES
        ADRENALINERUSH,
        RUNADRENALINERUSH,
        BANDAGE,
        RUNBANDAGE,
        SYRINGE,
        RUNSYRINGE,
        #endregion
        DASH,
        DEAD,
    }

    // managers
    ItemManager itemManager;
    IGameObject iManagerGO;

    GameManager gameManager;
    IGameObject gManagerGO;

    // particles
    IParticleSystem stepParticles;
    IParticleSystem shotParticles;

    // background music
    public bool isFighting = false;
    public bool onPause = false;

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

    public Item currentWeapon;
    public float currentWeaponDamage;
    public CurrentWeapon currentWeaponType;
    public CurrentAction currentAction;

    public bool hasShot = false;
    float timeSinceLastShot = 0.0f;
    public float shootingCooldown = 0.15f;

    public string dashAbilityName = "Roll";

    public bool shieldIsActive = false;

    public string healAbilityName = "Bandage";

    public Vector3 grenadeInitialVelocity = Vector3.zero;
    public Vector3 explosionPos = Vector3.zero;
    public float grenadeExplosionRadius = 50.0f;
    readonly public float grenadeDamage = 50.0f;

    public int shieldKillCounter;

    // audio
    public IAudioSource.AudioStateID currentAudioState = 0;
    float timeFromLastStep = 0.3f;

    public override void Start()
    {
        iManagerGO = IGameObject.Find("ItemManager");
        itemManager = iManagerGO?.GetComponent<ItemManager>();

        gManagerGO = IGameObject.Find("GameManager");
        gameManager = gManagerGO.GetComponent<GameManager>();

        attachedGameObject.animator.Play("Idle");
        attachedGameObject.animator.Blend = true;
        attachedGameObject.animator.TransitionTime = 0.1f;

        attachedGameObject.source.SetState(IAudioSource.AudioStateGroup.GAMEPLAYMODE, IAudioSource.AudioStateID.SHIP);
        attachedGameObject.source.SetSwitch(IAudioSource.AudioSwitchGroup.SURFACETYPE, IAudioSource.AudioSwitchID.SHIP);
        attachedGameObject.source.Play(IAudioSource.AudioEvent.PLAYMUSIC);

        stepParticles = attachedGameObject.FindInChildren("ShotPlayerPS")?.GetComponent<IParticleSystem>();
        shotParticles = attachedGameObject.FindInChildren("StepsPS")?.GetComponent<IParticleSystem>();

        currentWeaponType = CurrentWeapon.NONE;
        currentAction = CurrentAction.IDLE;
    }

    public override void Update()
    {
        if (onPause) return;

        if (itemManager.hasInitial && currentWeaponType == CurrentWeapon.NONE)
        {
            IGameObject.InstanciatePrefab("WP_CarabinaM4", attachedGameObject.transform.Position, attachedGameObject.transform.Rotation);
            currentWeaponType = CurrentWeapon.M4;
        }

        #region PLAYERSTATESWITCH
        switch (currentAction)
        {
            case CurrentAction.IDLE:
                IdleAction();
                break;
            case CurrentAction.RUN:
                RunAction();
                break;
            case CurrentAction.SHOOT:
                ShootAction();
                break;
            case CurrentAction.RUNSHOOT:
                RunAndShootAction();
                break;
            case CurrentAction.ADRENALINERUSH:

                break;
            case CurrentAction.RUNADRENALINERUSH:

                break;
            case CurrentAction.BANDAGE:

                break;
            case CurrentAction.RUNBANDAGE:

                break;
            case CurrentAction.SYRINGE:

                break;
            case CurrentAction.RUNSYRINGE:

                break;
            case CurrentAction.DASH:
                DashAction();
                break;
            case CurrentAction.DEAD:
                DeadAction();
                break;
        }
        #endregion

        UpdatePlayerState();
    }

    private void UpdatePlayerState()
    {
        attachedGameObject.animator.UpdateAnimation();

        // set movement
        movementDirection = Vector3.zero;
        movementMagnitude = 0;

        SetAimDirection();

        if (SetMoveDirection())
        {
            currentAction = CurrentAction.RUN;

            if (Input.GetKeyboardButton(Input.KeyboardCode.SPACEBAR) || Input.GetControllerButton(Input.ControllerButtonCode.R1))
            {
                currentAction = CurrentAction.RUNSHOOT;
            }
        }
        else
        {
            currentAction = CurrentAction.IDLE;
        }

        // totalDamage = currentweapondamage + damageIncrease;

        // background music
        if (!isFighting && currentAudioState == IAudioSource.AudioStateID.COMBAT)
        {
            attachedGameObject.source.SetState(IAudioSource.AudioStateGroup.GAMEPLAYMODE, IAudioSource.AudioStateID.SHIP);
        }
        if (isFighting && currentAudioState == IAudioSource.AudioStateID.SHIP)
        {
            attachedGameObject.source.SetState(IAudioSource.AudioStateGroup.GAMEPLAYMODE, IAudioSource.AudioStateID.COMBAT);
        }

        switch (currentWeaponType)
        {
            case CurrentWeapon.NONE:
                if (itemManager.hasInitial)
                    currentWeaponType = CurrentWeapon.M4;
                break;
            case CurrentWeapon.M4:
                break;
            case CurrentWeapon.IMPACIENTE:
                break;
            case CurrentWeapon.FLAMETHROWER:
                break;
        }
    }

    private void IdleAction()
    {
        switch (currentWeaponType)
        {
            case CurrentWeapon.NONE:
                attachedGameObject.animator.Play("Idle");
                break;
            case CurrentWeapon.M4:
                attachedGameObject.animator.Play("Idle M4");
                break;
            case CurrentWeapon.IMPACIENTE:
                attachedGameObject.animator.Play("Idle Impaciente");
                break;
            case CurrentWeapon.FLAMETHROWER:
                attachedGameObject.animator.Play("Idle Flamethrower");
                break;
            case CurrentWeapon.GRENADELAUNCHER:
                attachedGameObject.animator.Play("Idle Flamethrower");
                break;
        }

        stepParticles.End();
    }
    private void RunAction()
    {
        timeFromLastStep += Time.deltaTime;

        float currentSpeed = speed;
        if (gameManager.extraSpeed) { currentSpeed = 200.0f; }

        attachedGameObject.transform.Translate(movementDirection * movementMagnitude * currentSpeed * Time.deltaTime);

        if (timeFromLastStep >= 0.3f)
        {
            attachedGameObject.source.Play(IAudioSource.AudioEvent.P_STEP);
            stepParticles.Play();

            timeFromLastStep = 0.0f;
        }

        switch (currentWeaponType)
        {
            case CurrentWeapon.NONE:
                attachedGameObject.animator.Play("Run");
                break;
            case CurrentWeapon.M4:
                attachedGameObject.animator.Play("Run M4");
                break;
            case CurrentWeapon.IMPACIENTE:
                attachedGameObject.animator.Play("Run Impaciente");
                break;
            case CurrentWeapon.FLAMETHROWER:
                attachedGameObject.animator.Play("Run Flamethrower");
                break;
            case CurrentWeapon.GRENADELAUNCHER:
                attachedGameObject.animator.Play("Run Grenade Launcher");
                break;
        }
    }
    private void DashAction()
    {
        if (dashAbilityName == "Roll")
        {
            attachedGameObject.animator.Play("Roll");
        }
        else if (dashAbilityName == "Dash")
        {

        }
    }
    private void ShootAction()
    {
        attachedGameObject.animator.Play("Shoot M4");

        Shoot();

        stepParticles.End();
    }
    private void RunAndShootAction()
    {
        if (currentWeaponType == CurrentWeapon.M4)
        {
            attachedGameObject.animator.Play("Run and Shoot M4");
        }

        stepParticles.End();
    }
    private void DeadAction()
    {
        attachedGameObject.animator.Play("Death");

        stepParticles.End();
    }

    private bool SetMoveDirection()
    {
        bool toMove = false;
        Vector2 aimingDirection = Vector2.zero;

        #region CONTROLLER
        Vector2 leftJoystickDirection = Input.GetControllerJoystick(Input.ControllerJoystickCode.JOY_LEFT);

        if (leftJoystickDirection.x != 0.0f || leftJoystickDirection.y != 0.0f)
        {
            // 0.7071f = 1 / sqrt(2)
            Vector2 rotatedDirection = new Vector2(
                leftJoystickDirection.x * 0.7071f + leftJoystickDirection.y * 0.7071f,
                -leftJoystickDirection.x * 0.7071f + leftJoystickDirection.y * 0.7071f);

            movementDirection += new Vector3(rotatedDirection.x, 0.0f, rotatedDirection.y);
            movementMagnitude = leftJoystickDirection.Magnitude();
            toMove = true;

            aimingDirection += rotatedDirection;
        }

        #endregion
        #region KEYBOARD
        if (Input.GetKeyboardButton(Input.KeyboardCode.W))
        {
            movementDirection += Vector3.zero - Vector3.right - Vector3.forward;
            movementMagnitude = 1.0f;
            toMove = true;

            aimingDirection += Vector2.zero - Vector2.right - Vector2.up;
        }

        if (Input.GetKeyboardButton(Input.KeyboardCode.A))
        {
            movementDirection += Vector3.zero - Vector3.right + Vector3.forward;
            movementMagnitude = 1.0f;
            toMove = true;

            aimingDirection += Vector2.zero - Vector2.right + Vector2.up;
        }

        if (Input.GetKeyboardButton(Input.KeyboardCode.S))
        {
            movementDirection += Vector3.zero + Vector3.right + Vector3.forward;
            movementMagnitude = 1.0f;
            toMove = true;

            aimingDirection += Vector2.zero + Vector2.right + Vector2.up;
        }

        if (Input.GetKeyboardButton(Input.KeyboardCode.D))
        {
            movementDirection += Vector3.zero + Vector3.right - Vector3.forward;
            movementMagnitude = 1.0f;
            toMove = true;

            aimingDirection += Vector2.zero + Vector2.right - Vector2.up;
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

    private bool SetAimDirection()
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
        Vector2 rightJoystickVector = Input.GetControllerJoystick(Input.ControllerJoystickCode.JOY_RIGHT);

        if (rightJoystickVector.x != 0.0f || rightJoystickVector.y != 0.0f)
        {
            // 0.7071f = 1 / sqrt(2)
            Vector2 rotatedDirection = new Vector2(
                rightJoystickVector.x * 0.7071f + rightJoystickVector.y * 0.7071f,
                -rightJoystickVector.x * 0.7071f + rightJoystickVector.y * 0.7071f);

            aimingDirection += rotatedDirection;
            hasAimed = true;
        }

        #endregion

        if (hasAimed)
        {
            aimingDirection = aimingDirection.Normalize();
            float characterRotation = (float)Math.Atan2(aimingDirection.x, aimingDirection.y);
            attachedGameObject.transform.Rotation = new Vector3(0.0f, characterRotation, 0.0f);
        }

        return hasAimed;
    }

    private void Shoot()
    {
        Vector3 height = new Vector3(0.0f, 30.0f, 0.0f);

        if (timeSinceLastShot < shootingCooldown)
        {
            timeSinceLastShot += Time.deltaTime;
            if (!hasShot && timeSinceLastShot > shootingCooldown / 2)
            {
                InternalCalls.InstantiateBullet(attachedGameObject.transform.Position + attachedGameObject.transform.Forward * 13.5f + height, attachedGameObject.transform.Rotation);
                attachedGameObject.source.Play(IAudioSource.AudioEvent.W_M4_SHOOT);
                hasShot = true;
                shotParticles.Replay();
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
        if (isDead || gameManager.godMode || shieldIsActive || currentAction == CurrentAction.DASH)
            return;

        life -= 10.0f;
        Debug.Log("Player took damage! Current life is: " + life.ToString());

        if (life <= 0)
        {
            life = 0;
            isDead = true;
            attachedGameObject.transform.Rotate(Vector3.right * 90.0f);
        }
    }

    public void ReduceLifeExplosion() // temporary function for the hardcoding of collisions
    {
        if (isDead || gameManager.godMode || shieldIsActive || currentAction == CurrentAction.DASH)
            return;

        life -= grenadeDamage;
        Debug.Log("Player took explosion damage! Current life is: " + life.ToString());

        if (life <= 0)
        {
            life = 0;
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