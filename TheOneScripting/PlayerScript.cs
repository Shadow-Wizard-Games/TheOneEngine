using System;
using System.Text.RegularExpressions;

public class PlayerScript : MonoBehaviour
{
    public enum SkillSet
    {
        NONE,
        SHOULDERLASERSET,
        M4A1SET,
    }

    public enum CurrentWeapon
    {
        NONE,
        M4,
        SHOULDERLASER,
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
        ADRENALINERUSH,
        RUNADRENALINERUSH,
        HEAL,
        RUNHEAL,
        DASH,
        DEAD,
    }

    // particles
    IParticleSystem stepParticles;
    IParticleSystem shotParticles;
    IParticleSystem shotExplosion;
    IParticleSystem bulletShell;

    // background music
    public bool isFighting;

    // stats
    public bool isDead = false;
    public uint baseDamage;
    public float totalDamage = 0.0f;
    public float damageIncrease = 1.0f;

    // movement
    public float totalSpeedModification = 0;
    public float currentSpeed;
    public Vector3 movementDirection;
    public Vector3 lastMovementDirection;
    public float movementMagnitude;

    public CurrentWeapon currentWeaponType;
    public CurrentAction currentAction;

    public Item_M4A1 ItemM4;
    public Item_ShoulderLaser ItemShoulderLaser;

    IGameObject M4GO;
    IGameObject ShoulderLaserGO;
    public IGameObject ImpacienteGO;
    public IGameObject FlamethrowerGO;
    //IGameObject GrenadeLauncherGO;
    IGameObject shotParticlesGO;
    IGameObject shotExplosionGO;

    public SkillSet currentSkillSet;
    float skillSetChangeBaseCD;
    float skillSetChangeTime;

    // CHANGE WHEN INVENTORY OVERHAUL
    public float currentWeaponDamage;

    // DEPENDS ON WEAPON ITEM TYPE
    public bool hasShot = false;
    float timeSinceLastShot = 0.0f;

    // abilities
    public AbilityGrenadeLauncher GrenadeLauncher;
    public AbilityAdrenalineRush AdrenalineRush;
    public AbilityFlamethrower Flamethrower;
    public AbilityImpaciente Impaciente;
    //AbilityShield Shield;
    public AbilityDash Dash;
    public AbilityHeal Heal;

    // DEFINED IN ABILITY / ITEM SCRIPT
    public bool shieldIsActive;
    public Vector3 grenadeInitialVelocity;
    public Vector3 explosionPos;
    public int shieldKillCounter;

    // audio
    public IAudioSource.AudioStateID currentAudioState;
    float timeFromLastStep;

    // Lights
    public ILight shotLight;

    public float waitForAnimationToFinish = 0.0f;

    public override void Start()
    {
        managers.Start();

        stepParticles = attachedGameObject.FindInChildren("StepsPS")?.GetComponent<IParticleSystem>();
        shotParticles = attachedGameObject.FindInChildren("ShotPlayerPS")?.GetComponent<IParticleSystem>();
        shotParticlesGO = attachedGameObject.FindInChildren("ShotPlayerPS");
        shotExplosion = attachedGameObject.FindInChildren("ShotExplosion")?.GetComponent<IParticleSystem>();
        shotExplosionGO = attachedGameObject.FindInChildren("ShotExplosion");
        bulletShell = attachedGameObject.FindInChildren("BulletShellDrop")?.GetComponent<IParticleSystem>();
        shotLight = attachedGameObject.FindInChildren("ShotLight")?.GetComponent<ILight>();

        M4GO = attachedGameObject.FindInChildren("WP_CarabinaM4");
        M4GO.Disable();
        ShoulderLaserGO = attachedGameObject.FindInChildren("WP_ShoulderLaser");
        ShoulderLaserGO.Disable();
        ImpacienteGO = attachedGameObject.FindInChildren("WP_OlPainless");
        ImpacienteGO.Disable();
        FlamethrowerGO = attachedGameObject.FindInChildren("WP_Flamethrower");
        FlamethrowerGO.Disable();

        IGameObject Abilities = attachedGameObject.FindInChildren("Abilities");
        GrenadeLauncher = Abilities?.GetComponent<AbilityGrenadeLauncher>();
        AdrenalineRush = Abilities?.GetComponent<AbilityAdrenalineRush>();
        Flamethrower = Abilities?.GetComponent<AbilityFlamethrower>();
        Impaciente = Abilities?.GetComponent<AbilityImpaciente>();
        //Shield = Abilities.GetComponent<AbilityShield>();
        Heal = Abilities?.GetComponent<AbilityHeal>();
        Dash = Abilities?.GetComponent<AbilityDash>();

        attachedGameObject.animator.Blend = true;
        attachedGameObject.animator.TransitionTime = 0.1f;

        attachedGameObject.source.SetState(IAudioSource.AudioStateGroup.GAMEPLAYMODE, IAudioSource.AudioStateID.SHIP);
        attachedGameObject.source.SetSwitch(IAudioSource.AudioSwitchGroup.SURFACETYPE, IAudioSource.AudioSwitchID.SHIP);
        attachedGameObject.source.Play(IAudioSource.AudioEvent.PLAYMUSIC);

        currentAudioState = 0;
        isFighting = false;

        currentWeaponType = CurrentWeapon.NONE;
        currentAction = CurrentAction.IDLE;
        currentSkillSet = SkillSet.NONE;

        ItemM4 = new Item_M4A1();
        ItemShoulderLaser = new Item_ShoulderLaser();

        timeFromLastStep = 0.3f;

        skillSetChangeBaseCD = 10.0f;
        skillSetChangeTime = 0.0f;

        // THIS ALWAYS LAST IN START
        SetInitPosInScene();
    }
    public override void Update()
    {
        if (managers.gameManager.GetGameState() != GameManager.GameStates.RUNNING)
        {
            currentAction = CurrentAction.IDLE;
        }

        // to delete just testing
        AddItemsToInventoryForTest();

        // CHANGE WHEN INVENTORY OVERHAUL
        if (managers.itemManager.CheckItemInInventory(1) && currentWeaponType == CurrentWeapon.NONE)
        {
            M4GO.Enable();
            currentSkillSet = SkillSet.M4A1SET;
            currentWeaponType = CurrentWeapon.M4;
            currentWeaponDamage = ItemM4.damage;
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
        
        // calculus of speed
        totalSpeedModification = AdrenalineRush.speedModification + Heal.speedModification + Impaciente.speedModification;
        currentSpeed = managers.gameManager.GetSpeed() + totalSpeedModification;

        // calculus of damage
        totalDamage = (currentWeaponDamage + managers.gameManager.GetDamage()) * damageIncrease;

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
                    RunShootAction();
                    break;
                case CurrentAction.ADRENALINERUSH:
                    AdrenalineRushAction();
                    break;
                case CurrentAction.RUNADRENALINERUSH:
                    RunAdrenalineRushAction();
                    break;
                case CurrentAction.HEAL:
                    HealAction();
                    break;
                case CurrentAction.RUNHEAL:
                    RunHealAction();
                    break;
                case CurrentAction.DASH:
                    DashAction();
                    break;
                case CurrentAction.DEAD:
                    DeadAction();
                    break;
            }
        
        if(waitForAnimationToFinish > 0)
        {
            waitForAnimationToFinish -= Time.deltaTime;
            if(waitForAnimationToFinish < 0) { waitForAnimationToFinish = 0; }

            Debug.Log("waiting for ability " + waitForAnimationToFinish);
        }
        #endregion
    }

    private void UpdatePlayerState()
    {
        attachedGameObject.animator.UpdateAnimation();
        UpdateWeaponAnimation();

        WeaponAbilityStates();

        // set movement
        movementDirection = Vector3.zero;
        movementMagnitude = 0;

        currentAction = CurrentAction.IDLE;

        SetAimDirection();

        shotLight.SwitchOff();

        #region IDLE / MOVING STATES
        if (SetMoveDirection()
            && Dash.state != AbilityDash.AbilityState.ACTIVE)
        {
            currentAction = CurrentAction.RUN;

            if ((Input.GetKeyboardButton(Input.KeyboardCode.SPACEBAR) || Input.GetControllerButton(Input.ControllerButtonCode.R2))
                && currentWeaponType != CurrentWeapon.NONE
                && Dash.state != AbilityDash.AbilityState.ACTIVE
                && Heal.state != AbilityHeal.AbilityState.ACTIVE)
            {
                currentAction = CurrentAction.RUNSHOOT;
                return;
            }

            // grenade 
            if ((Input.GetKeyboardButton(Input.KeyboardCode.TWO) || Input.GetControllerButton(Input.ControllerButtonCode.R1))
                && currentWeaponType == CurrentWeapon.GRENADELAUNCHER
                && Dash.state != AbilityDash.AbilityState.ACTIVE
                && Heal.state != AbilityHeal.AbilityState.ACTIVE)
            {
                currentAction = CurrentAction.RUNSHOOT;
                return;
            }

            if ((Input.GetKeyboardButton(Input.KeyboardCode.Q) || Input.GetControllerButton(Input.ControllerButtonCode.X))
                && Heal.numHeals > 0
                && Heal.numHeals > 0
                && Heal.state == AbilityHeal.AbilityState.READY
                && currentAction != CurrentAction.DASH)
            {
                currentAction = CurrentAction.RUNHEAL;
                return;
            }

            if ((Input.GetKeyboardButton(Input.KeyboardCode.ONE) || Input.GetControllerButton(Input.ControllerButtonCode.L1))
                && AdrenalineRush.state == AbilityAdrenalineRush.AbilityState.READY
                && currentAction != CurrentAction.HEAL
                && currentAction != CurrentAction.DASH)
            {
                currentAction = CurrentAction.RUNADRENALINERUSH;
                return;
            }
        }
        else if (Dash.state != AbilityDash.AbilityState.ACTIVE
            && Heal.state != AbilityHeal.AbilityState.ACTIVE)
        {

            currentAction = CurrentAction.IDLE;
            
            if ((Input.GetKeyboardButton(Input.KeyboardCode.SPACEBAR) || Input.GetControllerButton(Input.ControllerButtonCode.R2))
                && currentWeaponType != CurrentWeapon.NONE
                && Dash.state != AbilityDash.AbilityState.ACTIVE
                && Heal.state != AbilityHeal.AbilityState.ACTIVE)
            {
                currentAction = CurrentAction.SHOOT;
                return;
            }

            // grenade
            if ((Input.GetKeyboardButton(Input.KeyboardCode.TWO) || Input.GetControllerButton(Input.ControllerButtonCode.R1))
                && currentWeaponType == CurrentWeapon.GRENADELAUNCHER
                && Dash.state != AbilityDash.AbilityState.ACTIVE
                && Heal.state != AbilityHeal.AbilityState.ACTIVE)
            {
                currentAction = CurrentAction.SHOOT;
                return;
            }
        }

        if ((Input.GetKeyboardButton(Input.KeyboardCode.LSHIFT) || Input.GetControllerButton(Input.ControllerButtonCode.B))
            && Dash.state == AbilityDash.AbilityState.READY)
        {
            currentAction = CurrentAction.DASH;
            return;
        }

        if ((Input.GetKeyboardButton(Input.KeyboardCode.Q) || Input.GetControllerButton(Input.ControllerButtonCode.X))
            && Heal.numHeals > 0
            && Heal.state == AbilityHeal.AbilityState.READY
            && Dash.state != AbilityDash.AbilityState.ACTIVE
            && AdrenalineRush.state != AbilityAdrenalineRush.AbilityState.ACTIVE)
        {
            currentAction = CurrentAction.HEAL;
            return;
        }

        if ((Input.GetKeyboardButton(Input.KeyboardCode.ONE) || Input.GetControllerButton(Input.ControllerButtonCode.L1))
            && AdrenalineRush.state == AbilityAdrenalineRush.AbilityState.READY
            && Heal.state != AbilityHeal.AbilityState.ACTIVE
            && Dash.state != AbilityDash.AbilityState.ACTIVE)
        {
            currentAction = CurrentAction.ADRENALINERUSH;
            return;
        }
        #endregion

    }
    private void WeaponAbilityStates()
    {
        CurrentWeapon currentEquippedWeapon = currentWeaponType;

        #region SKILL SET CHANGE
        if (skillSetChangeTime > 0.0f)
        {
            skillSetChangeTime -= Time.deltaTime;
        }
        else if (Input.GetKeyboardButton(Input.KeyboardCode.SIX) || Input.GetControllerButton(Input.ControllerButtonCode.LEFT))
        {
            if (currentSkillSet == SkillSet.M4A1SET && managers.itemManager.CheckItemInInventory(3))
            {
                currentSkillSet = SkillSet.SHOULDERLASERSET;
                currentWeaponType = CurrentWeapon.SHOULDERLASER;
            }
            else if (currentSkillSet == SkillSet.SHOULDERLASERSET && managers.itemManager.CheckItemInInventory(1))
            {
                currentSkillSet = SkillSet.M4A1SET;
                currentWeaponType = CurrentWeapon.M4;
            }

            skillSetChangeTime = skillSetChangeBaseCD;
        }
        #endregion

        if (Input.GetControllerButton(Input.ControllerButtonCode.R1))
        {
            if (currentSkillSet == SkillSet.M4A1SET && GrenadeLauncher.state == AbilityGrenadeLauncher.AbilityState.READY && managers.itemManager.CheckItemInInventory(2))
                currentWeaponType = CurrentWeapon.GRENADELAUNCHER;

            else if (currentSkillSet == SkillSet.SHOULDERLASERSET && Flamethrower.state == AbilityFlamethrower.AbilityState.READY && managers.itemManager.CheckItemInInventory(8))
                currentWeaponType = CurrentWeapon.FLAMETHROWER;
        }

        if (Input.GetKeyboardButton(Input.KeyboardCode.TWO))
        {
            if (currentSkillSet == SkillSet.M4A1SET && GrenadeLauncher.state == AbilityGrenadeLauncher.AbilityState.READY 
                && managers.itemManager.CheckItemInInventory(2))
            {
                currentWeaponType = CurrentWeapon.GRENADELAUNCHER;
                waitForAnimationToFinish = 0.3f;
                Debug.LogWarning("bitch");
            }
        }
        if (Input.GetKeyboardButton(Input.KeyboardCode.THREE))
        {
            if (currentSkillSet == SkillSet.SHOULDERLASERSET && Flamethrower.state == AbilityFlamethrower.AbilityState.READY 
                && managers.itemManager.CheckItemInInventory(8))
                currentWeaponType = CurrentWeapon.FLAMETHROWER;
        }


        if ((Input.GetKeyboardButton(Input.KeyboardCode.FOUR) || Input.GetControllerButton(Input.ControllerButtonCode.L2))
            && managers.itemManager.CheckItemInInventory(7)
            && currentSkillSet != SkillSet.NONE
            && Impaciente.state == AbilityImpaciente.AbilityState.READY)
        {
            currentWeaponType = CurrentWeapon.IMPACIENTE;
            Impaciente.Activated();
        }

        #region ENABLE / DISABLE WEAPONS
        if (currentWeaponType != currentEquippedWeapon 
            || Impaciente.state == AbilityImpaciente.AbilityState.COOLDOWN
            || Flamethrower.state == AbilityFlamethrower.AbilityState.COOLDOWN
            )
        {
            switch (currentEquippedWeapon)
            {
                case CurrentWeapon.M4:
                    M4GO.Disable();
                    break;
                case CurrentWeapon.SHOULDERLASER:
                    ShoulderLaserGO.Disable();
                    break;
                case CurrentWeapon.IMPACIENTE:
                    ImpacienteGO.Disable();
                    //Impaciente.state = AbilityImpaciente.AbilityState.COOLDOWN;
                    Debug.Log("disabled");
                    break;
                case CurrentWeapon.FLAMETHROWER:
                    FlamethrowerGO.Disable();
                    Flamethrower.state = AbilityFlamethrower.AbilityState.COOLDOWN;
                    Debug.Log("Flamethrower on Cooldown");
                    break;
                case CurrentWeapon.GRENADELAUNCHER:
                    //GrenadeLauncherGO.Disable();
                    GrenadeLauncher.state = AbilityGrenadeLauncher.AbilityState.COOLDOWN;
                    Debug.Log("Grenade Launcher on Cooldown");
                    break;
            }
            switch (currentWeaponType)
            {
                case CurrentWeapon.M4:
                    // CHANGE DEPENDING ON GRENADE LAUNCHER
                    M4GO.Enable();
                    currentWeaponDamage = ItemM4.damage;
                    break;
                case CurrentWeapon.SHOULDERLASER:
                    ShoulderLaserGO.Enable();
                    currentWeaponDamage = ItemShoulderLaser.damage;
                    break;
                case CurrentWeapon.IMPACIENTE:
                    ImpacienteGO.Enable();
                    currentWeaponDamage = Impaciente.damage;
                    Debug.Log("Impaciente Activated");
                    break;
                case CurrentWeapon.FLAMETHROWER:
                    FlamethrowerGO.Enable();
                    currentWeaponDamage = Flamethrower.damage;
                    Debug.Log("Flamethrower Activated");
                    break;
                case CurrentWeapon.GRENADELAUNCHER:
                    // CHANGE DEPENDING ON GRENADE LAUNCHER
                    M4GO.Enable();
                    Debug.Log("Grenade Launcher Activated");
                    break;
            }
        }
        #endregion

    }

    private void IdleAction()
    {
        // CHECK ADRENALINE RUSH ANIMATION FINISH
        if (Dash.state == AbilityDash.AbilityState.ACTIVE
            || Heal.state == AbilityHeal.AbilityState.ACTIVE) return;

        switch (currentWeaponType)
        {
            case CurrentWeapon.NONE:
                attachedGameObject.animator.Play("Idle");
                break;
            case CurrentWeapon.M4:
                if(waitForAnimationToFinish <= 0)
                {
                    attachedGameObject.animator.Play("Idle M4");
                    M4GO.animator.Play("Idle");
                }
                break;
            case CurrentWeapon.SHOULDERLASER:
                attachedGameObject.animator.Play("Idle");
                ShoulderLaserGO.animator.Play("Idle");
                break;
            case CurrentWeapon.IMPACIENTE:
                attachedGameObject.animator.Play("Idle Impaciente");
                ImpacienteGO.animator.Play("Idle");
                break;
            case CurrentWeapon.FLAMETHROWER:
                attachedGameObject.animator.Play("Idle M4");
                FlamethrowerGO.animator.Play("Idle");
                break;
            case CurrentWeapon.GRENADELAUNCHER:
                if (waitForAnimationToFinish <= 0)
                {
                    attachedGameObject.animator.Play("Idle M4");
                    M4GO.animator.Play("Idle");
                }
                break;
        }

        stepParticles.End();
        bulletShell.End();
    }
    private void RunAction()
    {
        Move();

        if (Dash.state == AbilityDash.AbilityState.ACTIVE
            || Heal.state == AbilityHeal.AbilityState.ACTIVE) return;

        switch (currentWeaponType)
        {
            case CurrentWeapon.NONE:
                attachedGameObject.animator.Play("Run");
                break;
            case CurrentWeapon.M4:
                if (waitForAnimationToFinish <= 0)
                {
                    attachedGameObject.animator.Play("Run M4");
                    M4GO.animator.Play("Run");
                }
                break;
            case CurrentWeapon.SHOULDERLASER:
                attachedGameObject.animator.Play("Run");
                ShoulderLaserGO.animator.Play("Run");
                break;
            case CurrentWeapon.IMPACIENTE:
                attachedGameObject.animator.Play("Run M4");
                ImpacienteGO.animator.Play("Run");
                break;
            case CurrentWeapon.FLAMETHROWER:
                attachedGameObject.animator.Play("Run M4");
                FlamethrowerGO.animator.Play("Run");
                break;
            case CurrentWeapon.GRENADELAUNCHER:
                if (waitForAnimationToFinish <= 0)
                {
                    attachedGameObject.animator.Play("Run Grenade Launcher");
                    M4GO.animator.Play("Run");
                }
                break;
        }

        bulletShell.End();
    }
    private void DashAction()
    {
        if (Dash.state != AbilityDash.AbilityState.READY) return;

        Dash.state = AbilityDash.AbilityState.ACTIVE;

        if (Dash.abilityName == "Roll")
        {
            attachedGameObject.source.Play(IAudioSource.AudioEvent.P_ROLL);

            attachedGameObject.animator.Play("Roll");
        }
        else if (Dash.abilityName == "Dash")
        {
            attachedGameObject.source.Play(IAudioSource.AudioEvent.P_DASH);

            attachedGameObject.animator.Play("Dash");
        }

        // Cancel heal when dash
        if (Heal.state == AbilityHeal.AbilityState.ACTIVE) Heal.state = AbilityHeal.AbilityState.READY;
    }
    private void ShootAction()
    {
        Shoot();

        switch (currentWeaponType)
        {
            case CurrentWeapon.M4:
                attachedGameObject.animator.Play("Shoot M4");
                M4GO.animator.Play("Shoot");
                break;
            case CurrentWeapon.SHOULDERLASER:
                attachedGameObject.animator.Play("Shoot Shoulder Laser");
                ShoulderLaserGO.animator.Play("Shoot");
                break;
            case CurrentWeapon.IMPACIENTE:
                attachedGameObject.animator.Play("Shoot Impaciente");
                ImpacienteGO.animator.Play("Shoot");
                break;
            case CurrentWeapon.FLAMETHROWER:
                attachedGameObject.animator.Play("Shoot M4");
                FlamethrowerGO.animator.Play("Shoot");
                break;
            case CurrentWeapon.GRENADELAUNCHER:
                attachedGameObject.animator.Play("Shoot M4");
                M4GO.animator.Play("Shoot");
                break;
        }
    }
    private void RunShootAction()
    {
        Move();

        Shoot();

        switch (currentWeaponType)
        {
            case CurrentWeapon.M4:
                attachedGameObject.animator.Play("Run Shoot M4");
                M4GO.animator.Play("Run and Shoot");
                break;
            case CurrentWeapon.SHOULDERLASER:
                attachedGameObject.animator.Play("Run");
                ShoulderLaserGO.animator.Play("Run and Shoot");
                break;
            case CurrentWeapon.IMPACIENTE:
                attachedGameObject.animator.Play("Run Shoot Impaciente");
                ImpacienteGO.animator.Play("Run and Shoot");
                break;
            case CurrentWeapon.FLAMETHROWER:
                attachedGameObject.animator.Play("Run Shoot M4");
                FlamethrowerGO.animator.Play("Run and Shoot");
                break;
            case CurrentWeapon.GRENADELAUNCHER:
                attachedGameObject.animator.Play("Run Shoot M4");
                M4GO.animator.Play("Run and Shoot");
                //GrenadeLauncher.state = AbilityGrenadeLauncher.AbilityState.ACTIVE;
                break;
        }
    }
    private void DeadAction()
    {
        attachedGameObject.animator.Play("Death");

        stepParticles.End();
        bulletShell.End();
    }
    private void AdrenalineRushAction()
    {
        // Calculate heal amount
        float totalHeal = managers.gameManager.GetMaxHealth() * AdrenalineRush.healAmount;
        AdrenalineRush.healingInterval = totalHeal / AdrenalineRush.numIntervals;

        AdrenalineRush.speedModification = managers.gameManager.GetSpeed() * (AdrenalineRush.speedAmount);

        // increase damage
        damageIncrease = 1 + AdrenalineRush.damageAmount;

        AdrenalineRush.state = AbilityAdrenalineRush.AbilityState.ACTIVE;

        attachedGameObject.animator.Play("Adrenaline Rush Static");

        Debug.Log("Activated Adrenaline Rush");
    }
    private void RunAdrenalineRushAction()
    {
        Move();

        if (AdrenalineRush.state != AbilityAdrenalineRush.AbilityState.READY) return;

        // calculate heal amount
        float totalHeal = managers.gameManager.health * AdrenalineRush.healAmount;
        AdrenalineRush.healingInterval = totalHeal / AdrenalineRush.numIntervals;

        AdrenalineRush.speedModification = managers.gameManager.GetSpeed() * (AdrenalineRush.speedAmount);

        // increase damage
        damageIncrease = 1 + AdrenalineRush.damageAmount;

        AdrenalineRush.state = AbilityAdrenalineRush.AbilityState.ACTIVE;

        attachedGameObject.animator.Play("Adrenaline Rush Moving");

        Debug.Log("Activated Adrenaline Rush");
    }
    private void HealAction()
    {
        Heal.speedModification = managers.gameManager.GetSpeed() * -Heal.slowAmount;

        Heal.state = AbilityHeal.AbilityState.ACTIVE;

        Debug.Log("Ability Heal Activated");
    }
    private void RunHealAction()
    {
        Move();

        if (Heal.state != AbilityHeal.AbilityState.READY) return;

        Heal.speedModification = managers.gameManager.GetSpeed() * -Heal.slowAmount;

        Heal.state = AbilityHeal.AbilityState.ACTIVE;

        Debug.Log("Ability Heal Activated");
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

    private void Move()
    {
        timeFromLastStep += Time.deltaTime;

        float currentSpeed = this.currentSpeed;
        if (managers.gameManager.extraSpeed) { currentSpeed = 200.0f; }

        attachedGameObject.transform.Translate(movementDirection * movementMagnitude * currentSpeed * Time.deltaTime);

        if (timeFromLastStep >= 0.3f)
        {
            attachedGameObject.source.Play(IAudioSource.AudioEvent.P_STEP);
            stepParticles.Replay();

            timeFromLastStep = 0.0f;
        }
    }

    private void Shoot()
    {
        switch (currentWeaponType)
        {
            case CurrentWeapon.M4:
                ShootM4();
                break;
            case CurrentWeapon.SHOULDERLASER:
                ShootShoulderLaser();
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

        if (timeSinceLastShot < ItemM4.fireRate)
        {
            timeSinceLastShot += Time.deltaTime;
            if (!hasShot && timeSinceLastShot > ItemM4.fireRate / 2)
            {
                InternalCalls.InstantiateBullet(attachedGameObject.transform.Position + attachedGameObject.transform.Forward * 13.5f + height, attachedGameObject.transform.Rotation);
                attachedGameObject.source.Play(IAudioSource.AudioEvent.W_M4_SHOOT);
                hasShot = true;
                shotExplosionGO.transform.Position = new Vector3(0.0f, 0.3f, 0.3f);
                shotParticlesGO.transform.Position = new Vector3(0.0f, 0.29f, 0.252f);
                shotExplosion.Replay();
                shotParticles.Replay();
                bulletShell.Play();
                shotLight.SwitchOn();
            }
        }
        else
        {
            timeSinceLastShot = 0.0f;
            hasShot = false;
        }
    }
    private void ShootShoulderLaser()
    {
        Vector3 height = new Vector3(0.0f, 30.0f, 0.0f);

        // CHANGE WITH REAL STATS, M4 PLACEHOLDER

        if (timeSinceLastShot < ItemShoulderLaser.fireRate)
        {
            timeSinceLastShot += Time.deltaTime;
            if (!hasShot && timeSinceLastShot > ItemShoulderLaser.fireRate / 2)
            {
                InternalCalls.InstantiateBullet(attachedGameObject.transform.Position + attachedGameObject.transform.Forward * 13.5f + height, attachedGameObject.transform.Rotation);
                attachedGameObject.source.Play(IAudioSource.AudioEvent.W_SL_SHOOT);
                hasShot = true;
                shotExplosion.Replay();
                shotParticles.Replay();
                shotLight.SwitchOn();
            }
        }
        else
        {
            timeSinceLastShot = 0.0f;
            hasShot = false;
        }
    }
    private void ShootImpaciente()
    {
        Vector3 height = new Vector3(0.0f, 13.0f, 0.0f);

        // CHANGE WITH REAL STATS, M4 PLACEHOLDER

        if (timeSinceLastShot < Impaciente.fireRate)
        {
            timeSinceLastShot += Time.deltaTime;
            if (!hasShot && timeSinceLastShot > Impaciente.fireRate / 2)
            {
                Vector3 offset = new Vector3(-10.0f, 23.5f, -10.0f);
                InternalCalls.InstantiateBullet(attachedGameObject.transform.Position + (attachedGameObject.transform.Forward * -2.7f + attachedGameObject.transform.Right) * offset + height, attachedGameObject.transform.Rotation);
                attachedGameObject.source.Play(IAudioSource.AudioEvent.A_LI);
                hasShot = true;
                shotExplosionGO.transform.Position = new Vector3(-0.09f, 0.113f, 0.4f);
                shotParticlesGO.transform.Position = new Vector3(-0.09f, 0.113f, 0.4f);
                shotExplosion.Replay();
                shotParticles.Replay();
                bulletShell.Play();
                shotLight.SwitchOn();
            }
        }
        else
        {
            timeSinceLastShot = 0.0f;
            hasShot = false;
        }
    }
    private void ShootFlamethrower()
    {
        Vector3 height = new Vector3(0.0f, 30.0f, 0.0f);

        // CHANGE WITH REAL STATS, M4 PLACEHOLDER, LET IT BURRRRRN

        if (timeSinceLastShot < 0.15f)
        {
            timeSinceLastShot += Time.deltaTime;
            if (!hasShot && timeSinceLastShot > 0.15f / 2)
            {
                InternalCalls.InstantiateBullet(attachedGameObject.transform.Position + attachedGameObject.transform.Forward * 13.5f + height, attachedGameObject.transform.Rotation);
                //attachedGameObject.source.Play(IAudioSource.AudioEvent.W_M4_SHOOT);
                hasShot = true;
                shotExplosion.Replay();
                shotParticles.Replay();
                shotLight.SwitchOn();
            }
        }
        else
        {
            timeSinceLastShot = 0.0f;
            hasShot = false;
        }
    }
    private void ShootGrenadeLauncher()
    {
        GrenadeLauncher.explosionCenterPos = attachedGameObject.transform.Position + lastMovementDirection * GrenadeLauncher.range;

        Vector3 height = new Vector3(0.0f, 30.0f, 0.0f);

        // CHANGE TO A PREFAB OF GRENADE ?
        InternalCalls.InstantiateGrenade(attachedGameObject.transform.Position + attachedGameObject.transform.Forward * 13.5f + height, attachedGameObject.transform.Rotation);
        //

        attachedGameObject.source.Play(IAudioSource.AudioEvent.A_GL_SHOOT);

        grenadeInitialVelocity = lastMovementDirection * GrenadeLauncher.grenadeVelocity;

        currentWeaponType = CurrentWeapon.M4;

        GrenadeLauncher.state = AbilityGrenadeLauncher.AbilityState.COOLDOWN;

        Debug.Log("Ability Grenade Launcher Activated");
    }

    private void UpdateWeaponAnimation()
    {
        switch (currentWeaponType)
        {
            case CurrentWeapon.M4:
                M4GO.animator.UpdateAnimation();
                break;
            case CurrentWeapon.SHOULDERLASER:
                ShoulderLaserGO.animator.UpdateAnimation();
                break;
            case CurrentWeapon.IMPACIENTE:
                ImpacienteGO.animator.UpdateAnimation();
                break;
            case CurrentWeapon.FLAMETHROWER:
                FlamethrowerGO.animator.UpdateAnimation();
                break;
            case CurrentWeapon.GRENADELAUNCHER:
                M4GO.animator.UpdateAnimation();
                break;
        }
    }

    public void ReduceLife()
    {
        if (isDead || managers.gameManager.godMode /*|| shieldIsActive*/ || currentAction == CurrentAction.DASH)
            return;

        float damage = managers.gameManager.godMode ? 3.0f : 0.0f;
        managers.gameManager.health -= damage;

        if (managers.gameManager.health <= 0)
        {
            managers.gameManager.health = 0;
            isDead = true;
            attachedGameObject.transform.Rotate(Vector3.right * 90.0f);
        }
    }

    public void ReduceLifeExplosion()
    {
        if (isDead || managers.gameManager.godMode /*|| shieldIsActive*/ || currentAction == CurrentAction.DASH)
            return;

        managers.gameManager.health -= 50;
        //Debug.Log("Player took explosion damage! Current life is: " + gameManager.health.ToString());

        if (managers.gameManager.health <= 0)
        {
            managers.gameManager.health = 0;
            isDead = true;
            attachedGameObject.transform.Rotate(Vector3.right * 90.0f);
        }
    }

    public float CurrentLife()
    {
        if (isDead) return 0;

        return managers.gameManager.health;
    }

    private void SetInitPosInScene()
    {
        if(managers.gameManager == null || managers.gameManager.lastLevel == " ")
            return;

        ITransform spawnTransform = IGameObject.Find("Spawn_" + managers.gameManager.lastLevel)?.GetComponent<ITransform>();

        if (spawnTransform != null)
        {
            Vector3 spawnPos = spawnTransform.Position;
            attachedGameObject.transform.Position = spawnPos;
        }        
    }

    private void AddItemsToInventoryForTest()
    {
        // m4
        if (Input.GetKeyboardButton(Input.KeyboardCode.F4) && !managers.itemManager.CheckItemInInventory(1))
            managers.itemManager.AddItem(1, 1);
        // grenade launcher
        if (Input.GetKeyboardButton(Input.KeyboardCode.F5) && !managers.itemManager.CheckItemInInventory(2))
            managers.itemManager.AddItem(2, 1);
        // shoulder laser
        if (Input.GetKeyboardButton(Input.KeyboardCode.F6) && !managers.itemManager.CheckItemInInventory(3))
            managers.itemManager.AddItem(3, 1);
        // impaciente
        if (Input.GetKeyboardButton(Input.KeyboardCode.F7) && !managers.itemManager.CheckItemInInventory(7))
            managers.itemManager.AddItem(7, 1);
        // flamethrower
        if (Input.GetKeyboardButton(Input.KeyboardCode.F8) && !managers.itemManager.CheckItemInInventory(8))
            managers.itemManager.AddItem(8, 1);
    }
}