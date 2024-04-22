﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
        eventType = EventType.COLLECTIBLE;
        itemManagerGO = IGameObject.Find("Manager");
        itemManager = itemManagerGO.GetComponent<ItemManager>();
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
        //Set the distance to the player
        playerDistance = Vector3.Distance(playerGO.transform.position, attachedGameObject.transform.position);
        
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
        if(itemManager != null && Input.GetKeyboardButton(Input.KeyboardCode.E))
        {
            itemManager.AddItem(1, 1); //change to corresponding item ID & QUANTITY
            attachedGameObject.Disable();

            //Degug
            Debug.LogWarning("LOOTED");
        }

        return false;
    }

    public override void DrawEventDebug()
    {
        if (!inRange)
        {
            Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, collectibleRange, new Vector3(1.0f, 0.8f, 0.0f)); //Yellow
        }
        else
        {
            Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, collectibleRange, new Vector3(0.9f, 0.0f, 0.9f)); //Purple
        }
    }
}
