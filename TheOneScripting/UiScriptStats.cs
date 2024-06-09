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

    float cooldown = 0;
    bool onCooldown = false;

    float flickerCooldown = 0;
    bool flickerOnCooldown = false;
    bool firstFrame = true;

    int currentButton = 0;

    public UiScriptStats()
    {
        canvas = new ICanvas(InternalCalls.GetGameObjectPtr());
    }

    public override void Start()
    {
        managers.Start();

        canvas.MoveSelectionButton(0 - canvas.GetSelectedButton());
        currentButton = canvas.GetSelectedButton();

        firstFrame = true;
        flickerCooldown = 0;
        flickerOnCooldown = false;

        onCooldown = true;
    }
    public override void Update()
    {
        float dt = Time.realDeltaTime;
        bool toMove = false;
        int direction = 0;

        if (firstFrame)
        {
            SetCanvases();
            canvas.CanvasFlicker(true);
            flickerOnCooldown = true;
            firstFrame = false;
        }

        if (flickerOnCooldown && flickerCooldown < 0.6f)
        {
            flickerCooldown += dt;
        }
        else
        {
            canvas.CanvasFlicker(false);
            flickerCooldown = 0.0f;
            flickerOnCooldown = false;
        }

        if (onCooldown && cooldown < 0.2f)
        {
            cooldown += dt;
        }
        else
        {
            cooldown = 0.0f;
            onCooldown = false;
        }

        canvas.SetTextString("", "Text_CurrentCurrencyAmount", managers.gameManager.currency);

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
                //up
                if (movementVector.y < 0.0f)
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
                //down
                else if (movementVector.y > 0.0f)
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
                //left
                if (movementVector.x < 0.0f)
                {
                }
                //right
                else if (movementVector.x > 0.0f)
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

            if (managers.gameManager.currency >= 800)
            {
                if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 0)
                {
                    if (managers.gameManager.GetDamageLvl() < 3)
                    {
                        onCooldown = true;
                        managers.gameManager.currency -= 800;
                        managers.gameManager.SetDamageLvl(managers.gameManager.GetDamageLvl() + 1);
                        ChangeStatLvl(StatType.DAMAGE, managers.gameManager.GetDamageLvl() + 1);

                        attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                    }
                }
            }
            if (managers.gameManager.currency >= 200)
            { 
                if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 1)
                {
                    if (managers.gameManager.GetLifeLvl() < 3)
                    {
                        onCooldown = true;
                        managers.gameManager.currency -= 200;
                        managers.gameManager.SetLifeLvl(managers.gameManager.GetLifeLvl() + 1);
                        ChangeStatLvl(StatType.HEALTH, managers.gameManager.GetLifeLvl() + 1);

                        attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                    }
                }

                if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 2)
                {
                    if (managers.gameManager.GetSpeedLvl() < 3)
                    {
                        onCooldown = true;
                        managers.gameManager.currency -= 200;
                        managers.gameManager.SetSpeedLvl(managers.gameManager.GetSpeedLvl() + 1);
                        ChangeStatLvl(StatType.SPEED, managers.gameManager.GetSpeedLvl() + 1);

                        attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                    }
                }
            }
        }
    }

    //only for canvas changes, NOT stat changes
    public void ChangeStatLvl(StatType type, int newLvl = -1)
    {

        switch (type)
        {
            case StatType.DAMAGE:
                switch (newLvl)
                {
                    case 1:
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageIconLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageLayerLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageArrowsLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_DamageIconLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_DamageLayerLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_DamageArrowsLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_DamageIconLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_DamageLayerLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_DamageArrowsLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_DamageIconLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_DamageLayerLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_DamageArrowsLvl4");
                        break;
                    case 2:
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageIconLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageLayerLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageArrowsLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageIconLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageLayerLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageArrowsLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_DamageIconLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_DamageLayerLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_DamageArrowsLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_DamageIconLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_DamageLayerLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_DamageArrowsLvl4");
                        break;
                    case 3:
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageIconLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageLayerLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageArrowsLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageIconLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageLayerLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageArrowsLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageIconLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageLayerLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_DamageArrowsLvl3");
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
                break;
            case StatType.HEALTH:
                switch (newLvl)
                {
                    case 1:
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthIconLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthLayerLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthArrowsLvl1");
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
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthIconLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthLayerLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthArrowsLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthIconLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthLayerLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthArrowsLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_HealthIconLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_HealthLayerLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_HealthArrowsLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_HealthIconLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_HealthLayerLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_HealthArrowsLvl4");
                        break;
                    case 3:
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthIconLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthLayerLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthArrowsLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthIconLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthLayerLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthArrowsLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthIconLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthLayerLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_HealthArrowsLvl3");
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
                break;
            case StatType.SPEED:
                switch (newLvl)
                {
                    case 1:
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedIconLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedLayerLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedArrowsLvl1");
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
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedIconLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedLayerLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedArrowsLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedIconLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedLayerLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedArrowsLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_SpeedIconLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_SpeedLayerLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_SpeedArrowsLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_SpeedIconLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_SpeedLayerLvl4");
                        canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_SpeedArrowsLvl4");
                        break;
                    case 3:
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedIconLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedLayerLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedArrowsLvl1");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedIconLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedLayerLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedArrowsLvl2");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedIconLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedLayerLvl3");
                        canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_SpeedArrowsLvl3");
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
                break;
            default:
                break;
        }
    }

    public void SetCanvases()
    {
        ChangeStatLvl(StatType.DAMAGE, managers.gameManager.GetDamageLvl() + 1);
        ChangeStatLvl(StatType.HEALTH, managers.gameManager.GetLifeLvl() + 1);
        ChangeStatLvl(StatType.SPEED, managers.gameManager.GetSpeedLvl() + 1);
    }
}