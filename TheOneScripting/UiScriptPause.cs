using System;

public class UiScriptPause : MonoBehaviour
{
    public ICanvas canvas;
    float cooldown = 0;
    bool onCooldown = false;

    IGameObject playerGO;
    PlayerScript player;

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

        onCooldown = true;
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

        //Input Updates
        if (!onCooldown)
        {
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
            if (toMove)
            {
                attachedGameObject.source.PlayAudio(IAudioSource.EventIDs.UI_HOVER);
                onCooldown = true;
                canvas.MoveSelection(direction);
            }

            // Selection Executters
            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 0)
            {
                attachedGameObject.source.PlayAudio(IAudioSource.EventIDs.UI_CLICK);
                canvas.ToggleEnable();
                menuManager.ResumeGame();
            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 2)
            {
                attachedGameObject.source.PlayAudio(IAudioSource.EventIDs.UI_CLICK);
                if (playerGO.source.currentID == IAudioSource.EventIDs.A_COMBAT_1)
                {
                    playerGO.source.StopAudio(IAudioSource.EventIDs.A_COMBAT_1);
                }
                if (playerGO.source.currentID == IAudioSource.EventIDs.A_AMBIENT_1)
                {
                    playerGO.source.StopAudio(IAudioSource.EventIDs.A_AMBIENT_1);
                }

                SceneManager.LoadScene("MainMenu");

            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 3)
            {
                attachedGameObject.source.PlayAudio(IAudioSource.EventIDs.UI_CLICK);
                InternalCalls.ExitApplication();
            }
        }
    }
}