using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static AudioManager;

public class EventCollectible : Event
{
    IGameObject playerGO;
    IGameObject itemManagerGO;
    ItemManager itemManager;
    Item currentItem;
    float playerDistance;

    float collectibleRange = 100.0f;

    //debug
    bool inRange = false;

    AudioManager.EventIDs currentID = 0;    //change to correct audio ID

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
        eventType = EventType.COLLECTIBLE;
        itemManagerGO = IGameObject.Find("Manager");
        itemManager = itemManagerGO.GetComponent<ItemManager>();
        //get item script from the GO
        //currentItem = attachedGameObject.GetComponent<Item_Shield>();
    }
    
    public override void Update()
    {
        if (CheckEventIsPossible())
        {
            if (DoEvent()) //Execute Event
            {
                //Log success
                //Debug.Log($"Executing event: {eventType}");
            }
            else
            {
                //Log error
                //Debug.Log($"Event {eventType} cannot be executed.");
            }
        }
        DrawEventDebug();       
    }

    public override bool CheckEventIsPossible()
    {
        //Set the distance to the player
        playerDistance = Vector3.Distance(playerGO.transform.position, attachedGameObject.transform.position);

        if (playerDistance < collectibleRange)
        {
            inRange = true;
            Debug.Log($"pepe ");
        }
        else
        {
            inRange = false;
        }
        Debug.Log($"mamaguevo {collectibleRange} {inRange}");

        return inRange;
    }

    public override bool DoEvent()
    {
        //Add item
        if(itemManager != null && Input.GetKeyboardButton(Input.KeyboardCode.E))
        {
            itemManager.AddItem(1, 1); //change to corresponding item ID & QUANTITY
            attachedGameObject.isActive = false; //disable GO, dont know if it works
        }

        return false;
    }

    public override void DrawEventDebug()
    {
        if (!inRange)
        {
            Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, collectibleRange, new Vector3(1.0f, 0.8f, 0.0f)); //Yellow
            Debug.Log("OUTrange");
        }
        else
        {
            Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, collectibleRange, new Vector3(0.9f, 0.0f, 0.9f)); //Purple
            Debug.Log("INrange");
        }
    }
}
