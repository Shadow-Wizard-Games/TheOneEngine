using System;
using System.Runtime.CompilerServices;

public class Quest
{
    string filepath = "Assets/GameData/Quests.json";
    public int id { get; protected set; }
    public string name { get; protected set; }
    public string details { get; protected set; }
    public string objectives { get; protected set; }
    public int reward { get; protected set; }
    public int rewardAmmount { get; protected set; }
    public string type { get; protected set; }
    public bool isMain { get; protected set; }

    public Quest(string[] dataPath)
    {
        this.id = DataManager.AccessFileDataInt(filepath, dataPath, "id");
        this.name = DataManager.AccessFileDataString(filepath, dataPath, "name");
        this.details = DataManager.AccessFileDataString(filepath, dataPath, "details");
        this.objectives = DataManager.AccessFileDataString(filepath, dataPath, "objectives");
        this.reward = DataManager.AccessFileDataInt(filepath, dataPath, "reward");
        this.rewardAmmount = DataManager.AccessFileDataInt(filepath, dataPath, "rewardAmmount");
        this.type = DataManager.AccessFileDataString(filepath, dataPath, "type");
        this.isMain = DataManager.AccessFileDataBool(filepath, dataPath, "isMain");
    }

    public void GiveReward(ItemManager itemManager)
    {
        if (reward > 0 && rewardAmmount > 0)
            itemManager.AddItem((uint)reward, (uint)rewardAmmount);
    }
}