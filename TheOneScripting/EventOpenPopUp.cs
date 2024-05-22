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
    string popup;
    int popupType;
    float cooldown = 0.0f;
    float maxCooldown;
    bool isOneTime;

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        gameManager = IGameObject.Find("GameManager").GetComponent<GameManager>();

        eventType = EventType.OPENPOPUP;
        goName = attachedGameObject.name;

        menuManager = IGameObject.Find("UI_Manager").GetComponent<UiManager>();

        popup = ExtractPopup();
        string[] datapath = { popup };
        popupType = DataManager.AccessFileDataInt(filepath, datapath, "popupType");
        isOneTime = DataManager.AccessFileDataBool(filepath, datapath, "isOneTime");
        maxCooldown = DataManager.AccessFileDataFloat(filepath, datapath, "cooldown");
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

        if (!menuManager.IsOnCooldown((UiManager.HudPopUpMenu)popupType) && cooldown <= 0 && !isOneTime)
        {
            string[] datapath = { popup };
            string text = DataManager.AccessFileDataString(filepath, datapath, "text");
            int dialoguer = DataManager.AccessFileDataInt(filepath, datapath, "dialoguer");
            menuManager.OpenHudPopUpMenu((UiManager.HudPopUpMenu)popupType, text, (UiManager.Dialoguer)dialoguer);

            cooldown = maxCooldown;
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

    public string ExtractPopup()
    {
        string Dialog = "";

        string pattern = $"Popup(\\d+)";

        Match match = Regex.Match(goName, pattern);

        if (match.Success)
        {
            Dialog += match.Groups[0].Value;
        }
        else { Debug.LogWarning("Could not find conversation in GO name"); }

        return Dialog;
    }
}