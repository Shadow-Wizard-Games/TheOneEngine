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

    public bool colliderRender;
    public bool gridRender;
    public bool godMode;
    public bool extraSpeed;

    public override void Start()
    {
        resumeGame = false;
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