using System;
using System.Runtime.InteropServices;

public class UiScriptMissions : MonoBehaviour
{
    public enum MissionStatus
    {
        TODO,
        DONE
    }

    public ICanvas canvas;
    float cooldown = 0;
    bool onCooldown = false;
    int currentButton = 0;

    int maxMissions = 4;

    IGameObject missionSlot1;
    IGameObject missionSlot2;
    IGameObject missionSlot3;
    IGameObject missionSlot4;

    ICanvas missionCanvas1;
    ICanvas missionCanvas2;
    ICanvas missionCanvas3;
    ICanvas missionCanvas4;

    float flickerCooldown = 0;
    bool flickerOnCooldown = false;
    bool firstFrame = true;

    public MissionStatus missionStatus;

    public UiScriptMissions()
    {
        canvas = new ICanvas(InternalCalls.GetGameObjectPtr());
    }

    public override void Start()
    {
        missionSlot1 = IGameObject.Find("Canvas_MissionsSlot1");
        missionSlot2 = IGameObject.Find("Canvas_MissionsSlot2");
        missionSlot3 = IGameObject.Find("Canvas_MissionsSlot3");
        missionSlot4 = IGameObject.Find("Canvas_MissionsSlot4");

        missionCanvas1 = missionSlot1.GetComponent<ICanvas>();
        missionCanvas2 = missionSlot2.GetComponent<ICanvas>();
        missionCanvas3 = missionSlot3.GetComponent<ICanvas>();
        missionCanvas4 = missionSlot4.GetComponent<ICanvas>();

        missionStatus = MissionStatus.TODO;

        //here, if theres a list or something of missions, put maxMissions = missions.Size() (or whatever it is)
        //reminder, min missions is 0, Max missions is 4!
        //after testing, it doesnt work if its not 4 xd
        maxMissions = 4;

        onCooldown = true;

        firstFrame = true;
        flickerCooldown = 0;
        flickerOnCooldown = false;

        canvas.MoveSelectionButton(0 - canvas.GetSelectedButton(), true);
    }

    public override void Update()
    {
        float dt = Time.realDeltaTime;
        bool toMove = false;
        int direction = 0;

        if (firstFrame)
        {
            canvas.CanvasFlicker(true);
            missionCanvas1.CanvasFlicker(true);
            missionCanvas2.CanvasFlicker(true);
            missionCanvas3.CanvasFlicker(true);
            missionCanvas4.CanvasFlicker(true);
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
            missionCanvas1.CanvasFlicker(false);
            missionCanvas2.CanvasFlicker(false);
            missionCanvas3.CanvasFlicker(false);
            missionCanvas4.CanvasFlicker(false);
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

        //Input Updates
        if (!onCooldown)
        {
            if (Input.GetKeyboardButton(Input.KeyboardCode.UP))
            {
                if (currentButton == 0 || currentButton == 1)
                {
                    direction = maxMissions + 1 - currentButton;
                }
                else
                {
                    direction = -1;
                }
                toMove = true;
            }
            else if (Input.GetKeyboardButton(Input.KeyboardCode.DOWN))
            {
                if (currentButton == 0 || currentButton == 1)
                {
                    direction = 2 - currentButton;
                }
                else if (currentButton == maxMissions + 1)
                {
                    direction = 0 - currentButton;
                }
                else
                {
                    direction = +1;
                }
                toMove = true;
            }
            else if (Input.GetKeyboardButton(Input.KeyboardCode.LEFT))
            {
                if (currentButton == 0)
                {
                    missionStatus = MissionStatus.DONE;
                    direction = 1 - currentButton;
                }
                else if (currentButton == 1)
                {
                    missionStatus = MissionStatus.TODO;
                    direction = 0 - currentButton;
                }
                toMove = true;
            }
            else if (Input.GetKeyboardButton(Input.KeyboardCode.RIGHT))
            {
                if (currentButton == 0)
                {
                    missionStatus = MissionStatus.DONE;
                    direction = 1 - currentButton;
                }
                else if (currentButton == 1)
                {
                    missionStatus = MissionStatus.TODO;
                    direction = 0 - currentButton;
                }
                toMove = true;
            }

            //Controller
            Vector2 movementVector = Input.GetControllerJoystick(Input.ControllerJoystickCode.JOY_LEFT);

            if (movementVector.y != 0.0f)
            {
                //up
                if (movementVector.y < 0.0f)
                {
                    if (currentButton == 0 || currentButton == 1)
                    {
                        direction = maxMissions + 1 - currentButton;
                    }
                    else
                    {
                        direction = -1;
                    }
                    toMove = true;
                }
                //down
                else if (movementVector.y > 0.0f)
                {
                    if (currentButton == 0 || currentButton == 1)
                    {
                        direction = 2 - currentButton;
                    }
                    else if (currentButton == maxMissions + 1)
                    {
                        direction = 0 - currentButton;
                    }
                    else
                    {
                        direction = +1;
                    }
                    toMove = true;
                }
            }
            else if (movementVector.x != 0.0f)
            {
                //left
                if (movementVector.x < 0.0f)
                {
                    if (currentButton == 0)
                    {
                        missionStatus = MissionStatus.DONE;
                        direction = 1 - currentButton;
                    }
                    else if (currentButton == 1)
                    {
                        missionStatus = MissionStatus.TODO;
                        direction = 0 - currentButton;
                    }
                    toMove = true;
                }
                //right
                else if (movementVector.x > 0.0f)
                {
                    if (currentButton == 0)
                    {
                        if (currentButton == 0)
                        {
                            missionStatus = MissionStatus.DONE;
                            direction = 1 - currentButton;
                        }
                        else if (currentButton == 1)
                        {
                            missionStatus = MissionStatus.TODO;
                            direction = 0 - currentButton;
                        }
                        toMove = true;
                    }
                    toMove = true;
                }
            }

            if (toMove)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_HOVER);
                onCooldown = true;
                canvas.MoveSelectionButton(direction, true);
                currentButton += direction;

                //here it should be any option to pass to the text in the right panel info of the hovered mission
            }


            //for each mission, there should be a toggler of the checker ACTIVE of the hovered mission
            // Selection Executters
            if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 2)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                onCooldown = true;
            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 3)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                onCooldown = true;
            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 4)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                onCooldown = true;
            }

            if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN)) && canvas.GetSelection() == 5)
            {
                attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_CLICK);
                onCooldown = true;
            }
        }
    }
}