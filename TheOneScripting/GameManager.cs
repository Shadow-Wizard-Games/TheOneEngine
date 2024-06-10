using System.Collections.Generic;
using System.Text.RegularExpressions;
using static UiManager;

public class GameManager : MonoBehaviour
{
    public enum GameStates
    {
        PAUSED = 0,
        RUNNING,
        DIALOGING,
        ONMENUS
    }

    public bool hasSaved;
    public bool credits;
    string saveLevel;

    public bool colliderRender;
    public bool gridRender;
    public bool godMode;
    public bool extraSpeed;

    private GameStates state = GameStates.PAUSED;

    public List<string> savedLevels = new List<string>();

    //Shortcuts logic
    public string lastLevel;
    public bool activeShortcutL1R1;
    public bool endGame = false;

    #region PLAYERDATA

    // Levels
    int damageLvl;
    int lifeLvl;
    int speedLvl;

    public int GetDamageLvl() { return damageLvl; }
    public int GetLifeLvl() { return lifeLvl; }
    public int GetSpeedLvl() { return speedLvl; }

    // Stats
    float maxHealth;
    float damage;
    float speed;

    public float GetMaxHealth() { return maxHealth; }
    public float GetDamage() { return damage; }
    public float GetSpeed() { return speed; }

    public float health;

    public int currency;

    public void ResetHealth() { health = maxHealth; }

    public void SetDamageLvl(int damageLvl)
    {
        this.damageLvl = damageLvl;

        switch (damageLvl)
        {
            case 0: damage = 0; break;
            case 1: damage = 5; break;
            case 2: damage = 10; break;
            case 3: damage = 15; break;
        }
    }

    public void SetSpeedLvl(int speedLvl)
    {
        this.speedLvl = speedLvl;

        switch (speedLvl)
        {
            case 0: speed = 80; break;
            case 1: speed = 90; break;
            case 2: speed = 100; break;
            case 3: speed = 110; break;
        }
    }

    public void SetLifeLvl(int lifeLvl)
    {
        this.lifeLvl = lifeLvl;

        switch (lifeLvl)
        {
            case 0: maxHealth = 100; break;
            case 1: maxHealth = 120; break;
            case 2: maxHealth = 140; break;
            case 3: maxHealth = 160; break;
        }
    }

    #endregion

    public override void Start()
    {
        managers.Start();

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

        string[] datapath = { "GameManager" };
        hasSaved = DataManager.AccessFileDataBool("GameData/SaveData.json", datapath, "hasSaved");

        ResetPlayerData();
    }

    public override void Update()
    {
        
    }

    public void SetGameState(GameStates state)
    {
        this.state = state;
    }

    public void SaveSceneState()
    {
        if (!savedLevels.Contains(SceneManager.GetCurrentSceneName()))
            savedLevels.Add(SceneManager.GetCurrentSceneName());

        lastLevel = SceneManager.GetCurrentSceneName();

        ShortcutsConditions();
    }

    public void UpdateSave()
    {
        DataManager.SaveGame();
        saveLevel = SceneManager.GetCurrentSceneName();
        this.SaveGameManagerData();
        managers.itemManager.SaveInventoryData();
        managers.questManager.SaveQuestData();

        hasSaved = true;
    }

    public void LoadSave()
    {
        if (!hasSaved)
            return;

        ResetSave();
        hasSaved = true;

        this.LoadGameManagerData();
        managers.itemManager.LoadInventoryData();
        managers.questManager.LoadQuestData();


        SceneManager.LoadScene("LastSave_" + saveLevel, true, "GameData/");
    }

    public string GetSavedLevel()
    {
        return saveLevel;
    }

    public void ResetSave()
    {
        lastLevel = " ";
        managers.itemManager.ResetInventory();
        managers.questManager.ResetQuests();
        ResetPlayerData();
        activeShortcutL1R1 = false;
        hasSaved = false;
        savedLevels.Clear();
    }

    private void ResetPlayerData()
    {
        SetLifeLvl(0);
        SetDamageLvl(0);
        SetSpeedLvl(0);
        ResetHealth();
        currency = 0;
    }

    public void DrawColliders()
    {
        //colliderRender = !colliderRender;
        //InternalCalls.ToggleCollidersDraw();
    }

    public void EndGame()
    {
        endGame = true;
        
        ResetSave();
        managers.questManager.StartGame();

        DataManager.RemoveFile("GameData");
        SceneManager.LoadScene("MainMenu");
    }

    public void DrawGrid()
    {
        gridRender = !gridRender;
        InternalCalls.ToggleGridDraw();
    }

    private void ShortcutsConditions() 
    {
        if(!activeShortcutL1R1 && savedLevels.Contains("L1R4"))
            activeShortcutL1R1 = true;
    }

    public GameStates GetGameState()
    {
        return state;
    }

    private void SaveGameManagerData()
    {
        string[] datapath = { "GameManager" };

        //hasSaved
        //saveLevel
        //savedLevels list
        //lastLevel

        DataManager.WriteFileDataInt("GameData/SaveData.json", datapath, "currency", currency);
        DataManager.WriteFileDataInt("GameData/SaveData.json", datapath, "damageLvl", damageLvl);
        DataManager.WriteFileDataInt("GameData/SaveData.json", datapath, "lifeLvl", lifeLvl);
        DataManager.WriteFileDataInt("GameData/SaveData.json", datapath, "speedLvl", speedLvl);
        DataManager.WriteFileDataBool("GameData/SaveData.json", datapath, "activeShortcutL1R1", activeShortcutL1R1);
        DataManager.WriteFileDataBool("GameData/SaveData.json", datapath, "hasSaved", hasSaved);
        DataManager.WriteFileDataString("GameData/SaveData.json", datapath, "saveLevel", saveLevel);
    }

    private void LoadGameManagerData()
    {
        string[] datapath = { "GameManager" };

        //hasSaved
        //saveLevel
        //savedLevels list
        //lastLevel

        currency = DataManager.AccessFileDataInt("GameData/SaveData.json", datapath, "currency");
        SetDamageLvl(DataManager.AccessFileDataInt("GameData/SaveData.json", datapath, "damageLvl"));
        SetLifeLvl(DataManager.AccessFileDataInt("GameData/SaveData.json", datapath, "lifeLvl"));
        SetSpeedLvl(DataManager.AccessFileDataInt("GameData/SaveData.json", datapath, "speedLvl"));
        health = maxHealth;
        activeShortcutL1R1 = DataManager.AccessFileDataBool("GameData/SaveData.json", datapath, "activeShortcutL1R1");
        hasSaved = DataManager.AccessFileDataBool("GameData/SaveData.json", datapath, "hasSaved");
        saveLevel = DataManager.AccessFileDataString("GameData/SaveData.json", datapath, "saveLevel");
    }
}