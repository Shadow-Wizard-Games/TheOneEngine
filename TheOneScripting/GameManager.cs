using System;
using System.Collections.Generic;
using System.IO;
using static Item;

public class GameManager : MonoBehaviour
{
    public bool resumeGame;
    public bool credits;
    string lastLevel = "L1R1";

    ItemManager itemManager;

    public override void Start()
    {
        resumeGame = false;
        credits = false;

        itemManager = IGameObject.Find("ItemManager").GetComponent<ItemManager>();
    }

    public override void Update()
    {
        
    }

    public void UpdateLevel()
    {
        lastLevel = SceneManager.GetCurrentSceneName();
        resumeGame = true;
    }

    public string GetSavedLevel()
    {
        return lastLevel;
    }

    public void ResetSave()
    {
        itemManager.ResetInventory();
    }
}