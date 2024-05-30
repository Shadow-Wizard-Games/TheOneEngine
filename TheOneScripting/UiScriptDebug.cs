using System;

public class UiScriptDebug : MonoBehaviour
{
    public ICanvas canvas;
    float cooldown = 0;
    bool onCooldown = false;

    float flickerCooldown = 0;
    bool flickerOnCooldown = false;
    bool firstFrame = true;

    IGameObject GameManagerGO;
    GameManager gameManager;

    public UiScriptDebug()
    {
        canvas = new ICanvas(InternalCalls.GetGameObjectPtr());
    }

    public override void Start()
    {
        GameManagerGO = IGameObject.Find("GameManager");
        gameManager = GameManagerGO.GetComponent<GameManager>();

        onCooldown = true;
        
        firstFrame = true;
        flickerCooldown = 0;
        flickerOnCooldown = false;

        canvas.MoveSelectionButton(0 - canvas.GetSelectedButton());

        UpdateCheckers();
    }

    public override void Update()
    {
        float dt = Time.realDeltaTime;
        bool toMove = false;
        int direction = 0;

        if (firstFrame)
        {
            canvas.CanvasFlicker(true);
            flickerOnCooldown = true;
            firstFrame = false;
        }

        if (flickerOnCooldown && flickerCooldown < 0.6f)
        {
            flickerCooldown += dt;
        }
        else
        {
            canvas.CanvasFlicker(false);
            flickerCooldown = 0.0f;
            flickerOnCooldown = false;
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

        UpdateCheckers();

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
                    direction = +2;
                    toMove = true;
                }
                else if (movementVector.y < 0.0f)
                {
                    direction = -2;
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
                canvas.MoveSelection(direction);
            }

            // Selection Executters
            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 0)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                gameManager.DrawColliders();
                onCooldown = true;
            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 1)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                gameManager.DrawGrid();
                onCooldown = true;
            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 2)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                gameManager.godMode = !gameManager.godMode;
                onCooldown = true;
            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 3)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                gameManager.extraSpeed = !gameManager.extraSpeed;
                onCooldown = true;
            }
        }
    }

    private void UpdateCheckers()
    {
        canvas.ToggleChecker(gameManager.colliderRender, "Checker_ColliderRender");
        canvas.ToggleChecker(gameManager.gridRender, "Checker_GridRender");
        canvas.ToggleChecker(gameManager.godMode, "Checker_GodMode");
        canvas.ToggleChecker(gameManager.extraSpeed, "Checker_ExtraSpeed");
    }
}