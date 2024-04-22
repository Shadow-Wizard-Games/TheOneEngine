﻿using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;

using static AudioManager;

public class EventNextRoom : Event
{
    IGameObject playerGO;
    PlayerScript player;

    float playerDistance;

    float tpRange = 100.0f;
    bool inRange = false;

    string goName;

    AudioManager.EventIDs currentID = 0;    //change to correct audio ID

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        eventType = EventType.NEXTROOM;
        goName = attachedGameObject.name;
    }

    public override void Update()
    {
        if (CheckEventIsPossible())
        {
            DoEvent();
        }
        DrawEventDebug();
    }

    public override bool CheckEventIsPossible()
    {
        playerDistance = Vector3.Distance(playerGO.transform.position, attachedGameObject.transform.position);

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

        if(Input.GetKeyboardButton(Input.KeyboardCode.E))
        {
            string sceneName = ExtractSceneName();

            Debug.LogWarning(sceneName);
            Debug.LogWarning(goName);

            if (player.currentID == AudioManager.EventIDs.A_COMBAT_1)
            {
                playerGO.source.StopAudio(AudioManager.EventIDs.A_COMBAT_1);
            }
            if (player.currentID == AudioManager.EventIDs.A_AMBIENT_1)
            {
                playerGO.source.StopAudio(AudioManager.EventIDs.A_AMBIENT_1);
            }

            SceneManager.LoadScene(sceneName);
        }

        return ret;
    }

    public override void DrawEventDebug()
    {
        if (!inRange)
        {
            Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, tpRange, new Vector3(1.0f, 0.8f, 0.0f)); //Yellow
        }
        else
        {
            Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, tpRange, new Vector3(0.9f, 0.0f, 0.9f)); //Purple
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