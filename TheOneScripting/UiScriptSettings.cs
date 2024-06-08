using System;

public class UiScriptSettings : MonoBehaviour
{
    public ICanvas canvas;

    float cooldown = 0;
    bool onCooldown = false;

    IGameObject settingsControlsGO;
    IGameObject settingsDisplayGO;

    ICanvas settingsDisCanvas;

    int currentButton = 0;

    bool firstFrame = true;

    public bool editing = false;

    public UiScriptSettings()
    {
        canvas = new ICanvas(InternalCalls.GetGameObjectPtr());
    }

    public override void Start()
    {
        settingsControlsGO = IGameObject.Find("Canvas_SettingsControls");
        settingsDisplayGO = IGameObject.Find("Canvas_SettingsDisplay");

        canvas.MoveSelectionButton(0 - canvas.GetSelectedButton());
        currentButton = canvas.GetSelectedButton();

        settingsDisCanvas = settingsDisplayGO.GetComponent<ICanvas>();

        settingsControlsGO.Disable();
        settingsDisplayGO.Disable();
        onCooldown = true;

        firstFrame = true;
    }
    public override void Update()
    {
        float dt = Time.realDeltaTime;
        bool toMove = false;
        int direction = 0;

        if (firstFrame)
        {
            settingsControlsGO.Disable();
            settingsDisplayGO.Disable();
            settingsControlsGO.Enable();
            firstFrame = false;
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

        if (!onCooldown && editing && (Input.GetControllerButton(Input.ControllerButtonCode.B) || Input.GetKeyboardButton(Input.KeyboardCode.ESCAPE)))
        {
            settingsDisCanvas.SetUiItemState(ICanvas.UiState.IDLE, "Checker_Vsync");
            settingsDisCanvas.SetUiItemState(ICanvas.UiState.IDLE, "Checker_Fullscreen");
            settingsDisCanvas.SetUiItemState(ICanvas.UiState.IDLE, "Checker_PixelFX");
            settingsDisCanvas.SetUiItemState(ICanvas.UiState.IDLE, "Checker_CRTShader");
            settingsDisCanvas.SetUiItemState(ICanvas.UiState.IDLE, "Slider_MainVolume");
            settingsDisCanvas.SetUiItemState(ICanvas.UiState.IDLE, "Slider_SFX");
            settingsDisCanvas.SetUiItemState(ICanvas.UiState.IDLE, "Slider_Music");
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
                    //up
                    if (movementVector.y < 0.0f)
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
                    //down
                    else if (movementVector.y > 0.0f)
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

                if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 0)
                {
                    attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                    settingsControlsGO.Enable();
                    settingsDisplayGO.Disable();
                }

                if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 1)
                {
                    attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                    settingsControlsGO.Disable();
                    settingsDisplayGO.Disable();
                    settingsDisCanvas.SetUiItemState(ICanvas.UiState.HOVERED, "Checker_Vsync");
                    settingsDisplayGO.Enable();
                    editing = true;
                }
            }
        }
    }
}