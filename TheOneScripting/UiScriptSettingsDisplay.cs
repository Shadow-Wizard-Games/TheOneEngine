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
    bool pixelfx = true;
    bool crtshader = true;
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

        vsync = InternalCalls.GetVsync();
        fullscreen = InternalCalls.GetFullscreen();
        pixelfx = InternalCalls.GetPixelFX();
        crtshader = InternalCalls.GetCRTShader();
        globalVolume = InternalCalls.GetMasterVolume();
        sfxVolume = InternalCalls.GetSFXVolume();
        musicVolume = InternalCalls.GetMusicVolume();

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
                        direction = 6 - currentButton;
                    }
                    else if (currentButton >= 2 && currentButton <= 4)
                    {
                        direction = -2;
                    }
                    else if (currentButton >= 5 && currentButton <= 6)
                    {
                        direction = -1;
                    }
                    toMove = true;
                }
                else if (Input.GetKeyboardButton(Input.KeyboardCode.DOWN))
                {
                    if (currentButton >= 0 && currentButton <= 2)
                    {
                        direction = +2;
                    }
                    else if (currentButton >= 3 && currentButton <= 5)
                    {
                        direction = +1;
                    }
                    else if (currentButton == 6)
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
                    else if (currentButton == 2)
                    {
                        direction = 3 - currentButton;
                    }
                    else if (currentButton == 3)
                    {
                        direction = 2 - currentButton;
                    }
                    toMove = true;

                    //editing slider
                    if (currentButton == 4)
                    {
                        globalVolume--;
                        canvas.SetSliderValue(globalVolume, "Slider_MainVolume");
                        InternalCalls.SetMasterVolume(globalVolume);
                    }
                    if (currentButton == 5)
                    {
                        sfxVolume--;
                        canvas.SetSliderValue(sfxVolume, "Slider_SFX");
                        InternalCalls.SetSFXVolume(sfxVolume);
                    }
                    if (currentButton == 6)
                    {
                        musicVolume--;
                        canvas.SetSliderValue(musicVolume, "Slider_Music");
                        InternalCalls.SetMusicVolume(musicVolume);
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
                    else if (currentButton == 2)
                    {
                        direction = 3 - currentButton;
                    }
                    else if (currentButton == 3)
                    {
                        direction = 2 - currentButton;
                    }
                    toMove = true;

                    //editing slider
                    if (currentButton == 4)
                    {
                        globalVolume++;
                        canvas.SetSliderValue(globalVolume, "Slider_MainVolume");
                        InternalCalls.SetMasterVolume(globalVolume);
                    }
                    if (currentButton == 5)
                    {
                        sfxVolume++;
                        canvas.SetSliderValue(sfxVolume, "Slider_SFX");
                        InternalCalls.SetSFXVolume(sfxVolume);
                    }
                    if (currentButton == 6)
                    {
                        musicVolume++;
                        canvas.SetSliderValue(musicVolume, "Slider_Music");
                        InternalCalls.SetMusicVolume(musicVolume);
                    }
                }

                //Controller
                Vector2 movementVector = Input.GetControllerJoystick(Input.ControllerJoystickCode.JOY_LEFT);

                if (movementVector.y != 0.0f)
                {
                    if (movementVector.y > 0.0f)
                    {
                        if (currentButton >= 0 && currentButton <= 1)
                        {
                            direction = 6 - currentButton;
                        }
                        else if (currentButton >= 2 && currentButton <= 4)
                        {
                            direction = -2;
                        }
                        else if (currentButton >= 5 && currentButton <= 6)
                        {
                            direction = -1;
                        }
                        toMove = true;
                    }
                    else if (movementVector.y < 0.0f)
                    {
                        if (currentButton >= 0 && currentButton <= 2)
                        {
                            direction = +2;
                        }
                        else if (currentButton >= 3 && currentButton <= 5)
                        {
                            direction = +1;
                        }
                        else if (currentButton == 6)
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
                        else if (currentButton == 2)
                        {
                            direction = 3 - currentButton;
                        }
                        else if (currentButton == 3)
                        {
                            direction = 2 - currentButton;
                        }
                        toMove = true;

                        //editing slider
                        if (currentButton == 4)
                        {
                            globalVolume--;
                            canvas.SetSliderValue(globalVolume, "Slider_MainVolume");
                            InternalCalls.SetMasterVolume(globalVolume);
                        }
                        if (currentButton == 5)
                        {
                            sfxVolume--;
                            canvas.SetSliderValue(sfxVolume, "Slider_SFX");
                            InternalCalls.SetSFXVolume(sfxVolume);
                        }
                        if (currentButton == 6)
                        {
                            musicVolume--;
                            canvas.SetSliderValue(musicVolume, "Slider_Music");
                            InternalCalls.SetMusicVolume(musicVolume);
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
                        else if (currentButton == 2)
                        {
                            direction = 3 - currentButton;
                        }
                        else if (currentButton == 3)
                        {
                            direction = 2 - currentButton;
                        }
                        toMove = true;

                        //editing slider
                        if (currentButton == 4)
                        {
                            globalVolume++;
                            canvas.SetSliderValue(globalVolume, "Slider_MainVolume");
                            InternalCalls.SetMasterVolume(globalVolume);
                        }
                        if (currentButton == 5)
                        {
                            sfxVolume++;
                            canvas.SetSliderValue(sfxVolume, "Slider_SFX");
                            InternalCalls.SetSFXVolume(sfxVolume);
                        }
                        if (currentButton == 6)
                        {
                            musicVolume++;
                            canvas.SetSliderValue(musicVolume, "Slider_Music");
                            InternalCalls.SetMusicVolume(musicVolume);
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
                    //put here to toggle engine vsync variable
                    InternalCalls.SetVsync(vsync);
                }

                if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 1)
                {
                    attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                    fullscreen = !fullscreen;
                    canvas.ToggleChecker(fullscreen, "Checker_Fullscreen");
                    onCooldown = true;
                    //put here to toggle engine fullscreen variable
                    InternalCalls.SetFullscreen(fullscreen);
                }

                if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 2)
                {
                    attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                    pixelfx = !pixelfx;
                    canvas.ToggleChecker(pixelfx, "Checker_PixelFX");
                    onCooldown = true;
                    //put here to toggle engine pixelfx variable
                    InternalCalls.SetPixelFX(pixelfx);
                }

                if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 3)
                {
                    attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                    crtshader = !crtshader;
                    canvas.ToggleChecker(crtshader, "Checker_CRTShader");
                    onCooldown = true;
                    //put here to toggle engine crtshader variable
                    InternalCalls.SetCRTShader(crtshader);
                }
            }
        }
    }
}