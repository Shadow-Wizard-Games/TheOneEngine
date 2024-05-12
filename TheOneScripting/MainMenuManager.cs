using System;

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

    IGameObject GameManagerGO;
    GameManager gameManager;

    public MainMenuManager()
    {
        canvas = new ICanvas(InternalCalls.GetGameObjectPtr());
    }

    public override void Start()
    {
        attachedGameObject.source.PlayAudio(AudioManager.EventIDs.UI_A_MENU);
        canvasLogo = IGameObject.Find("LogoCanvas").GetComponent<ICanvas>();
        canvasTitle = IGameObject.Find("TitleCanvas").GetComponent<ICanvas>();

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
                attachedGameObject.source.PlayAudio(AudioManager.EventIDs.UI_HOVER);
            }

            // SELECTION EXECUTERS
            // New Game
            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 0)
            {
                if(gameManager.hasSaved) { gameManager.ResetSave(); }

                DataManager.RemoveFile("GameData");
                SceneManager.LoadScene("IntroScene");
                attachedGameObject.source.StopAudio(AudioManager.EventIDs.UI_A_MENU);
                attachedGameObject.source.PlayAudio(AudioManager.EventIDs.UI_CLICK);
            }

            // Resume Game
            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 1)
            {
                if (gameManager.hasSaved)
                {
                    gameManager.LoadSave();
                    attachedGameObject.source.StopAudio(AudioManager.EventIDs.UI_A_MENU);
                    attachedGameObject.source.PlayAudio(AudioManager.EventIDs.UI_CLICK);
                }
            }

            // Quit Game
            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelectedButton() == 3)
            {
                InternalCalls.ExitApplication();
                attachedGameObject.source.PlayAudio(AudioManager.EventIDs.UI_CLICK);
            }
        }
    }
}