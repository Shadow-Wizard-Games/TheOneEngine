using System;
using System.Collections.Generic;
using System.IO;
using static Item;

public class QuestManager : MonoBehaviour
{
    List<Quest> quests;
    List<Quest> activeQuests;
    List<Quest> completedQuests;

    string filepath = "Assets/GameData/Quests.json";

    public int alienPc = 0;

    public override void Start()
    {
        managers.Start();

        quests = new List<Quest>();
        activeQuests = new List<Quest>();
        completedQuests = new List<Quest>();

        LoadQuests();
        StartGame();
    }

    public override void Update()
    {
        
    }

    void LoadQuests()
    {
        string[] dataPath = { "QuestManager" };
        int questNum = DataManager.AccessFileDataInt(filepath, dataPath, "questsNum");

        for (int i = 0; i < questNum; i++)
        {
            string[] questDataPath = {"Quests", "Quest" + i.ToString()};
            quests.Add(new Quest(questDataPath));
        }
    }

    public void StartGame()
    {
        ActivateQuest(1);
        ActivateQuest(2);
    }

    public void ResetQuests()
    {
        activeQuests.Clear();
        completedQuests.Clear();
    }

    public void ActivateQuest(int id)
    {
        foreach (var item in quests)
        {
            if (item.id == id)
            {
                activeQuests.Add(item);
                break;
            }
        }
    }

    public void CompleteQuest(int id, bool notify = true)
    {
        foreach (var item in activeQuests)
        {
            if (item.id == id)
            {
                if (notify)
                {
                    UiManager uiManager = IGameObject.Find("UI_Manager").GetComponent<UiManager>();
                    uiManager.OpenHudPopUpMenu(UiManager.HudPopUpMenu.PickUpFeedback, "Quest Completed:", item.name);
                }
                item.GiveReward(managers.itemManager);
                completedQuests.Add(item);
                activeQuests.Remove(item);
                break;
            }
        }
    }

    public bool IsQuestActive(int id)
    {
        foreach (var item in activeQuests)
        {
            if (item.id == id)
                return true;
        }

        return false;
    }

    public bool IsQuestComplete(int id)
    {
        foreach (var item in completedQuests)
        {
            if (item.id == id)
                return true;
        }

        return false;
    }

    public Quest GetMainQuest()
    {
        foreach (var item in activeQuests)
        {
            if (item.isMain)
                return item;
        }
        return null;
    }

    public Quest GetSideQuest()
    {
        foreach (var item in activeQuests)
        {
            if (!item.isMain)
                return item;
        }
        return null;
    }

    public void LoadQuestData()
    {
        string[] datapath1 = { "QuestManager" };

        int i = DataManager.AccessFileDataInt("GameData/SaveData.json", datapath1, "quantity1");
        alienPc = DataManager.AccessFileDataInt("GameData/SaveData.json", datapath1, "alienPc");

        for (int j = 0; j < i; j++)
        {
            string[] datapath = { "QuestManager", "activeQuests", "quest" + j.ToString() };
            int temp1 = DataManager.AccessFileDataInt("GameData/SaveData.json", datapath, "id");

            ActivateQuest(temp1);
        }

        i = DataManager.AccessFileDataInt("GameData/SaveData.json", datapath1, "quantity2");

        for (int j = 0; j < i; j++)
        {
            string[] datapath = { "QuestManager", "completedQuests", "quest" + j.ToString() };
            int temp1 = DataManager.AccessFileDataInt("GameData/SaveData.json", datapath, "id");

            ActivateQuest(temp1);
            CompleteQuest(temp1, false);
        }
    }

    public void SaveQuestData()
    {
        string[] datapath1 = { "QuestManager" };

        DataManager.WriteFileDataInt("GameData/SaveData.json", datapath1, "quantity1", activeQuests.Count);
        DataManager.WriteFileDataInt("GameData/SaveData.json", datapath1, "alienPc", alienPc);

        int i = 0;

        foreach (var item in activeQuests)
        {
            string[] datapath = { "QuestManager", "activeQuests", "quest" + i.ToString() };
            DataManager.WriteFileDataInt("GameData/SaveData.json", datapath, "id", item.id);
            i++;
        }

        DataManager.WriteFileDataInt("GameData/SaveData.json", datapath1, "quantity2", completedQuests.Count);

        i = 0;

        foreach (var item in completedQuests)
        {
            string[] datapath = { "QuestManager", "completedQuests", "quest" + i.ToString() };
            DataManager.WriteFileDataInt("GameData/SaveData.json", datapath, "id", item.id);
            i++;
        }
    }
}