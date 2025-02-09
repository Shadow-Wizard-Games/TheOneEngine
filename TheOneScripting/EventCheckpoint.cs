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

    GameManager gameManager;
    UiManager menuManager;

    IParticleSystem saveParticles;

    float playerDistance;

    readonly float tpRange = 100.0f;
    bool inRange = false;

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");

        gameManager = IGameObject.Find("GameManager").GetComponent<GameManager>();

        menuManager = IGameObject.Find("UI_Manager").GetComponent<UiManager>();

        IGameObject saveParticlesGO = IGameObject.Find("SaveParticles");
        if (saveParticlesGO != null) saveParticles = saveParticlesGO.GetComponent<IParticleSystem>();

        eventType = EventType.CHECKPOINT;
    }

    public override void Update()
    {
        if (CheckEventIsPossible())
        {
            DoEvent();
        }

        if (gameManager.colliderRender) { DrawEventDebug(); }
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
        }

        return inRange;
    }

    public override bool DoEvent()
    {
        bool ret = true;

        if (Input.GetKeyboardButton(Input.KeyboardCode.E) || Input.GetControllerButton(Input.ControllerButtonCode.Y))
        {
            if (saveParticles != null) saveParticles.Replay();
            
            gameManager.UpdateSave();
            menuManager.OpenHudPopUpMenu(HudPopUpMenu.SaveScene, "saving progress");
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