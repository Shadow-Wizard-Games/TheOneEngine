using System.Collections.Generic;
using System.Text.RegularExpressions;

public class GameManager : MonoBehaviour
{
    public bool hasSaved;
    public bool credits;
    string saveLevel;

    ItemManager itemManager;
    QuestManager questManager;

    public bool colliderRender;
    public bool gridRender;
    public bool godMode;
    public bool extraSpeed;

    bool gamePaused = false;

    public List<string> savedLevels = new List<string>();

    //Shortcuts logic
    public string lastLevel;
    public bool activeShortcutL1R1;

    public override void Start()
    {
        hasSaved = false;
        credits = false;

        colliderRender = true;
        gridRender = true;
        godMode = false;
        extraSpeed = false;

        lastLevel = " ";
        activeShortcutL1R1 = true;

        DrawColliders();
        DrawGrid();

        itemManager = IGameObject.Find("ItemManager").GetComponent<ItemManager>();
        questManager = IGameObject.Find("QuestManager").GetComponent<QuestManager>();
    }

    public override void Update()
    {
        ShortcutsConditions();
    }

    public void SaveSceneState()
    {
        if (!savedLevels.Contains(SceneManager.GetCurrentSceneName()))
            savedLevels.Add(SceneManager.GetCurrentSceneName());

        lastLevel = SceneManager.GetCurrentSceneName();
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
        questManager.ResetQuests();
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

    private void ShortcutsConditions() 
    {
        switch (SceneManager.GetCurrentSceneName())
        {
            case "L1R1":
                if (savedLevels.Contains("L1R4") && !activeShortcutL1R1)
                {
                    IGameObject.Find("SwapScene_L1_R4").Enable();
                    activeShortcutL1R1 = true;
                }
                else if (!savedLevels.Contains("L1R4") && activeShortcutL1R1)
                {
                    IGameObject.Find("SwapScene_L1_R4").Disable();
                    activeShortcutL1R1 = false;
                }
                break;
            default:
                break;
        }
    }


    public void TooglePause()
    {
        gamePaused = !gamePaused;
    }

    public bool GetGameState()
    {
        return gamePaused;
    }
}