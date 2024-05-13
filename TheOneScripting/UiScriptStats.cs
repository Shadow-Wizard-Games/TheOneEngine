using System;

public class UiScriptStats : MonoBehaviour
{
    public enum StatType
    {
        DAMAGE,
        HEALTH,
        SPEED
    }

    public ICanvas canvas;
    ItemManager itemManager;
    IGameObject iManagerGO;
    UiManager menuManager;

    int currency = 0;

    float cooldown = 0;
    bool onCooldown = false;

    int currentButton = 0;

    //it goes from lvl 1 to 4
    int playerDamageLvl = 1;

    //it goes from lvl 1 to 4
    int playerHealthLvl = 1;

    //it goes from lvl 1 to 4
    int playerSpeedLvl = 1;

    public UiScriptStats()
    {
        canvas = new ICanvas(InternalCalls.GetGameObjectPtr());
    }

    public override void Start()
    {
        iManagerGO = IGameObject.Find("ItemManager");
        itemManager = iManagerGO.GetComponent<ItemManager>();
        menuManager = IGameObject.Find("UI_Manager").GetComponent<UiManager>();

        canvas.MoveSelectionButton(0 - canvas.GetSelectedButton());
        currentButton = canvas.GetSelectedButton();

        currency = 200;//PUT HERE TO GET REAL CURRENCY OF PLAYER
        playerDamageLvl = 1;//PUT HERE TO GET REAL DAMAGE LVL OF PLAYER
        playerHealthLvl = 1;//PUT HERE TO GET REAL HEALTH LVL OF PLAYER
        playerSpeedLvl = 1;//PUT HERE TO GET REAL SPEED LVL OF PLAYER

        ChangeStatLvl(StatType.DAMAGE, false, playerDamageLvl);
        ChangeStatLvl(StatType.HEALTH, false, playerHealthLvl);
        ChangeStatLvl(StatType.SPEED,  false, playerSpeedLvl);


        onCooldown = true;
    }
    public override void Update()
    {
        float dt = InternalCalls.GetAppDeltaTime();
        bool toMove = false;
        int direction = 0;

        if (onCooldown && cooldown < 0.2f)
        {
            cooldown += dt;
        }
        else
        {
            cooldown = 0.0f;
            onCooldown = false;
        }

        canvas.SetTextString(currency.ToString(), "Text_CurrentCurrencyAmount");

        //Input Updates
        if (!onCooldown)
        {
            if (Input.GetKeyboardButton(Input.KeyboardCode.UP))
            {
                if (currentButton <= 0)
                {
                    direction = 2 - currentButton;
                }
                else
                {
                    direction = -1;
                }
                toMove = true;
            }
            else if (Input.GetKeyboardButton(Input.KeyboardCode.DOWN))
            {
                if (currentButton >= 2)
                {
                    direction = 0 - currentButton;
                }
                else
                {
                    direction = +1;
                }
                toMove = true;
            }
            else if (Input.GetKeyboardButton(Input.KeyboardCode.LEFT))
            {
            }
            else if (Input.GetKeyboardButton(Input.KeyboardCode.RIGHT))
            {
            }

            //Controller
            Vector2 movementVector = Input.GetControllerJoystick(Input.ControllerJoystickCode.JOY_LEFT);

            if (movementVector.y != 0.0f)
            {
                if (movementVector.y > 0.0f)
                {
                    if (currentButton <= 0)
                    {
                        direction = 2 - currentButton;
                    }
                    else
                    {
                        direction = -1;
                    }
                    toMove = true;
                }
                else if (movementVector.y < 0.0f)
                {
                    if (currentButton >= 2)
                    {
                        direction = 0 - currentButton;
                    }
                    else
                    {
                        direction = +1;
                    }
                    toMove = true;
                }
            }
            else if (movementVector.x != 0.0f)
            {
                if (movementVector.x > 0.0f)
                {
                }
                else if (movementVector.x < 0.0f)
                {
                }
            }

            if (toMove)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_HOVER);
                onCooldown = true;
                canvas.MoveSelectionButton(direction);
                currentButton += direction;
            }

            if (currency >= 100)
            {
                if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 0)
                {
                    attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                    //this function only for canvas update, not stat !!!
                    ChangeStatLvl(StatType.DAMAGE);
                    onCooldown = true;
                    currency = currency - 100;
                    //PUT HERE FUNCTION TO BUY DAMAGE STAT UPGRADE
                    //PLAYER.CURRENCY = THIS.CURRENCY
                    //PLAYER.DAMAGELVL = THIS.PLAYERDAMAGELVL
                    //here i assume player has itself something like bullet.damage = player.damage + player.damagelvl * 10
                }

                if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 1)
                {
                    attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                    //this function only for canvas update, not stat !!!
                    ChangeStatLvl(StatType.HEALTH);
                    onCooldown = true;
                    currency = currency - 100;
                    //PUT HERE FUNCTION TO BUY HEALTH STAT UPGRADE
                    //PLAYER.CURRENCY = THIS.CURRENCY
                    //PLAYER.HEALTHLVL = THIS.PLAYERHEALTHLVL
                    //here i assume player has itself something like bullet.health = player.health + player.healthlvl * 20
                }

                if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 2)
                {
                    attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                    //this function only for canvas update, not stat !!!
                    ChangeStatLvl(StatType.SPEED);
                    onCooldown = true;
                    currency = currency - 100;
                    //PUT HERE FUNCTION TO BUY SPEED STAT UPGRADE
                    //PLAYER.CURRENCY = THIS.CURRENCY
                    //PLAYER.SPEEDLVL = THIS.PLAYERSPEEDLVL
                    //here i assume player has itself something like bullet.speed = player.speed + player.speedlvl * 10
                }
            }
        }
    }

    //only for canvas changes, NOT stat changes
    public void ChangeStatLvl(StatType type, bool trueifjustlvlup = true, int newLvl = -1)
    {

        switch (type)
        {
            case StatType.DAMAGE:
                if (trueifjustlvlup && this.playerDamageLvl <= 3) newLvl = this.playerDamageLvl + 1;
                switch (newLvl)
                {
                    case 1:
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,   "Button_DamageIconLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,   "Button_DamageLayerLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,   "Button_DamageArrowsLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED,"Button_DamageIconLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED,"Button_DamageLayerLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED,"Button_DamageArrowsLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED,"Button_DamageIconLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED,"Button_DamageLayerLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED,"Button_DamageArrowsLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED,"Button_DamageIconLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED,"Button_DamageLayerLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED,"Button_DamageArrowsLvl4");
                        break;
                    case 2:
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_DamageIconLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_DamageLayerLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_DamageArrowsLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_DamageIconLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_DamageLayerLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_DamageArrowsLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_DamageIconLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_DamageLayerLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_DamageArrowsLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_DamageIconLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_DamageLayerLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_DamageArrowsLvl4");
                        break;
                    case 3:
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_DamageIconLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_DamageLayerLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_DamageArrowsLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_DamageIconLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_DamageLayerLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_DamageArrowsLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_DamageIconLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_DamageLayerLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_DamageArrowsLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_DamageIconLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_DamageLayerLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_DamageArrowsLvl4");
                        break;
                    case 4:
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageIconLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageLayerLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageArrowsLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageIconLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageLayerLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageArrowsLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageIconLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageLayerLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageArrowsLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageIconLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageLayerLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageArrowsLvl4");
                        break;
                    default:
                        break;
                }
                this.playerDamageLvl = newLvl;
                break;
            case StatType.HEALTH:
                if (trueifjustlvlup && this.playerHealthLvl <= 3) newLvl = this.playerHealthLvl + 1;
                switch (newLvl)
                {
                    case 1:
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_HealthIconLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_HealthLayerLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_HealthArrowsLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_HealthIconLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_HealthLayerLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_HealthArrowsLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_HealthIconLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_HealthLayerLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_HealthArrowsLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_HealthIconLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_HealthLayerLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_HealthArrowsLvl4");
                        break;
                    case 2:
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_HealthIconLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_HealthLayerLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_HealthArrowsLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_HealthIconLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_HealthLayerLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_HealthArrowsLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_HealthIconLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_HealthLayerLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_HealthArrowsLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_HealthIconLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_HealthLayerLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_HealthArrowsLvl4");
                        break;
                    case 3:
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_HealthIconLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_HealthLayerLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_HealthArrowsLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_HealthIconLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_HealthLayerLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_HealthArrowsLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_HealthIconLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_HealthLayerLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_HealthArrowsLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_HealthIconLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_HealthLayerLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_HealthArrowsLvl4");
                        break;
                    case 4:
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthIconLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthLayerLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthArrowsLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthIconLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthLayerLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthArrowsLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthIconLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthLayerLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthArrowsLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthIconLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthLayerLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthArrowsLvl4");
                        break;
                    default:
                        break;
                }
                this.playerHealthLvl = newLvl;
                break;
            case StatType.SPEED:
                if (trueifjustlvlup && this.playerSpeedLvl <= 3) newLvl = this.playerSpeedLvl + 1;
                switch (newLvl)
                {
                    case 1:
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_SpeedIconLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_SpeedLayerLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_SpeedArrowsLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_SpeedIconLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_SpeedLayerLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_SpeedArrowsLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_SpeedIconLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_SpeedLayerLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_SpeedArrowsLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_SpeedIconLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_SpeedLayerLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_SpeedArrowsLvl4");
                        break; 
                    case 2:    
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_SpeedIconLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_SpeedLayerLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_SpeedArrowsLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_SpeedIconLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_SpeedLayerLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_SpeedArrowsLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_SpeedIconLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_SpeedLayerLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_SpeedArrowsLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_SpeedIconLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_SpeedLayerLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_SpeedArrowsLvl4");
                        break; 
                    case 3:    
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_SpeedIconLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_SpeedLayerLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_SpeedArrowsLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_SpeedIconLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_SpeedLayerLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_SpeedArrowsLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_SpeedIconLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_SpeedLayerLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE,    "Button_SpeedArrowsLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_SpeedIconLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_SpeedLayerLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_SpeedArrowsLvl4");
                        break;
                    case 4:   
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedIconLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedLayerLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedArrowsLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedIconLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedLayerLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedArrowsLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedIconLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedLayerLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedArrowsLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedIconLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedLayerLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedArrowsLvl4");
                        break;
                    default:
                        break;
                }
                this.playerSpeedLvl = newLvl;
                break;
            default:
                break;
        }
    }
}