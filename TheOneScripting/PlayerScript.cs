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
    GameManager gameManager;

    // particles
    IParticleSystem stepParticles;
    IParticleSystem shotParticles;

    // background music
    public bool isFighting;
    public bool onPause;

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
    public CurrentWeapon currentWeaponType;
    public CurrentAction currentAction;

    // CHANGE WHEN INVENTORY OVERHAUL
    public float currentWeaponDamage;
    //

    public bool hasShot = false;
    float timeSinceLastShot = 0.0f;
    public float shootingCooldown = 0.15f;

    // abilities
    AbilityGrenadeLauncher GrenadeLauncher;
    AbilityAdrenalineRush AdrenalineRush;
    AbilityFlamethrower Flamethrower;
    AbilityImpaciente Impaciente;
    AbilityShield Shield;
    AbilityHeal Heal;
    AbilityDash Dash;

    // DEFINED IN ABILITY / ITEM SCRIPT
    public bool shieldIsActive = false;
    public string healAbilityName = "Bandage";
    public Vector3 grenadeInitialVelocity = Vector3.zero;
    public Vector3 explosionPos = Vector3.zero;
    public float grenadeExplosionRadius = 50.0f;
    readonly public float grenadeDamage = 50.0f;
    public int shieldKillCounter;
    //

    // audio
    public IAudioSource.AudioStateID currentAudioState;
    float timeFromLastStep;

    public override void Start()
    {
        itemManager = IGameObject.Find("ItemManager")?.GetComponent<ItemManager>();
        gameManager = IGameObject.Find("GameManager")?.GetComponent<GameManager>();

        stepParticles = attachedGameObject.FindInChildren("StepsPS")?.GetComponent<IParticleSystem>();
        shotParticles = attachedGameObject.FindInChildren("ShotPlayerPS")?.GetComponent<IParticleSystem>();

        IGameObject abilitiesGO = attachedGameObject.FindInChildren("Abilities");
        //GrenadeLauncher = abilitiesGO.GetComponent<AbilityGrenadeLauncher>();
        //AdrenalineRush = abilitiesGO.GetComponent<AbilityAdrenalineRush>();
        //Flamethrower = abilitiesGO.GetComponent<AbilityFlamethrower>();
        //Impaciente = abilitiesGO.GetComponent<AbilityImpaciente>();
        //Shield = abilitiesGO.GetComponent<AbilityShield>();
        Dash = abilitiesGO?.GetComponent<AbilityDash>();
        Heal = abilitiesGO?.GetComponent<AbilityHeal>();

        attachedGameObject.animator.Blend = true;
        attachedGameObject.animator.TransitionTime = 0.1f;

        attachedGameObject.source.SetState(IAudioSource.AudioStateGroup.GAMEPLAYMODE, IAudioSource.AudioStateID.SHIP);
        attachedGameObject.source.SetSwitch(IAudioSource.AudioSwitchGroup.SURFACETYPE, IAudioSource.AudioSwitchID.SHIP);
        attachedGameObject.source.Play(IAudioSource.AudioEvent.PLAYMUSIC);

        currentAudioState = 0;
        isFighting = false;
        onPause = false;

        currentWeaponType = CurrentWeapon.NONE;
        currentAction = CurrentAction.IDLE;

        timeFromLastStep = 0.3f;
        speed = 100.0f;
    }

    public override void Update()
    {
        if (onPause) return;

        // CHANGE WHEN INVENTORY OVERHAUL
        if (itemManager.hasM4 && currentWeaponType == CurrentWeapon.NONE)
        {
            IGameObject.InstanciatePrefab("WP_CarabinaM4", attachedGameObject.transform.Position, attachedGameObject.transform.Rotation);
            currentWeaponType = CurrentWeapon.M4;
        }
        //

        UpdatePlayerState();

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

            if (currentWeaponType != CurrentWeapon.NONE &&
                (Input.GetKeyboardButton(Input.KeyboardCode.SPACEBAR) || Input.GetControllerButton(Input.ControllerButtonCode.R1)))
            {
                currentAction = CurrentAction.RUNSHOOT;
            }
        }
        else
        {
            currentAction = CurrentAction.IDLE;

            if (currentWeaponType != CurrentWeapon.NONE &&
                (Input.GetKeyboardButton(Input.KeyboardCode.SPACEBAR) || Input.GetControllerButton(Input.ControllerButtonCode.R1)))
            {
                currentAction = CurrentAction.SHOOT;
            }
        }

        if (Input.GetKeyboardButton(Input.KeyboardCode.LSHIFT) || Input.GetControllerButton(Input.ControllerButtonCode.A))
        {
            if (Dash.state == AbilityDash.AbilityState.READY) currentAction = CurrentAction.DASH;
        }


        // background music
        if (!isFighting && currentAudioState == IAudioSource.AudioStateID.COMBAT)
        {
            attachedGameObject.source.SetState(IAudioSource.AudioStateGroup.GAMEPLAYMODE, IAudioSource.AudioStateID.SHIP);
        }
        if (isFighting && currentAudioState == IAudioSource.AudioStateID.SHIP)
        {
            attachedGameObject.source.SetState(IAudioSource.AudioStateGroup.GAMEPLAYMODE, IAudioSource.AudioStateID.COMBAT);
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
        if (Dash.state != Ability.AbilityState.READY) return;

        Dash.state = Ability.AbilityState.ACTIVE;

        if (Dash.abilityName == "Roll")
        {
            attachedGameObject.source.Play(IAudioSource.AudioEvent.P_ROLL);

            attachedGameObject.animator.Play("Roll");
        }
        else if (Dash.abilityName == "Dash")
        {
            attachedGameObject.source.Play(IAudioSource.AudioEvent.P_DASH);

            attachedGameObject.animator.Play("Roll");
        }
    }
    private void ShootAction()
    {
        Shoot();

        switch (currentWeaponType)
        {
            case CurrentWeapon.M4:
                attachedGameObject.animator.Play("Shoot M4");
                break;
            case CurrentWeapon.IMPACIENTE:
                attachedGameObject.animator.Play("Shoot Impaciente");
                break;
            case CurrentWeapon.FLAMETHROWER:
                attachedGameObject.animator.Play("Shoot Flamethrower");
                break;
            case CurrentWeapon.GRENADELAUNCHER:
                attachedGameObject.animator.Play("Shoot Grenade Launcher");
                break;
        }

    }
    private void RunAndShootAction()
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

        Shoot();

        switch (currentWeaponType)
        {
            case CurrentWeapon.M4:
                attachedGameObject.animator.Play("Run and Shoot M4");
                break;
            case CurrentWeapon.IMPACIENTE:
                attachedGameObject.animator.Play("Run and Shoot Impaciente");
                break;
            case CurrentWeapon.FLAMETHROWER:
                attachedGameObject.animator.Play("Run and Shoot Flamethrower");
                break;
            case CurrentWeapon.GRENADELAUNCHER:
                attachedGameObject.animator.Play("Run and Shoot Grenade Launcher");
                break;
        }
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

    // MAKE IT DEPEND ON CURRENT WEAPON
    private void Shoot()
    {
        switch (currentWeaponType)
        {
            case CurrentWeapon.M4:
                ShootM4();
                break;
            case CurrentWeapon.IMPACIENTE:
                ShootImpaciente();
                break;
            case CurrentWeapon.FLAMETHROWER:
                ShootFlamethrower();
                break;
            case CurrentWeapon.GRENADELAUNCHER:
                ShootGrenadeLauncher();
                break;
        }
    }

    private void ShootM4()
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
    private void ShootImpaciente() { }
    private void ShootFlamethrower() { }
    private void ShootGrenadeLauncher() { }

    public void ReduceLife(int damage) // temporary function for the hardcoding of collisions
    {
        if (isDead || gameManager.godMode || shieldIsActive || currentAction == CurrentAction.DASH)
            return;

        life -= damage;
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