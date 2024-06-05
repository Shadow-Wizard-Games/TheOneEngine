using System.Collections.Generic;
using System.Text.RegularExpressions;

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
            case 0:
                damage = 0;
                break;
            case 1:
                damage = 5;
                break;
            case 2:
                damage = 10;
                break;
            case 3:
                damage = 15;
                break;
        }
    }

    public void SetSpeedLvl(int speedLvl)
    {
        this.speedLvl = speedLvl;

        switch (speedLvl)
        {
            case 0:
                speed = 80;
                break;
            case 1:
                speed = 90;
                break;
            case 2:
                speed = 100;
                break;
            case 3:
                speed = 110;
                break;
        }
    }

    public void SetLifeLvl(int lifeLvl)
    {
        this.lifeLvl = lifeLvl;

        switch (lifeLvl)
        {
            case 0:
                maxHealth = 100;
                break;
            case 1:
                maxHealth = 120;
                break;
            case 2:
                maxHealth = 140;
                break;
            case 3:
                maxHealth = 160;
                break;
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

        ResetPlayerData();
    }

    public override void Update()
    {
        ShortcutsConditions();
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
        managers.itemManager.ResetInventory();
        managers.questManager.ResetQuests();
        ResetPlayerData();
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

    public GameStates GetGameState()
    {
        return state;
    }
}