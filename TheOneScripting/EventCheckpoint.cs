using System;
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
        if (Input.GetKeyboardButton(Input.KeyboardCode.A) || Input.GetControllerButton(Input.ControllerButtonCode.Y))
        {
            managers.gameManager.health = managers.gameManager.GetMaxHealth();
            managers.gameManager.UpdateSave();
            uiManager.OpenHudPopUpMenu(HudPopUpMenu.SaveScene, "saving progress");

            saveParticles?.Replay();
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