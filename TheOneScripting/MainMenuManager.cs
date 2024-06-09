using System;

public class MainMenuManager : MonoBehaviour
{
    public ICanvas canvas;
    public ICanvas canvasEngineLogo;
    public ICanvas canvasLogo;
    public ICanvas canvasTitle;
    public ICanvas canvasCredits;

    UiScriptSettings settingsScript;

    float cooldown = 0;
    bool onCooldown = false;

    bool wasEditing = false;

    bool mainMenu = false;
    bool title = false;
    bool credits = false;
    bool logo = false;
    bool engineLogo = true;

    int creditsView = 0;

    public MainMenuManager()
    {
        canvas = new ICanvas(InternalCalls.GetGameObjectPtr());
    }

    public override void Start()
    {
        managers.Start();

        attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_A_MENU);
        canvasEngineLogo = IGameObject.Find("EngineLogo").GetComponent<ICanvas>();
        canvasLogo = IGameObject.Find("LogoCanvas").GetComponent<ICanvas>();
        canvasTitle = IGameObject.Find("TitleCanvas").GetComponent<ICanvas>();
        canvasCredits = IGameObject.Find("CreditsCanvas").GetComponent<ICanvas>();

        settingsScript = IGameObject.Find("Canvas_Settings").GetComponent<UiScriptSettings>();

        IGameObject.Find("Canvas_Settings").Disable();
        IGameObject.Find("Canvas_SettingsControls").Disable();
        IGameObject.Find("Canvas_SettingsDisplay").Disable();

        if (managers.gameManager != null)
        {
            if (managers.gameManager.credits)
            {
                logo = false;
                engineLogo = false;
                title = false;
                mainMenu = true;

                canvasLogo.ToggleEnable();
                canvasTitle.ToggleEnable();
            }
        }
    }
    public override void Update()
    {
        float dt = Time.realDeltaTime;
        bool toMove = false;
        int direction = 0;

        if (wasEditing && !settingsScript.editing) onCooldown = true;

        wasEditing = settingsScript.editing;

        if (onCooldown && cooldown < 0.2f)
        {
            cooldown += dt;
        }
        else
        {
            cooldown = 0.0f;
            onCooldown = false;
        }

        if (!onCooldown && (Input.GetControllerButton(Input.ControllerButtonCode.B) || Input.GetKeyboardButton(Input.KeyboardCode.ESCAPE)))
        {
            if (!settingsScript.editing)
            {
                IGameObject.Find("Canvas_Settings").Disable();
                mainMenu = true;
            }
        }

        if (!settingsScript.editing)
        {
            if ((title || logo || credits || engineLogo) && !onCooldown)
            {
                if (creditsView == 0)
                {
                    canvasCredits.PrintItemUI(true, "Text_Producer&Lead");
                    canvasCredits.PrintItemUI(false, "Text_ArtTeam");
                    canvasCredits.PrintItemUI(false, "Text_DesignTeam");
                    canvasCredits.PrintItemUI(false, "Text_CodeTeam");
                    onCooldown = true;
                    creditsView++;
                }
                if (Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN))
                {
                    if (engineLogo)
                    {
                        logo = true;
                        engineLogo = false;
                        canvasEngineLogo.ToggleEnable();
                        onCooldown = true;
                    }
                    else if (logo)
                    {
                        title = true;
                        logo = false;
                        canvasLogo.ToggleEnable();
                        onCooldown = true;
                    }
                    else if (credits)
                    {
                        if (creditsView == 1)
                        {
                            canvasCredits.PrintItemUI(false, "Text_Producer&Lead");
                            canvasCredits.PrintItemUI(true, "Text_ArtTeam");
                            canvasCredits.PrintItemUI(false, "Text_DesignTeam");
                            canvasCredits.PrintItemUI(false, "Text_CodeTeam");
                            attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                            onCooldown = true;
                            creditsView++;
                        }
                        else if (creditsView == 2)
                        {
                            canvasCredits.PrintItemUI(false, "Text_Producer&Lead");
                            canvasCredits.PrintItemUI(false, "Text_ArtTeam");
                            canvasCredits.PrintItemUI(true, "Text_DesignTeam");
                            canvasCredits.PrintItemUI(false, "Text_CodeTeam");
                            attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                            onCooldown = true;
                            creditsView++;
                        }
                        else if (creditsView == 3)
                        {
                            canvasCredits.PrintItemUI(false, "Text_Producer&Lead");
                            canvasCredits.PrintItemUI(false, "Text_ArtTeam");
                            canvasCredits.PrintItemUI(false, "Text_DesignTeam");
                            canvasCredits.PrintItemUI(true, "Text_CodeTeam");
                            attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                            onCooldown = true;
                            creditsView++;
                        }
                        else if (creditsView == 4)
                        {
                            canvasCredits.PrintItemUI(true, "Text_Producer&Lead");
                            canvasCredits.PrintItemUI(false, "Text_ArtTeam");
                            canvasCredits.PrintItemUI(false, "Text_DesignTeam");
                            canvasCredits.PrintItemUI(false, "Text_CodeTeam");
                            creditsView = 0;
                            credits = false;
                            mainMenu = true;
                            canvasCredits.ToggleEnable();
                            attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                            onCooldown = true;
                        }
                    }
                    else
                    {
                        title = false;
                        mainMenu = true;
                        canvasTitle.ToggleEnable();
                        onCooldown = true;
                        managers.gameManager.credits = true;
                    }
                }
            }

            if (mainMenu && !onCooldown)
            {
                //Keyboard
                if (Input.GetKeyboardButton(Input.KeyboardCode.UP))
                {
                    toMove = true;
                    direction = -1;
                }

                if (Input.GetKeyboardButton(Input.KeyboardCode.DOWN))
                {
                    toMove = true;
                    direction = 1;
                }

                //Controller
                Vector2 movementVector = Input.GetControllerJoystick(Input.ControllerJoystickCode.JOY_LEFT);

                if (movementVector.y != 0.0f)
                {
                    if (movementVector.y > 0.0f)
                    {
                        toMove = true;
                        direction = 1;
                    }
                    else if (movementVector.y < 0.0f)
                    {
                        toMove = true;
                        direction = -1;
                    }
                }

                // Select Button
                if (toMove && !onCooldown)
                {
                    onCooldown = true;
                    canvas.MoveSelectionButton(direction);
                    attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_HOVER);
                }

                // SELECTION EXECUTERS
                // New Game
                if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 0)
                {
                    managers.gameManager.ResetSave();
                    managers.questManager.StartGame();

                    DataManager.RemoveFile("GameData");
                    SceneManager.LoadScene("IntroScene");
                    attachedGameObject.source.Stop(IAudioSource.AudioEvent.UI_A_MENU);
                    attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                }

                // Resume Game
                if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 1)
                {
                    if (managers.gameManager.hasSaved)
                    {
                        managers.gameManager.LoadSave();
                        attachedGameObject.source.Stop(IAudioSource.AudioEvent.UI_A_MENU);
                        attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                    }
                }

                // Settings
                if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 2)
                {
                    IGameObject.Find("Canvas_Settings").Enable();
                    attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                    onCooldown = true;
                    mainMenu = false;
                }

                // Credits
                if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 3)
                {
                    credits = true;
                    canvasCredits.ToggleEnable();
                    attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                    onCooldown = true;
                    mainMenu = false;
                }

                // Exit
                if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 4)
                {
                    InternalCalls.ExitApplication();
                    attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                }
            }
        }

    }
}