using System;
using System.Collections.Generic;
using static Item;

public class ItemManager : MonoBehaviour
{
    Dictionary<uint, Item> itemData; // id, Item
    Dictionary<uint, uint> inventory; // id, quantity
    Dictionary<uint, Item> equippedAbilities; // slot (1 ~ 4), Item
    Dictionary<uint, Item> equippedConsumables; // slot (1 ~ 6), Item
    // have a reference to player

    public override void Start()
    {
        InitData();
    }

    public override void Update()
    {

    }

    void InitData()
    {
        // load from file all ItemData
        // load from file inventory
        // load from file equipped items
    }

    public void AddItemToInventory(uint id, uint quantity)
    {
        if (CheckItemAvailability(id))
        {
            inventory[id] += quantity;
        }
        else
        {
            inventory.Add(id, 1);
        }
    }

    public void UseItem(uint id)
    {
        if (CheckItemAvailability(id))
        {
            // Player.UseItem ??
        }
        else
        {
            Debug.Log("Can't use Item id: '" + id + "'. Not found.");
        }

    }

    public void EquipItem(uint id, uint slot)
    {
        if (CheckItemAvailability(id))
        {
            Item itemToEquip = CheckItemInItemData(id);
            if (itemToEquip == null)
            {
                switch (itemToEquip.type)
                {
                    case ItemType.CONSUMABLE:
                        if (slot > 6) AddToEquipment(slot, equippedConsumables, itemToEquip);
                        else Debug.Log("Can't equip Item id: '" + id + "'. Not a suitable slot.");

                        break;
                    case ItemType.ABILITY:
                        if (slot > 4) AddToEquipment(slot, equippedAbilities, itemToEquip);
                        else Debug.Log("Can't equip Item id: '" + id + "'. Not a suitable slot.");

                        break;
                    default:
                        Debug.Log("Can't equip Item id: '" + id + "'. Not Consumable or Ability.");

                        break;
                }
            }
            else
            {
                Debug.Log("Can't equip Item id: '" + id + "'. Not found.");
            }
        }
        else
        {
            Debug.Log("Can't equip Item id: '" + id + "'. Not found.");
        }
    }

    public void ThrowItem(uint id)
    {
        if (CheckItemAvailability(id))
        {
            inventory.Remove(id);
        }
        else
        {
            Debug.Log("Can't throw Item id: '" + id + "'. Not found.");
        }
    }

    // Private functions, to check certain things in the ItemManager
    private bool CheckItemAvailability(uint id)
    {
        return inventory.ContainsKey(id) && inventory[id] > 0;
    }

    private Item CheckItemInItemData(uint id)
    {
        Item item;
        if (itemData.TryGetValue(id, out item)) return item;

        return null;
    }

    private void AddToEquipment(uint slot, Dictionary<uint, Item> equipmentSet, Item itemToEquip)
    {
        if (equipmentSet.ContainsKey(slot))
        {
            equipmentSet[slot] = itemToEquip;
        }
        else
        {
            equipmentSet.Add(slot, itemToEquip);
        }
    }
}