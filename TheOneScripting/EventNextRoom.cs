﻿using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;

public class EventNextRoom : Event
{
    IGameObject playerGO;
    UiManager uiManager;

    float playerDistance;

    readonly float tpRange = 100.0f;
    bool inRange = false;

    string goName;
    float cooldown = 0;

    bool brodcastMesage = true;

    public override void Start()
    {
        managers.Start();

        playerGO = IGameObject.Find("SK_MainCharacter");
        uiManager = IGameObject.Find("UI_Manager").GetComponent<UiManager>();

        eventType = EventType.NEXTROOM;
        goName = attachedGameObject.name;
    }

    public override void Update()
    {
        if (cooldown > 0)
            cooldown -= Time.realDeltaTime;

        if (CheckEventIsPossible() && managers.gameManager.GetGameState() == GameManager.GameStates.RUNNING)
        {
            DoEvent();
        }

        if (managers.gameManager.colliderRender) { DrawEventDebug(); }
    }

    public override bool CheckEventIsPossible()
    {
        playerDistance = Vector3.Distance(playerGO.transform.Position, attachedGameObject.transform.Position);

        if (playerDistance < tpRange)
        {
            inRange = true;
        }
        else
        {
            inRange = false;
            brodcastMesage = true;
        }

        return inRange;
    }

    public override bool DoEvent()
    {
        bool ret = true;

        if (brodcastMesage) { uiManager.OpenHudPopUpMenu(UiManager.HudPopUpMenu.PickUpFeedback, "Next room:", "Press A"); brodcastMesage = false; }
        if ((Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.E)) && cooldown <= 0)
        {
            cooldown = 1;

            string sceneName = ExtractSceneName();

            if(sceneName == "L2R1" && SceneManager.GetCurrentSceneName() == "L1R5")
            {
                if (!managers.itemManager.CheckItemInInventory(9))
                {
                    uiManager.OpenHudPopUpMenu(UiManager.HudPopUpMenu.PickUpFeedback, "Error:", "Missing Key");
                    return ret;
                }
            }
            else if (sceneName == "L1R4" && SceneManager.GetCurrentSceneName() == "L1R1")
            {
                if (!managers.gameManager.activeShortcutL1R1)
                {
                    uiManager.OpenHudPopUpMenu(UiManager.HudPopUpMenu.PickUpFeedback, "Error:", "Door blocked");
                    return ret;
                }
            }
            else if (sceneName == "L2R2" && managers.questManager.IsQuestComplete(14))
            {
                playerGO.source.Play(IAudioSource.AudioEvent.STOPMUSIC);

                managers.gameManager.SaveSceneState();
                SceneManager.LoadScene("L2R5");
                return ret;
            }

            playerGO.source.Play(IAudioSource.AudioEvent.STOPMUSIC);

            managers.gameManager.SaveSceneState();
            SceneManager.LoadScene(sceneName);
        }

        return ret;
    }

    public override void DrawEventDebug()
    {
        if (!inRange)
        {
            Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 4, tpRange, new Vector3(1.0f, 0.8f, 0.0f)); //Yellow
        }
        else
        {
            Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 4, tpRange, new Vector3(0.9f, 0.0f, 0.9f)); //Purple
        }
    }

    public string ExtractSceneName()
    {
        string sceneName = "";

        string patternL = $"L(\\d+)";
        string patternR = $"R(\\d+)";

        Match matchL = Regex.Match(goName, patternL);
        Match matchR = Regex.Match(goName, patternR);

        if (matchL.Success)
        {
            sceneName += matchL.Groups[0].Value;
        }
        else { Debug.LogWarning("Could not find scene name in GO name"); }

        if (matchR.Success)
        {
            sceneName += matchR.Groups[0].Value;
        }
        else { Debug.LogWarning("Could not find scene name in GO name"); }

        return sceneName;
    }
}