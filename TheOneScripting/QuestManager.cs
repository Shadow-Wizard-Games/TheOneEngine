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

    ItemManager itemManager;

    public override void Start()
    {
        quests = new List<Quest>();
        activeQuests = new List<Quest>();
        completedQuests = new List<Quest>();

        itemManager = IGameObject.Find("ItemManager").GetComponent<ItemManager>();

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

    void StartGame()
    {
        ActivateQuest(1);
    }

    public void ResetQuests()
    {
        activeQuests.Clear();
        completedQuests.Clear();
        StartGame();
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

    public void CompleteQuest(int id)
    {
        foreach (var item in activeQuests)
        {
            if (item.id == id)
            {
                UiManager uiManager = IGameObject.Find("UI_Manager").GetComponent<UiManager>();
                uiManager.OpenHudPopUpMenu(UiManager.HudPopUpMenu.PickUpFeedback, "Quest Completed:", item.name);
                item.GiveReward(itemManager);
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
}