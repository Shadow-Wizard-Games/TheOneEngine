﻿using System;

public class MainMenuManager : MonoBehaviour
{
    public ICanvas canvas;
    public ICanvas canvasLogo;
    public ICanvas canvasTitle;
    float cooldown = 0;
    bool onCooldown = false;

    bool mainMenu = false;
    bool title = false;
    bool logo = true;

    public MainMenuManager()
    {
        canvas = new ICanvas(InternalCalls.GetGameObjectPtr());
    }

    public override void Start()
    {
        attachedGameObject.source.PlayAudio(AudioManager.EventIDs.UI_A_MENU);
        canvasLogo = IGameObject.Find("LogoCanvas").GetComponent<ICanvas>();
        canvasTitle = IGameObject.Find("TitleCanvas").GetComponent<ICanvas>();
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

        if ((title || logo) && !onCooldown)
        {
            if (Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN))
            {
                if (logo)
                {
                    title = true;
                    logo = false;
                    canvasLogo.ToggleEnable();
                    onCooldown = true;
                }
                else
                {
                    title = false;
                    mainMenu = true;
                    canvasTitle.ToggleEnable();
                    onCooldown = true;
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
                canvas.MoveSelection(direction);
                attachedGameObject.source.PlayAudio(AudioManager.EventIDs.UI_HOVER);
            }

            // Selection Executters
            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 0)
            {
                SceneManager.LoadScene("Level1");
                attachedGameObject.source.StopAudio(AudioManager.EventIDs.UI_A_MENU);
                attachedGameObject.source.PlayAudio(AudioManager.EventIDs.UI_CLICK);
            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 3)
            {
                InternalCalls.ExitApplication();
                attachedGameObject.source.PlayAudio(AudioManager.EventIDs.UI_CLICK);
            }
        }
    }
}