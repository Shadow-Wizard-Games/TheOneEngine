using System;

public class UiScriptDebug : MonoBehaviour
{
    public ICanvas canvas;
    float cooldown = 0;
    bool onCooldown = false;

    float flickerCooldown = 0;
    bool flickerOnCooldown = false;
    public bool firstFrame = true;

    public UiScriptDebug()
    {
        canvas = new ICanvas(InternalCalls.GetGameObjectPtr());
    }

    public override void Start()
    {
        managers.Start();

        onCooldown = true;
        
        firstFrame = true;
        flickerCooldown = 0;
        flickerOnCooldown = false;

        canvas.MoveSelectionButton(0 - canvas.GetSelectedButton());

        //UpdateCheckers();
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
            if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 0)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                managers.gameManager.DrawColliders();
                onCooldown = true;
            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 1)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);

                managers.gameManager.currency += 10000;

                managers.gameManager.DrawGrid();
                onCooldown = true;
            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 2)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                managers.gameManager.godMode = !managers.gameManager.godMode;
                InternalCalls.SetCollisionSolverActive(!managers.gameManager.godMode);
                onCooldown = true;
            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 3)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                managers.gameManager.extraSpeed = !managers.gameManager.extraSpeed;
                onCooldown = true;
            }
        }
    }

    private void UpdateCheckers()
    {
        canvas.ToggleChecker(managers.gameManager.colliderRender, "Checker_ColliderRender");
        canvas.ToggleChecker(managers.gameManager.gridRender, "Checker_GridRender");
        canvas.ToggleChecker(managers.gameManager.godMode, "Checker_GodMode");
        canvas.ToggleChecker(managers.gameManager.extraSpeed, "Checker_ExtraSpeed");
    }
}