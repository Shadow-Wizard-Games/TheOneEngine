using System;
using System.Collections.Generic;
using System.IO;
using static Item;

public class GameManager : MonoBehaviour
{
    public bool hasSaved;
    public bool credits;
    string saveLevel;

    ItemManager itemManager;

    public bool colliderRender;
    public bool gridRender;
    public bool godMode;
    public bool extraSpeed;

    public List<string> savedLevels = new List<string>();

    //Historn: Shortcuts hardcoded
    public bool activeShortcutL1R1 = false;

    public override void Start()
    {
        hasSaved = false;
        credits = false;

        colliderRender = true;
        gridRender = true;
        godMode = false;
        extraSpeed = false;

        DrawColliders();
        DrawGrid();

        itemManager = IGameObject.Find("ItemManager").GetComponent<ItemManager>();
    }

    public override void Update()
    {
        if (SceneManager.GetCurrentSceneName() == "L1R1" && savedLevels.Contains("L1R4") && !activeShortcutL1R1)
        {
            //shortcutToL1R4 = IGameObject.Find("S");
            Debug.Log("Shortcut L1R1 To L1R4 available");
            IGameObject.Find("SwapScene_L1_R4").Enable();
            activeShortcutL1R1 = true;
        }
        if (SceneManager.GetCurrentSceneName() == "L1R1" && !savedLevels.Contains("L1R4"))
        {
            IGameObject.Find("SwapScene_L1_R4").Disable();
        }
        //Debug.Log("Num of scenes saved " + savedLevels.Count); 
    }

    public void SaveSceneState()
    {
        if (!savedLevels.Contains(SceneManager.GetCurrentSceneName()))
            savedLevels.Add(SceneManager.GetCurrentSceneName());

        InternalCalls.CreateSaveFromScene("GameData/Scenes", SceneManager.GetCurrentSceneName());
    }

    public void UpdateSave()
    {
        DataManager.SaveGame();
        saveLevel = SceneManager.GetCurrentSceneName();
        hasSaved = true;
    }

    public void LoadSave()
    {
        bool notFound = true;
        foreach (var item in saveLevel)
        {
            if (notFound && item.ToString() == saveLevel)
            {
                notFound = false;
                continue;
            }
            else
                DataManager.RemoveFile(item.ToString());
        }

        SceneManager.LoadScene("LastSave_" + saveLevel, true, "GameData/");
    }

    public string GetSavedLevel()
    {
        return saveLevel;
    }

    public void ResetSave()
    {
        itemManager.ResetInventory();
    }

    public void DrawColliders()
    {
        colliderRender = !colliderRender;
        InternalCalls.ToggleCollidersDraw();
    }

    public void DrawGrid()
    {
        gridRender = !gridRender;
        InternalCalls.ToggleGridDraw();
    }
}