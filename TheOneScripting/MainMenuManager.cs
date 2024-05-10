﻿using System;

public class MainMenuManager : MonoBehaviour
{
    public ICanvas canvas;
    public ICanvas canvasLogo;
    public ICanvas canvasTitle;
    public ICanvas canvasCredits;
    float cooldown = 0;
    bool onCooldown = false;

    bool mainMenu = false;
    bool title = false;
    bool credits = false;
    bool logo = true;

    int creditsView = 0;

    IGameObject GameManagerGO;
    GameManager gameManager;

    public MainMenuManager()
    {
        canvas = new ICanvas(InternalCalls.GetGameObjectPtr());
    }

    public override void Start()
    {
        attachedGameObject.source.PlayAudio(IAudioSource.Events.UI_A_MENU);
        canvasLogo = IGameObject.Find("LogoCanvas").GetComponent<ICanvas>();
        canvasTitle = IGameObject.Find("TitleCanvas").GetComponent<ICanvas>();
        canvasCredits = IGameObject.Find("CreditsCanvas").GetComponent<ICanvas>();

        GameManagerGO = IGameObject.Find("GameManager");
        gameManager = GameManagerGO.GetComponent<GameManager>();

        if (gameManager != null)
        {
            if (gameManager.credits)
            {
                logo = false;
                title = false;
                mainMenu = true;

                canvasLogo.ToggleEnable();
                canvasTitle.ToggleEnable();
            }
        }
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

        if ((title || logo || credits) && !onCooldown)
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
            if (Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN))
            {
                if (logo)
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
                        attachedGameObject.source.PlayAudio(IAudioSource.Events.UI_CLICK);
                        onCooldown = true;
                        creditsView++;
                    }
                    else if (creditsView == 2)
                    {
                        canvasCredits.PrintItemUI(false, "Text_Producer&Lead");
                        canvasCredits.PrintItemUI(false, "Text_ArtTeam");
                        canvasCredits.PrintItemUI(true, "Text_DesignTeam");
                        canvasCredits.PrintItemUI(false, "Text_CodeTeam");
                        attachedGameObject.source.PlayAudio(IAudioSource.Events.UI_CLICK);
                        onCooldown = true;
                        creditsView++;
                    }
                    else if (creditsView == 3)
                    {
                        canvasCredits.PrintItemUI(false, "Text_Producer&Lead");
                        canvasCredits.PrintItemUI(false, "Text_ArtTeam");
                        canvasCredits.PrintItemUI(false, "Text_DesignTeam");
                        canvasCredits.PrintItemUI(true, "Text_CodeTeam");
                        attachedGameObject.source.PlayAudio(IAudioSource.Events.UI_CLICK);
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
                        attachedGameObject.source.PlayAudio(IAudioSource.Events.UI_CLICK);
                        onCooldown = true;
                    }
                }
                else
                {
                    title = false;
                    mainMenu = true;
                    canvasTitle.ToggleEnable();
                    onCooldown = true;
                    gameManager.credits = true;
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
                attachedGameObject.source.PlayAudio(IAudioSource.Events.UI_HOVER);
            }

            // Selection Executters
            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 0)
            {
                if(gameManager.resumeGame) { gameManager.ResetSave(); }

                SceneManager.LoadScene("IntroScene");
                attachedGameObject.source.StopAudio(IAudioSource.Events.UI_A_MENU);
                attachedGameObject.source.PlayAudio(IAudioSource.Events.UI_CLICK);
            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 1)
            {
                if (gameManager.resumeGame)
                {
                    SceneManager.LoadScene(gameManager.GetSavedLevel());
                    attachedGameObject.source.StopAudio(IAudioSource.Events.UI_A_MENU);
                    attachedGameObject.source.PlayAudio(IAudioSource.Events.UI_CLICK);
                }
            }

            //to add: settings


            //to add: credits
            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 3)
            {
                credits = true;
                canvasCredits.ToggleEnable();
                attachedGameObject.source.PlayAudio(IAudioSource.Events.UI_CLICK);
                onCooldown = true;
                mainMenu = false;
            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 4)
            {
                InternalCalls.ExitApplication();
                attachedGameObject.source.PlayAudio(IAudioSource.Events.UI_CLICK);
            }
        }
    }
}