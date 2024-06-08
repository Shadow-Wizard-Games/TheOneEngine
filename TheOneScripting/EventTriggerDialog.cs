using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;
using static UiManager;
using static IAudioSource;

public class EventTriggerDialog : Event
{
    IGameObject playerGO;
    PlayerScript player;

    float playerDistance;

    float tpRange = 100.0f;
    bool inRange = false;

    string goName;

    string filepath = "Assets/GameData/Dialogs.json";
    string dynamicPath = "GameData/Dialogs.json";
    string charachter;
    int dialogNum = 1;
    int conversationNum = 1;
    int neededQuestId = -1;
    bool isFirst = true;
    bool isLast = false;
    float cooldown = 0.0f;

    IGameObject dialogueGo;
    ICanvas dialogCanvas;

    IParticleSystem dialogWarnig;

    string audioEventString;

    public override void Start()
    {
        managers.Start();

        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        eventType = EventType.OPENPOPUP;
        goName = attachedGameObject.name;

        charachter = ExtractCharacter();
        string[] characterPath = { charachter };
        conversationNum = DataManager.AccessFileDataInt(dynamicPath, characterPath, "conversationNum");

        dialogWarnig = attachedGameObject.FindInChildren("PS_DialogWarning")?.GetComponent<IParticleSystem>();

        if(conversationNum <= 0)
        {
            conversationNum = 1;
        }

        string[] dataPath = { charachter, "Conversation" + conversationNum };
        neededQuestId = DataManager.AccessFileDataInt(filepath, dataPath, "neededQuestId");

        dialogueGo = IGameObject.Find("Canvas_Dialogue");
        dialogCanvas = dialogueGo.GetComponent<ICanvas>();
    }

    public override void Update()
    {
        attachedGameObject.animator.UpdateAnimation();
        UpdateAnimator();

        if (cooldown > 0)
            cooldown -= Time.realDeltaTime;

        if (managers.questManager.IsQuestComplete(neededQuestId) || neededQuestId == -1)
        {
            dialogWarnig.Play();
            if (CheckEventIsPossible())
            {
                DoEvent();
            }
        }
        else { dialogWarnig.Stop(); }

        if (managers.gameManager.colliderRender) { DrawEventDebug(); }
    }

    public override bool CheckEventIsPossible()
    {
        playerDistance = Vector3.Distance(playerGO.transform.Position, attachedGameObject.transform.Position);

        if (playerDistance < tpRange)
        {
            inRange = true;
        }
        else
        {
            inRange = false;
        }

        return inRange;
    }

    public override bool DoEvent()
    {
        bool ret = true;

        if (Input.GetControllerButton(Input.ControllerButtonCode.A) ||  Input.GetKeyboardButton(Input.KeyboardCode.E) && cooldown <= 0)
        {
            if (isFirst)
            {
                managers.gameManager.SetGameState(GameManager.GameStates.DIALOGING);
                dialogueGo.Enable();
            }
            else if (isLast)
            {
                string[] dataPath1 = { charachter, "Conversation" + conversationNum.ToString() };
                int completeQuestId = DataManager.AccessFileDataInt(filepath, dataPath1, "completeQuestId");
                managers.questManager.CompleteQuest(completeQuestId);
                int triggerQuestId = DataManager.AccessFileDataInt(filepath, dataPath1, "triggerQuestId");
                managers.questManager.ActivateQuest(triggerQuestId);

                conversationNum++;
                string[] characterPath = { charachter };
                DataManager.WriteFileDataInt(dynamicPath, characterPath, "conversationNum", conversationNum);

                string[] dataPath = { charachter, "Conversation" + conversationNum.ToString() };
                neededQuestId = DataManager.AccessFileDataInt(filepath, dataPath, "neededQuestId");

                managers.gameManager.SetGameState(GameManager.GameStates.RUNNING);
                dialogueGo.Disable();

                return ret;
            }

            if (Enum.TryParse(audioEventString, out AudioEvent aEvent))
            {
                attachedGameObject.source.Stop(aEvent);
            }

            string[] datapath = { charachter, "Conversation" + conversationNum.ToString(), "Dialog" + dialogNum.ToString() };
            string text = DataManager.AccessFileDataString(filepath, datapath, "text");
            isLast = DataManager.AccessFileDataBool(filepath, datapath, "isLast");
            int dialoguer = DataManager.AccessFileDataInt(filepath, datapath, "dialoguer");
            cooldown = DataManager.AccessFileDataInt(filepath, datapath, "cooldown");

            audioEventString = DataManager.AccessFileDataString(filepath, datapath, "audioEvent");
            if (Enum.TryParse(audioEventString, out AudioEvent audioEvent))
            {
                attachedGameObject.source.Play(audioEvent);
            }

            dialogCanvas.SetTextString(text, "Text_Dialogue");
            dialogCanvas.PrintItemUI(false, "Img_ShopKeeper");
            dialogCanvas.PrintItemUI(false, "Img_Medic");
            dialogCanvas.PrintItemUI(false, "Img_CampLeader");
            dialogCanvas.PrintItemUI(false, "Img_Sargeant");
            dialogCanvas.PrintItemUI(false, "Img_Default");

            switch ((UiManager.Dialoguer)dialoguer)
            {
                case Dialoguer.ShopKeeper:
                    dialogCanvas.PrintItemUI(true, "Img_ShopKeeper");
                    break;
                case Dialoguer.Medic:
                    dialogCanvas.PrintItemUI(true, "Img_Medic");
                    break;
                case Dialoguer.CampLeader:
                    dialogCanvas.PrintItemUI(true, "Img_CampLeader");
                    break;
                case Dialoguer.Sargeant:
                    dialogCanvas.PrintItemUI(true, "Img_Sargeant");
                    break;
                case Dialoguer.Default:
                    dialogCanvas.PrintItemUI(true, "Img_Default");
                    break;
                default:
                    break;
            }

            if (!isLast)
            {
                dialogNum++;
                isFirst = false;
            }
        }

        return ret;
    }

    public override void DrawEventDebug()
    {
        if (!inRange)
        {
            Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 4, tpRange, new Vector3(1.0f, 0.8f, 0.0f)); //Yellow
        }
        else
        {
            Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 4, tpRange, new Vector3(0.9f, 0.0f, 0.9f)); //Purple
        }
    }

    public string ExtractCharacter()
    {
        string Dialog = "";

        string pattern = $"Character(\\d+)";

        Match match = Regex.Match(goName, pattern);

        if (match.Success)
        {
            Dialog += match.Groups[0].Value;
        }
        else { Debug.LogWarning("Could not find character in GO name"); }

        return Dialog;
    }

    private void UpdateAnimator()
    {
        if (managers.questManager.IsQuestComplete(neededQuestId) && managers.gameManager.GetGameState() != GameManager.GameStates.DIALOGING)
            attachedGameObject.animator.Play("Greetings");
        else if(managers.gameManager.GetGameState() == GameManager.GameStates.DIALOGING)
            attachedGameObject.animator.Play("Talk");
        else
            attachedGameObject.animator.Play("Idle");
    }
}