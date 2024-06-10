﻿using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;
using static UiManager;

public class EventCheckpoint : Event
{
    IGameObject playerGO;
    UiManager uiManager;

    IParticleSystem saveParticles;

    float playerDistance;

    readonly float tpRange = 100.0f;
    bool inRange = false;

    bool brodcastMesage = true;

    bool onCooldown = false;
    float cooldown = 0.0f;

    public override void Start()
    {
        managers.Start();

        playerGO = IGameObject.Find("SK_MainCharacter");

        uiManager = IGameObject.Find("UI_Manager").GetComponent<UiManager>();

        saveParticles = attachedGameObject.FindInChildren("SaveParticles")?.GetComponent<IParticleSystem>();

        eventType = EventType.CHECKPOINT;
    }

    public override void Update()
    {
        if (onCooldown && cooldown < 1.5f)
        {
            cooldown += Time.deltaTime;
        }
        else
        {
            cooldown = 0.0f;
            onCooldown = false;
        }

        if (CheckEventIsPossible())
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

        if (brodcastMesage) { uiManager.OpenHudPopUpMenu(UiManager.HudPopUpMenu.PickUpFeedback, "Checkpoint:", "Press A"); brodcastMesage = false; }
        if (!onCooldown && (Input.GetKeyboardButton(Input.KeyboardCode.E) || Input.GetControllerButton(Input.ControllerButtonCode.A)))
        {
            managers.gameManager.health = managers.gameManager.GetMaxHealth();
            if (managers.itemManager.GetItemInInventoryAmount(4) < 2)
            {
                uint temp = 2 - (uint)managers.itemManager.GetItemInInventoryAmount(4);
                managers.itemManager.AddItem(4, temp);
            }
            managers.gameManager.UpdateSave();
            uiManager.OpenHudPopUpMenu(HudPopUpMenu.SaveScene, "saving progress");

            saveParticles?.Replay();
            onCooldown = true;
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
}