using System;

public class UiScriptInventory : MonoBehaviour
{
    public ICanvas canvas;

    bool hasM4 = false;

    UiManager menuManager;

    float cooldown = 0;
    bool onCooldown = false;

    float flickerCooldown = 0;
    bool flickerOnCooldown = false;
    public bool firstFrame = true;

    int currentButton = 0;

    public UiScriptInventory()
    {
        canvas = new ICanvas(InternalCalls.GetGameObjectPtr());
    }

    public override void Start()
    {
        managers.Start();

        menuManager = IGameObject.Find("UI_Manager").GetComponent<UiManager>();

        canvas.MoveSelectionButton(0 - canvas.GetSelectedButton());
        currentButton = canvas.GetSelectedButton();

        firstFrame = true;
        flickerCooldown = 0;
        flickerOnCooldown = false;

        onCooldown = true;
    }
    public override void Update()
    {
        if (managers.itemManager.CheckItemInInventory(1) && !hasM4)
        {
            // Activar la imagen de la M4
            hasM4 = true;
        }

        float dt = Time.realDeltaTime;
        bool toMove = false;
        int direction = 0;

        if (firstFrame)
        {
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

        //Input Updates
        if (!onCooldown)
        {
            if (Input.GetKeyboardButton(Input.KeyboardCode.UP))
            {
                if (currentButton == 0)
                {
                    direction = 10 - currentButton;
                }
                else if (currentButton >= 1 && currentButton <= 3)
                {
                    direction = -1;
                }
                else if (currentButton >= 4 && currentButton <= 6)
                {
                    direction = 2 - currentButton;
                }
                else if (currentButton >= 7 && currentButton <= 9)
                {
                    direction = 20 - currentButton;
                }
                else if (currentButton >= 10 && currentButton <= 16)
                {
                    direction = -7;
                }
                else if (currentButton >= 17 && currentButton <= 18)
                {
                    direction = 19 - currentButton;
                }
                else if (currentButton == 19)
                {
                    direction = 16 - currentButton;
                }
                else if (currentButton == 20)
                {
                    direction = 19 - currentButton;
                }
                toMove = true;
            }
            else if (Input.GetKeyboardButton(Input.KeyboardCode.DOWN))
            {
                if (currentButton >= 0 && currentButton <= 2)
                {
                    direction = +1;
                }
                else if (currentButton >= 3 && currentButton <= 9)
                {
                    direction = +7;
                }
                else if (currentButton >= 10 && currentButton <= 13)
                {
                    direction = 0 - currentButton;
                }
                else if (currentButton >= 14 && currentButton <= 16)
                {
                    direction = 19 - currentButton;
                }
                else if (currentButton >= 17 && currentButton <= 19)
                {
                    direction = 20 - currentButton;
                }
                else if (currentButton == 20)
                {
                    direction = 9 - currentButton;
                }
                toMove = true;
            }
            else if (Input.GetKeyboardButton(Input.KeyboardCode.LEFT))
            {
                if (currentButton >= 0 && currentButton <= 2)
                {
                    direction = 18 - currentButton;
                }
                else if (currentButton == 3 || currentButton == 10)
                {
                    direction = +6;
                }
                else if (currentButton >= 4 && currentButton <= 16)
                {
                    direction = -1;
                }
                else if (currentButton == 17)
                {
                    direction = 1 - currentButton;
                }
                else if (currentButton >= 18 && currentButton <= 20)
                {
                    direction = 17 - currentButton;
                }
                toMove = true;
            }
            else if (Input.GetKeyboardButton(Input.KeyboardCode.RIGHT))
            {
                if (currentButton >= 0 && currentButton <= 2)
                {
                    direction = 17 - currentButton;
                }
                else if (currentButton == 9 || currentButton == 16)
                {
                    direction = -6;
                }
                else if (currentButton >= 3 && currentButton <= 17)
                {
                    direction = +1;
                }
                else if (currentButton == 18)
                {
                    direction = 1 - currentButton;
                }
                else if (currentButton >= 19 && currentButton <= 20)
                {
                    direction = 18 - currentButton;
                }
                toMove = true;
            }

            //Controller
            Vector2 movementVector = Input.GetControllerJoystick(Input.ControllerJoystickCode.JOY_LEFT);

            if (movementVector.y != 0.0f)
            {
                if (movementVector.y > 0.0f)
                {
                    if (currentButton == 0)
                    {
                        direction = 10 - currentButton;
                    }
                    else if (currentButton >= 1 && currentButton <= 3)
                    {
                        direction = -1;
                    }
                    else if (currentButton >= 4 && currentButton <= 6)
                    {
                        direction = 2 - currentButton;
                    }
                    else if (currentButton >= 7 && currentButton <= 9)
                    {
                        direction = 20 - currentButton;
                    }
                    else if (currentButton >= 10 && currentButton <= 16)
                    {
                        direction = -7;
                    }
                    else if (currentButton >= 17 && currentButton <= 18)
                    {
                        direction = 19 - currentButton;
                    }
                    else if (currentButton == 19)
                    {
                        direction = 16 - currentButton;
                    }
                    else if (currentButton == 20)
                    {
                        direction = 19 - currentButton;
                    }
                    toMove = true;
                }
                else if (movementVector.y < 0.0f)
                {
                    if (currentButton >= 0 && currentButton <= 2)
                    {
                        direction = +1;
                    }
                    else if (currentButton >= 3 && currentButton <= 9)
                    {
                        direction = +7;
                    }
                    else if (currentButton >= 10 && currentButton <= 13)
                    {
                        direction = 0 - currentButton;
                    }
                    else if (currentButton >= 14 && currentButton <= 16)
                    {
                        direction = 19 - currentButton;
                    }
                    else if (currentButton >= 17 && currentButton <= 19)
                    {
                        direction = 20 - currentButton;
                    }
                    else if (currentButton == 20)
                    {
                        direction = 9 - currentButton;
                    }
                    toMove = true;
                }
            }
            else if (movementVector.x != 0.0f)
            {
                if (movementVector.x > 0.0f)
                {
                    if (currentButton >= 0 && currentButton <= 2)
                    {
                        direction = 18 - currentButton;
                    }
                    else if (currentButton == 3 || currentButton == 10)
                    {
                        direction = +6;
                    }
                    else if (currentButton >= 4 && currentButton <= 16)
                    {
                        direction = -1;
                    }
                    else if (currentButton == 17)
                    {
                        direction = 1 - currentButton;
                    }
                    else if (currentButton >= 18 && currentButton <= 20)
                    {
                        direction = 17 - currentButton;
                    }
                    toMove = true;
                }
                else if (movementVector.x < 0.0f)
                {
                    if (currentButton >= 0 && currentButton <= 2)
                    {
                        direction = 17 - currentButton;
                    }
                    else if (currentButton == 9 || currentButton == 16)
                    {
                        direction = -6;
                    }
                    else if (currentButton >= 3 && currentButton <= 17)
                    {
                        direction = +1;
                    }
                    else if (currentButton == 18)
                    {
                        direction = 1 - currentButton;
                    }
                    else if (currentButton >= 19 && currentButton <= 20)
                    {
                        direction = 18 - currentButton;
                    }
                    toMove = true;
                }
            }

            if (toMove)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_HOVER);
                onCooldown = true;
                canvas.MoveSelectionButton(direction);
                currentButton += direction;
            }

            canvas.PrintItemUI(false, "Img_AbilityArrowLeft");
            canvas.PrintItemUI(false, "Img_AbilityArrowRight");
            canvas.PrintItemUI(false, "Img_AbilityArrowTop");
            canvas.PrintItemUI(false, "Img_AbilityArrowBot");
            switch (canvas.GetSelectedButton())
            {
                case 17:
                    canvas.PrintItemUI(true, "Img_AbilityArrowLeft");
                    break;
                case 18:
                    canvas.PrintItemUI(true, "Img_AbilityArrowRight");
                    break;
                case 19:
                    canvas.PrintItemUI(true, "Img_AbilityArrowTop");
                    break;
                case 20:
                    canvas.PrintItemUI(true, "Img_AbilityArrowBot");
                    break;
                default:
                    break;
            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 0)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                menuManager.OpenMenu(UiManager.MenuState.Stats);
            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 2)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                //FUNCTION TO USE CURRENT CONSUMIBLE ITEM
            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && (canvas.GetSelectedButton() >= 3 && canvas.GetSelectedButton() <= 16))
            {
                int itemSlot = canvas.GetSelectedButton() - 3;
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                //FUNCTION TO USE ITEM IN THE SLOT
            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && (canvas.GetSelectedButton() >= 17 && canvas.GetSelectedButton() <= 20))
            {
                int itemSlot = canvas.GetSelectedButton() - 17;
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                //FUNCTION TO SEE INFO OF CURRENT ABILITY
            }
        }
    }
}