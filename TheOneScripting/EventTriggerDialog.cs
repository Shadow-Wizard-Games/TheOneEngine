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

    GameManager gameManager;
    UiManager menuManager;

    float playerDistance;

    float tpRange = 100.0f;
    bool inRange = false;

    string goName;

    string filepath = "Assets/GameData/Dialogs.json";
    string conversation;
    int dialogNum = 1;
    bool isFirst = true;
    bool isLast = false;
    float cooldown = 0.0f;

    IGameObject dialogueGo;
    ICanvas dialogCanvas;

    string audioEventString;

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        gameManager = IGameObject.Find("GameManager").GetComponent<GameManager>();

        eventType = EventType.OPENPOPUP;
        goName = attachedGameObject.name;

        menuManager = IGameObject.Find("UI_Manager").GetComponent<UiManager>();

        conversation = ExtractConversation();
        string[] datapath = { conversation };

        dialogueGo = IGameObject.Find("Canvas_Dialogue");
        dialogCanvas = dialogueGo.GetComponent<ICanvas>();
    }

    public override void Update()
    {
        if (cooldown > 0)
            cooldown -= Time.realDeltaTime;

        if (CheckEventIsPossible())
        {
            DoEvent();
        }

        if (gameManager.colliderRender) { DrawEventDebug(); }
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

        if ((Input.GetKeyboardButton(Input.KeyboardCode.E) || isFirst) && cooldown <= 0) //&& No esta en pause)
        {
            if(isFirst)
            {
                dialogueGo.Enable();
                // Pausar
            }
            else if (isLast)
            {
                // Resumear
                dialogueGo.Disable();
                if (Enum.TryParse(audioEventString, out AudioEvent aEvent))
                {
                    attachedGameObject.source.Play(aEvent);
                }
                attachedGameObject.source.Stop(aEvent);
                attachedGameObject.Destroy();
                return ret;
            }

            string[] datapath = { conversation, "Dialog" + dialogNum.ToString() };
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

    public string ExtractConversation()
    {
        string Dialog = "";

        string pattern = $"Conversation(\\d+)";

        Match match = Regex.Match(goName, pattern);

        if (match.Success)
        {
            Dialog += match.Groups[0].Value;
        }
        else { Debug.LogWarning("Could not find conversation in GO name"); }

        return Dialog;
    }
}