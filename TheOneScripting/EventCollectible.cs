using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class EventCollectible : Event
{
    IGameObject playerGO;
    IGameObject itemManagerGO;
    ItemManager itemManager;
    UiManager uiManager;
    float playerDistance;

    GameManager gameManager;

    readonly float collectibleRange = 100.0f;

    //debug
    bool inRange = false;

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
        eventType = EventType.COLLECTIBLE;
        itemManagerGO = IGameObject.Find("ItemManager");
        itemManager = itemManagerGO.GetComponent<ItemManager>();

        gameManager = IGameObject.Find("GameManager").GetComponent<GameManager>();
        uiManager = IGameObject.Find("UI_Manager").GetComponent<UiManager>();
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
        //Set the distance to the player
        playerDistance = Vector3.Distance(playerGO.transform.Position, attachedGameObject.transform.Position);

        if (playerDistance < collectibleRange)
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
        //Add item
        if (itemManager != null)
        {
            if (Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.E))
            {
                itemManager.AddItem(1, 1); //change to corresponding item ID & QUANTITY
                attachedGameObject.Disable();
                uiManager.OpenHudPopUpMenu(UiManager.HudPopUpMenu.PickUpFeedback, "m4a1");
                //Degug
                Debug.LogWarning("LOOTED");
            }
        }

        return false;
    }

    public override void DrawEventDebug()
    {
        if (!inRange)
        {
            Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 4, collectibleRange, new Vector3(1.0f, 0.8f, 0.0f)); //Yellow
        }
        else
        {
            Debug.DrawWireCircle(attachedGameObject.transform.Position + Vector3.up * 4, collectibleRange, new Vector3(0.9f, 0.0f, 0.9f)); //Purple
        }
    }
}
