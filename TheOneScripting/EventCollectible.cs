using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Xml.Linq;

public class EventCollectible : Event
{
    IGameObject playerGO;
    ItemManager itemManager;
    UiManager uiManager;
    float playerDistance;

    string goName;
    string filepath = "Assets/GameData/Collectibles.json";

    GameManager gameManager;
    QuestManager questManager;

    readonly float collectibleRange = 100.0f;

    //debug
    bool inRange = false;

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
        eventType = EventType.COLLECTIBLE;
        itemManager = IGameObject.Find("ItemManager").GetComponent<ItemManager>();
        gameManager = IGameObject.Find("GameManager").GetComponent<GameManager>();
        uiManager = IGameObject.Find("UI_Manager").GetComponent<UiManager>();
        questManager = IGameObject.Find("QuestManager").GetComponent<QuestManager>();
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
        //Set the distance to the player
        playerDistance = Vector3.Distance(playerGO.transform.Position, attachedGameObject.transform.Position);

        if (playerDistance < collectibleRange)
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
        //Add item
        if (itemManager != null)
        {
            if (Input.GetControllerButton(Input.ControllerButtonCode.Y) || Input.GetKeyboardButton(Input.KeyboardCode.E))
            {
                string collectibleName = ExtractCollectible();

                string[] dataPath = { collectibleName };
                int id = DataManager.AccessFileDataInt(filepath, dataPath, "itemId");
                int iammount = DataManager.AccessFileDataInt(filepath, dataPath, "itemAmmount");

                itemManager.AddItem((uint)id, (uint)iammount);

                string feed = DataManager.AccessFileDataString(filepath, dataPath, "text");
                uiManager.OpenHudPopUpMenu(UiManager.HudPopUpMenu.PickUpFeedback, "Looted item:", feed);

                int qTriggerId = DataManager.AccessFileDataInt(filepath, dataPath, "triggerQuestId");
                int qcompleteId = DataManager.AccessFileDataInt(filepath, dataPath, "completeQuestId");
                questManager.ActivateQuest(qTriggerId);
                questManager.CompleteQuest(qcompleteId);

                attachedGameObject.Destroy();
            }
        }

        return false;
    }

    public override void DrawEventDebug()
    {
        if (!inRange)
        {
            Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 4, collectibleRange, new Vector3(1.0f, 0.8f, 0.0f)); //Yellow
        }
        else
        {
            Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 4, collectibleRange, new Vector3(0.9f, 0.0f, 0.9f)); //Purple
        }
    }

    public string ExtractCollectible()
    {
        string Dialog = "";

        string pattern = $"Collectible(\\d+)";

        Match match = Regex.Match(goName, pattern);

        if (match.Success)
        {
            Dialog += match.Groups[0].Value;
        }
        else { Debug.LogWarning("Could not find character in GO name"); }

        return Dialog;
    }
}
