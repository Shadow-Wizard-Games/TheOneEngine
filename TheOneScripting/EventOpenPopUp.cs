using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;

public class EventOpenPopUp : Event
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

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        gameManager = IGameObject.Find("GameManager").GetComponent<GameManager>();

        eventType = EventType.OPENPOPUP;
        goName = attachedGameObject.name;

        menuManager = IGameObject.Find("UI_Manager").GetComponent<UiManager>();
    }

    public override void Update()
    {
        if (CheckEventIsPossible())
        {
            DoEvent();
        }

        if (gameManager.colliderRender) { DrawEventDebug(); }
    }

    public override bool CheckEventIsPossible()
    {
        playerDistance = Vector3.Distance(playerGO.transform.position, attachedGameObject.transform.position);

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

        if (Input.GetKeyboardButton(Input.KeyboardCode.E))
        {
            string dialog = ExtractDialogNum();
            string[] datapath = { dialog };
            int popupType = DataManager.AccessFileDataInt(filepath, datapath, "popupType");
            string text = DataManager.AccessFileDataString(filepath, datapath, "text");
            int dialoguer = DataManager.AccessFileDataInt(filepath, datapath, "dialoguer");
            Debug.LogWarning("popupType: " + popupType);
            Debug.LogWarning("text: " + text);
            Debug.LogWarning("dialoguer: " + dialoguer);
            menuManager.OpenHudPopUpMenu((UiManager.HudPopUpMenu)popupType, text, (UiManager.Dialoguer)dialoguer);
        }

        return ret;
    }

    public override void DrawEventDebug()
    {
        if (!inRange)
        {
            Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, tpRange, new Vector3(1.0f, 0.8f, 0.0f)); //Yellow
        }
        else
        {
            Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, tpRange, new Vector3(0.9f, 0.0f, 0.9f)); //Purple
        }
    }

    public string ExtractDialogNum()
    {
        string Dialog = "";

        string pattern = $"Dialog(\\d+)";

        Match match = Regex.Match(goName, pattern);

        if (match.Success)
        {
            Dialog += match.Groups[0].Value;
        }
        else { Debug.LogWarning("Could not find scene name in GO name"); }

        return Dialog;
    }
}