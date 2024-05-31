using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;

public class EventNextRoom : Event
{
    IGameObject playerGO;

    GameManager gameManager;
    ItemManager itemManager;
    UiManager uiManager;

    float playerDistance;

    readonly float tpRange = 100.0f;
    bool inRange = false;

    string goName;

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");

        gameManager = IGameObject.Find("GameManager").GetComponent<GameManager>();
        itemManager = IGameObject.Find("ItemManager").GetComponent<ItemManager>();
        uiManager = IGameObject.Find("UI_Manager").GetComponent<UiManager>();

        eventType = EventType.NEXTROOM;
        goName = attachedGameObject.name;
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

        if (Input.GetControllerButton(Input.ControllerButtonCode.Y) || Input.GetKeyboardButton(Input.KeyboardCode.E))
        {
            string sceneName = ExtractSceneName();

            if(sceneName == "L1R5")
            {
                if (!itemManager.CheckItemInInventory(2))
                {
                    uiManager.OpenHudPopUpMenu(UiManager.HudPopUpMenu.PickUpFeedback, "Error:", "Missing Key");
                    return ret;
                }
            }

            playerGO.source.Play(IAudioSource.AudioEvent.STOPMUSIC);

            gameManager.SaveSceneState();
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