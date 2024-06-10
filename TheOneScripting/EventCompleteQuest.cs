using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;
using System.Collections;

public class EventCompleteQuest : Event
{
    IGameObject playerGO;

    float playerDistance;

    readonly float tpRange = 100.0f;
    bool inRange = false;

    string goName;
    int id;
    int nextId;
    int prevId;

    string filepath = "Assets/GameData/Quests.json";

    public override void Start()
    {
        managers.Start();

        playerGO = IGameObject.Find("SK_MainCharacter");

        eventType = EventType.QUESTCOMPLETE;
        goName = attachedGameObject.name;

        string[] datapath = { "CompletionEvents", ExtractQuestCompletion() };
        id = DataManager.AccessFileDataInt(filepath, datapath, "id");
        nextId = DataManager.AccessFileDataInt(filepath, datapath, "nextId");
        prevId = DataManager.AccessFileDataInt(filepath, datapath, "prevId");
    }

    public override void Update()
    {
        if (CheckEventIsPossible())
        {
            DoEvent();
        }

        if (managers.gameManager.colliderRender) { DrawEventDebug(); }
    }

    public override bool CheckEventIsPossible()
    {
        if (!managers.questManager.IsQuestActive(id))
            return false;

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
        Debug.Log(prevId.ToString());
        if (managers.questManager.IsQuestComplete(prevId) || prevId == 0)
        {
            managers.questManager.CompleteQuest(id);
            managers.questManager.ActivateQuest(nextId);

            attachedGameObject.Destroy();
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

    public string ExtractQuestCompletion()
    {
        string Dialog = "";

        string pattern = $"EventQC(\\d+)";

        Match match = Regex.Match(goName, pattern);

        if (match.Success)
        {
            Dialog += match.Groups[0].Value;
        }
        else { Debug.LogWarning("Could not find EventQC in GO name"); }

        return Dialog;
    }
}