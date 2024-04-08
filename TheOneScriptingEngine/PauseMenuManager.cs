using System;

public class PauseMenuManager : MonoBehaviour
{
    public ICanvas canvas;
    float cooldown = 0;
    bool onCooldown = false;
    bool pauseEnabled = false;

    IGameObject playerGO;
    PlayerScript player;

    public PauseMenuManager()
    {
        canvas = new ICanvas(InternalCalls.GetGameObjectPtr());
    }

    public override void Start()
    {
        // Error here 
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();
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

        //Keyboard
        if (!onCooldown)
        {
            if (Input.GetKeyboardButton(Input.KeyboardCode.ESCAPE))
            {
                attachedGameObject.source.PlayAudio(AudioManager.EventIDs.UI_CLICK);
                canvas.ToggleEnable();
                pauseEnabled = true;
                onCooldown = true;
            }
        }

        if (pauseEnabled)
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
            if (toMove && !onCooldown)
            {
                attachedGameObject.source.PlayAudio(AudioManager.EventIDs.UI_HOVER);
                onCooldown = true;
                canvas.MoveSelection(direction);
            }

            // Selection Executters
            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 0)
            {
                attachedGameObject.source.PlayAudio(AudioManager.EventIDs.UI_CLICK);
                canvas.ToggleEnable();
                pauseEnabled = false;
            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 2)
            {
                attachedGameObject.source.PlayAudio(AudioManager.EventIDs.UI_CLICK);
                if (player.currentID == AudioManager.EventIDs.A_COMBAT_1)
                {
                    playerGO.source.StopAudio(AudioManager.EventIDs.A_COMBAT_1);
                }
                if (player.currentID == AudioManager.EventIDs.A_AMBIENT_1)
                {
                    playerGO.source.StopAudio(AudioManager.EventIDs.A_AMBIENT_1);
                }

                SceneManager.LoadScene("MainMenu");

            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 3)
            {
                attachedGameObject.source.PlayAudio(AudioManager.EventIDs.UI_CLICK);
                InternalCalls.ExitApplication();
            }
        }
    }
}