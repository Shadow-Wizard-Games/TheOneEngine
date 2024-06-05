using System;

public class UiScriptSettingsDisplay : MonoBehaviour
{
    public ICanvas canvas;

    UiScriptSettings settingsScript;

    float cooldown = 0;
    bool onCooldown = false;

    int currentButton = 0;

    bool vsync = true;
    bool fullscreen = false;
    int globalVolume = 10;
    int sfxVolume = 10;
    int musicVolume = 10;


    public UiScriptSettingsDisplay()
    {
        canvas = new ICanvas(InternalCalls.GetGameObjectPtr());
    }

    public override void Start()
    {
        currentButton = canvas.GetSelection();

        onCooldown = true;

        vsync = true;       //PUT HERE TO GET VSYNC
        fullscreen = false; //PUT HERE TO GET FULLSCREEN
        globalVolume = 10;  //PUT HERE TO GET GLOBAL VOLUME
        sfxVolume = 10;     //PUT HERE TO GET SFX VOLUME
        musicVolume = 10;   //PUT HERE TO GET MUSIC VOLUME


        canvas.ToggleChecker(vsync, "Checker_Vsync");
        canvas.ToggleChecker(fullscreen, "Checker_Fullscreen");
        canvas.SetSliderValue(globalVolume, "Slider_MainVolume");
        canvas.SetSliderValue(sfxVolume, "Slider_SFX");
        canvas.SetSliderValue(musicVolume, "Slider_Music");

        settingsScript = IGameObject.Find("Canvas_Settings").GetComponent<UiScriptSettings>();
    }
    public override void Update()
    {
        globalVolume = canvas.GetSliderValue("Slider_MainVolume");
        sfxVolume = canvas.GetSliderValue("Slider_SFX");
        musicVolume = canvas.GetSliderValue("Slider_Music");

        float dt = Time.realDeltaTime;
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

        if (settingsScript.editing)
        {
            //Input Updates
            if (!onCooldown)
            {
                if (Input.GetKeyboardButton(Input.KeyboardCode.UP))
                {
                    if (currentButton >= 0 && currentButton <= 1)
                    {
                        direction = 4 - currentButton;
                    }
                    else if (currentButton >= 2 && currentButton <= 4)
                    {
                        direction = -1;
                    }
                    toMove = true;
                }
                else if (Input.GetKeyboardButton(Input.KeyboardCode.DOWN))
                {
                    if (currentButton >= 0 && currentButton <= 1)
                    {
                        direction = 2 - currentButton;
                    }
                    else if (currentButton >= 2 && currentButton <= 3)
                    {
                        direction = +1;
                    }
                    else if (currentButton == 4)
                    {
                        direction = 0 - currentButton;
                    }
                    toMove = true;
                }
                else if (Input.GetKeyboardButton(Input.KeyboardCode.LEFT))
                {
                    if (currentButton == 0)
                    {
                        direction = 1 - currentButton;
                    }
                    else if (currentButton == 1)
                    {
                        direction = 0 - currentButton;
                    }
                    toMove = true;

                    //editing slider
                    if (currentButton == 2)
                    {
                        globalVolume--;
                        canvas.SetSliderValue(globalVolume, "Slider_MainVolume");
                    }
                    if (currentButton == 3)
                    {
                        sfxVolume--;
                        canvas.SetSliderValue(sfxVolume, "Slider_SFX");
                    }
                    if (currentButton == 4)
                    {
                        musicVolume--;
                        canvas.SetSliderValue(musicVolume, "Slider_Music");
                    }
                }
                else if (Input.GetKeyboardButton(Input.KeyboardCode.RIGHT))
                {
                    if (currentButton == 0)
                    {
                        direction = 1 - currentButton;
                    }
                    else if (currentButton == 1)
                    {
                        direction = 0 - currentButton;
                    }
                    toMove = true;

                    //editing slider
                    if (currentButton == 2)
                    {
                        globalVolume++;
                        canvas.SetSliderValue(globalVolume, "Slider_MainVolume");
                    }
                    if (currentButton == 3)
                    {
                        sfxVolume++;
                        canvas.SetSliderValue(sfxVolume, "Slider_SFX");
                    }
                    if (currentButton == 4)
                    {
                        musicVolume++;
                        canvas.SetSliderValue(musicVolume, "Slider_Music");
                    }
                }

                //Controller
                Vector2 movementVector = Input.GetControllerJoystick(Input.ControllerJoystickCode.JOY_LEFT);

                if (movementVector.y != 0.0f)
                {
                    if (movementVector.y > 0.0f)
                    {
                        if (currentButton <= 0)
                        {
                            if (currentButton >= 0 && currentButton <= 1)
                            {
                                direction = 4 - currentButton;
                            }
                            else if (currentButton >= 2 && currentButton <= 4)
                            {
                                direction = -1;
                            }
                        }
                        else
                        {
                            direction = -1;
                        }
                        toMove = true;
                    }
                    else if (movementVector.y < 0.0f)
                    {
                        if (currentButton >= 0 && currentButton <= 1)
                        {
                            direction = 2 - currentButton;
                        }
                        else if (currentButton >= 2 && currentButton <= 3)
                        {
                            direction = +1;
                        }
                        else if (currentButton == 4)
                        {
                            direction = 0 - currentButton;
                        }
                        toMove = true;
                    }
                }
                else if (movementVector.x != 0.0f)
                {
                    if (movementVector.x > 0.0f)
                    {
                        if (currentButton == 0)
                        {
                            direction = 1 - currentButton;
                        }
                        else if (currentButton == 1)
                        {
                            direction = 0 - currentButton;
                        }
                        toMove = true;

                        //editing slider
                        if (currentButton == 2)
                        {
                            globalVolume--;
                            canvas.SetSliderValue(globalVolume, "Slider_MainVolume");
                        }
                        if (currentButton == 3)
                        {
                            sfxVolume--;
                            canvas.SetSliderValue(sfxVolume, "Slider_SFX");
                        }
                        if (currentButton == 4)
                        {
                            musicVolume--;
                            canvas.SetSliderValue(musicVolume, "Slider_Music");
                        }
                    }
                    else if (movementVector.x < 0.0f)
                    {
                        if (currentButton == 0)
                        {
                            direction = 1 - currentButton;
                        }
                        else if (currentButton == 1)
                        {
                            direction = 0 - currentButton;
                        }
                        toMove = true;

                        //editing slider
                        if (currentButton == 2)
                        {
                            globalVolume++;
                            canvas.SetSliderValue(globalVolume, "Slider_MainVolume");
                        }
                        if (currentButton == 3)
                        {
                            sfxVolume++;
                            canvas.SetSliderValue(sfxVolume, "Slider_SFX");
                        }
                        if (currentButton == 4)
                        {
                            musicVolume++;
                            canvas.SetSliderValue(musicVolume, "Slider_Music");
                        }
                    }
                }

                if (toMove)
                {
                    attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_HOVER);
                    onCooldown = true;
                    canvas.MoveSelection(direction);
                    currentButton += direction;
                }

                if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 0)
                {
                    attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                    vsync = !vsync;
                    canvas.ToggleChecker(vsync, "Checker_Vsync");
                    onCooldown = true;
                }

                if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 1)
                {
                    attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                    fullscreen = !fullscreen;
                    canvas.ToggleChecker(fullscreen, "Checker_Fullscreen");
                    onCooldown = true;
                }
            }
        }
    }
}