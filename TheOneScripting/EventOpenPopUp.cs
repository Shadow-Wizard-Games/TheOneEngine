using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;
using static UiManager;
using static IAudioSource;

public class EventOpenPopUp : Event
{
    IGameObject playerGO;
    PlayerScript player;

    UiManager menuManager;

    float playerDistance;

    float tpRange = 100.0f;
    bool inRange = false;

    string goName;

    string filepath = "Assets/GameData/Dialogs.json";
    UiManager.HudPopUpMenu popupType;
    string popupStr;
    float cooldown = 0.0f;
    float maxCooldown;
    bool isOneTime;
    string audioEventString;

    public override void Start()
    {
        managers.Start();

        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        eventType = EventType.OPENPOPUP;
        goName = attachedGameObject.name;

        menuManager = IGameObject.Find("UI_Manager").GetComponent<UiManager>();

        popupStr = ExtractPopup();
        string[] datapath = { popupStr };
        int popupInt = DataManager.AccessFileDataInt(filepath, datapath, "popupType");

        popupType = HudPopUpMenu.PickUpFeedback;
        if (DataManager.IsValidEnumValue<UiManager.HudPopUpMenu>(popupInt))
        {
            popupType = (UiManager.HudPopUpMenu)popupInt;
        }

        isOneTime = DataManager.AccessFileDataBool(filepath, datapath, "isOneTime");
        maxCooldown = DataManager.AccessFileDataInt(filepath, datapath, "cooldown");
    }

    public override void Update()
    {
        if (!menuManager.IsOnCooldown(popupType) && cooldown > 0)
            cooldown -= Time.realDeltaTime;

        if (CheckEventIsPossible())
        {
            DoEvent();
        }

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

        if (!menuManager.IsOnCooldown(popupType) && cooldown <= 0)
        {
            string[] datapath = { popupStr };
            string text = DataManager.AccessFileDataString(filepath, datapath, "text");
            string text1 = DataManager.AccessFileDataString(filepath, datapath, "titleText");
            int dialoguer = DataManager.AccessFileDataInt(filepath, datapath, "dialoguer");

            Dialoguer dialoguerEnum = Dialoguer.None;
            if (DataManager.IsValidEnumValue<UiManager.Dialoguer>(dialoguer))
            {
                dialoguerEnum = (UiManager.Dialoguer)dialoguer;
            }

            float duration = DataManager.AccessFileDataInt(filepath, datapath, "duration");
            menuManager.OpenHudPopUpMenu(popupType, text1, text, dialoguerEnum, duration);

            audioEventString = DataManager.AccessFileDataString(filepath, datapath, "audioEvent");
            if (Enum.TryParse(audioEventString, out AudioEvent audioEvent))
            {
                attachedGameObject.source.Play(audioEvent);
            }

            cooldown = maxCooldown;

            if(isOneTime) { attachedGameObject.Destroy(); }
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
        else { Debug.LogWarning("Could not find Popup in GO name"); }

        return Dialog;
    }
}