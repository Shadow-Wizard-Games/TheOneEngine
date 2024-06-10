﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Xml.Linq;

public class EventCollectible : Event
{
    IGameObject playerGO;
    UiManager uiManager;
    float playerDistance;

    string goName;
    string filepath = "Assets/GameData/Collectibles.json";

    readonly float collectibleRange = 100.0f;

    //debug
    bool inRange = false;
    bool brodcastMesage = true;

    public override void Start()
    {
        managers.Start();

        goName = attachedGameObject.name;

        playerGO = IGameObject.Find("SK_MainCharacter");
        eventType = EventType.COLLECTIBLE;

        uiManager = IGameObject.Find("UI_Manager").GetComponent<UiManager>();

        string collectibleName = ExtractCollectible();

        string[] dataPath = { collectibleName };
        int id = DataManager.AccessFileDataInt(filepath, dataPath, "itemId");

        if (managers.itemManager.CheckItemInInventory((uint)id))
            attachedGameObject.Destroy();
    }

    public override void Update()
    {

        if (CheckEventIsPossible() && managers.gameManager.GetGameState() == GameManager.GameStates.RUNNING)
        {
            DoEvent();
        }

        if (managers.gameManager.colliderRender) { DrawEventDebug(); }
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
            brodcastMesage = true;
        }
        return inRange;
    }

    public override bool DoEvent()
    {
        //Add item
        if (managers.itemManager != null)
        {
            if (brodcastMesage) { uiManager.OpenHudPopUpMenu(UiManager.HudPopUpMenu.PickUpFeedback, "Collectible:", "Press A"); brodcastMesage = false; }
            if (Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.E))
            {
                string collectibleName = ExtractCollectible();

                string[] dataPath = { collectibleName };
                int id = DataManager.AccessFileDataInt(filepath, dataPath, "itemId");
                int iammount = DataManager.AccessFileDataInt(filepath, dataPath, "itemAmmount");

                managers.itemManager.AddItem((uint)id, (uint)iammount);

                string feed = DataManager.AccessFileDataString(filepath, dataPath, "text");
                uiManager.OpenHudPopUpMenu(UiManager.HudPopUpMenu.PickUpFeedback, "Looted item:", feed);

                int qTriggerId = DataManager.AccessFileDataInt(filepath, dataPath, "triggerQuestId");
                int qcompleteId = DataManager.AccessFileDataInt(filepath, dataPath, "completeQuestId");
                managers.questManager.ActivateQuest(qTriggerId);
                managers.questManager.CompleteQuest(qcompleteId);

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
