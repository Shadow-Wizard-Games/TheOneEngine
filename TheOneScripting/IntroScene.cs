using static IAudioSource;
using System;

class IntroScene : MonoBehaviour
{
    enum IntroSceneStates
    {
        INTRO = 0,
        DIALOG,
        END
    }

    public ICanvas canvasText;

    IGameObject gameObject;

    private float timerScene = 0.0f;
    private float durationScene = 2.0f;

    IntroSceneStates state;

    bool isLast = false;
    string filepath = "Assets/GameData/Dialogs.json";
    int dialogNum = 1;
    string audioEventString;

    public override void Start()
    {
        canvasText = new ICanvas(containerGOptr);
        gameObject = IGameObject.Find("IntroScene");

        state = IntroSceneStates.INTRO;
    }

    public override void Update()
    {
        timerScene += Time.deltaTime;

        if (timerScene >= durationScene)
        {
            canvasText.PrintItemUI(true, "ContinueTxt");

            if (Input.GetKeyboardButton(Input.KeyboardCode.RETURN) ||
            Input.GetControllerButton(Input.ControllerButtonCode.X))
            {
                if (state == IntroSceneStates.INTRO)
                {
                    state = IntroSceneStates.DIALOG;
                    canvasText.PrintItemUI(false, "IntroImg");
                    canvasText.PrintItemUI(true, "SubtitlesTxt");
                }

                switch (state)
                {
                    case IntroSceneStates.INTRO:
                        break;
                    case IntroSceneStates.DIALOG:
                        if (isLast)
                        {
                            state = IntroSceneStates.END;
                            canvasText.PrintItemUI(false, "SubtitlesTxt");
                            break;
                        }

                        string[] datapath = { "NarrativeIntro", "Dialog" + dialogNum.ToString() };
                        string text = DataManager.AccessFileDataString(filepath, datapath, "dialoguer");
                        text += DataManager.AccessFileDataString(filepath, datapath, "text");
                        durationScene = DataManager.AccessFileDataInt(filepath, datapath, "cooldown");
                        canvasText.SetTextString(text, "SubtitlesTxt");

                        isLast = DataManager.AccessFileDataBool(filepath, datapath, "isLast");
                        audioEventString = DataManager.AccessFileDataString(filepath, datapath, "audioEvent");
                        Debug.LogCheck(audioEventString);
                        if (Enum.TryParse(audioEventString, out AudioEvent audioEvent))
                        {
                            gameObject.source.Play(audioEvent);
                        }
                        dialogNum++;
                        break;
                    case IntroSceneStates.END:
                        break;
                    default:
                        break;
                }

                canvasText.PrintItemUI(false, "ContinueTxt");
                timerScene = 0.0f;
            }
        }

        if (Input.GetKeyboardButton(Input.KeyboardCode.SPACEBAR) || state == IntroSceneStates.END)
        {
            if (Enum.TryParse(audioEventString, out AudioEvent audioEvent))
            {
                gameObject.source.Stop(audioEvent);
            }
            
            SceneManager.LoadScene("L1R1");
        }
    }
}

