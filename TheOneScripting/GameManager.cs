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