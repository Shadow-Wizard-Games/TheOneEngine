using System;

public class UiScriptPause : MonoBehaviour
{
    public ICanvas canvas;
    float cooldown = 0;
    bool onCooldown = false;

    IGameObject playerGO;
    PlayerScript player;

    IGameObject GameManagerGO;
    GameManager gameManager;

    UiManager menuManager;

    public UiScriptPause()
    {
        canvas = new ICanvas(InternalCalls.GetGameObjectPtr());
    }

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        menuManager = IGameObject.Find("UI_Manager").GetComponent<UiManager>();

        GameManagerGO = IGameObject.Find("GameManager");
        gameManager = GameManagerGO.GetComponent<GameManager>();

        onCooldown = true;

        canvas.MoveSelectionButton(2 - canvas.GetSelectedButton());
    }

    public override void Update()
    {
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

        //Input Updates
        if (!onCooldown)
        {
            if (Input.GetKeyboardButton(Input.KeyboardCode.UP))
            {
                direction = -2;
                toMove = true;
            }
            else if (Input.GetKeyboardButton(Input.KeyboardCode.DOWN))
            {
                direction = +2;
                toMove = true;
            }
            else if (Input.GetKeyboardButton(Input.KeyboardCode.LEFT))
            {
                direction = -1;
                toMove = true;
            }
            else if (Input.GetKeyboardButton(Input.KeyboardCode.RIGHT))
            {
                direction = +1;
                toMove = true;
            }

            //Controller
            Vector2 movementVector = Input.GetControllerJoystick(Input.ControllerJoystickCode.JOY_LEFT);

            if (movementVector.y != 0.0f)
            {
                if (movementVector.y > 0.0f)
                {
                    direction = -2;
                    toMove = true;
                }
                else if (movementVector.y < 0.0f)
                {
                    direction = +2;
                    toMove = true;
                }
            }
            else if (movementVector.x != 0.0f)
            {
                if (movementVector.x > 0.0f)
                {
                    direction = +1;
                    toMove = true;
                }
                else if (movementVector.x < 0.0f)
                {
                    direction = -1;
                    toMove = true;
                }
            }

            // Select Button
            if (toMove)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_HOVER);
                onCooldown = true;
                canvas.MoveSelectionButton(direction);
            }

            // Selection Executters
            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 0)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                menuManager.OpenMenu(UiManager.MenuState.Inventory);
            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 1)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                menuManager.OpenMenu(UiManager.MenuState.Missions);
            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 2)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                menuManager.ResumeGame();
            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 3)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                menuManager.OpenMenu(UiManager.MenuState.Settings);
            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 4)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                playerGO.source.Play(IAudioSource.AudioEvent.STOPMUSIC);

                SceneManager.LoadScene("MainMenu");
            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 5)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                InternalCalls.ExitApplication();
            }
        }
    }
}