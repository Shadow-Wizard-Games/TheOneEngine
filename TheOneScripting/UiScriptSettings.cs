using System;

public class UiScriptSettings : MonoBehaviour
{
    public ICanvas canvas;

    float cooldown = 0;
    bool onCooldown = false;

    public bool firstFrameUpdate = true;

    int currentButton = 0;

    public bool editing = false;

    public UiScriptSettings()
    {
        canvas = new ICanvas(InternalCalls.GetGameObjectPtr());
    }

    public override void Start()
    {
        IGameObject.Find("Canvas_SettingsControls").Disable();
        IGameObject.Find("Canvas_SettingsDisplay").Disable();

        canvas.MoveSelectionButton(0 - canvas.GetSelectedButton());
        currentButton = canvas.GetSelectedButton();

        onCooldown = true;
    }
    public override void Update()
    {
        if (!firstFrameUpdate)
        {
            IGameObject.Find("Canvas_SettingsControls").Disable();
            IGameObject.Find("Canvas_SettingsDisplay").Disable();

            IGameObject.Find("Canvas_SettingsControls").Enable();

            firstFrameUpdate = true;
        }


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

        if (!onCooldown && editing && Input.GetKeyboardButton(Input.KeyboardCode.ESCAPE))
        {
            IGameObject.Find("Canvas_SettingsDisplay").GetComponent<ICanvas>().SetUiItemState(ICanvas.UiState.IDLE,"Checker_Vsync");
            IGameObject.Find("Canvas_SettingsDisplay").GetComponent<ICanvas>().SetUiItemState(ICanvas.UiState.IDLE,"Checker_Fullscreen");
            IGameObject.Find("Canvas_SettingsDisplay").GetComponent<ICanvas>().SetUiItemState(ICanvas.UiState.IDLE,"Slider_MainVolume");
            IGameObject.Find("Canvas_SettingsDisplay").GetComponent<ICanvas>().SetUiItemState(ICanvas.UiState.IDLE,"Slider_SFX");
            IGameObject.Find("Canvas_SettingsDisplay").GetComponent<ICanvas>().SetUiItemState(ICanvas.UiState.IDLE,"Slider_Music");
            editing = false;
        }

        //Input Updates
        if (!editing)
        {
            if (!onCooldown)
            {
                if (Input.GetKeyboardButton(Input.KeyboardCode.UP))
                {
                    if (currentButton <= 0)
                    {
                        direction = 1 - currentButton;
                    }
                    else
                    {
                        direction = -1;
                    }
                    toMove = true;
                }
                else if (Input.GetKeyboardButton(Input.KeyboardCode.DOWN))
                {
                    if (currentButton >= 1)
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
                            direction = 1 - currentButton;
                        }
                        else
                        {
                            direction = -1;
                        }
                        toMove = true;
                    }
                    else if (movementVector.y < 0.0f)
                    {
                        if (currentButton >= 1)
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
                    attachedGameObject.source.PlayAudio(IAudioSource.EventIDs.UI_HOVER);
                    onCooldown = true;
                    canvas.MoveSelectionButton(direction);
                    currentButton += direction;
                }

                if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 0)
                {
                    attachedGameObject.source.PlayAudio(IAudioSource.EventIDs.UI_CLICK);
                    IGameObject.Find("Canvas_SettingsControls").Enable();
                    IGameObject.Find("Canvas_SettingsDisplay").Disable();
                }

                if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 1)
                {
                    attachedGameObject.source.PlayAudio(IAudioSource.EventIDs.UI_CLICK);
                    IGameObject.Find("Canvas_SettingsControls").Disable();
                    IGameObject.Find("Canvas_SettingsDisplay").Disable();
                    IGameObject.Find("Canvas_SettingsDisplay").GetComponent<ICanvas>().SetUiItemState(ICanvas.UiState.HOVERED,"Checker_Vsync");
                    IGameObject.Find("Canvas_SettingsDisplay").Enable();
                    editing = true;
                }
            }
        }
    }
}