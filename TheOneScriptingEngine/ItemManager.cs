using System;
using System.Collections.Generic;
using System.IO;
using static Item;

public class ItemManager : MonoBehaviour
{
    private Dictionary<uint, Item> itemData = new Dictionary<uint, Item>(); // id, Item
    public Dictionary<uint, uint> inventory = new Dictionary<uint, uint>(); // id, quantity
    public Dictionary<uint, Item> equipped = new Dictionary<uint, Item>(); // slot (1 ~ 6), Item
    public bool hasInitial = false;

    public override void Start()
    {
        //LoadData();
        itemData.Add(1, new Item_M4A1());
        itemData.Add(2, new Item_Healing(false));
    }

    private void SaveData()
    {

    }

    private void LoadData()
    {

    }

    public override void Update()
    {
        if (!hasInitial)
        {
            Debug.Log("No tiene arma xd");
            if (CheckItemInInventory(1))
            {
                Debug.Log("Ahora zí jeje");
                hasInitial = true;
            }
        }
    }

    public void AddItem(uint id, uint quantity)
    {
        if (CheckItemInItemData(id))
        {
            if (CheckItemInInventory(id))
            {
                inventory[id] += quantity;
            }
            else
            {
                inventory.Add(id, quantity);
            }
        }
        else
        {
            Debug.Log("Item id: '" + id + "' has not been found in Item Data.");
        }
    }

    public void EquipItem(uint id)
    {
        if (CheckItemInInventory(id))
        {
            if (equipped.ContainsValue(itemData[id]))
            {
                Debug.Log("Item id: '" + id + "' is already equipped.");
                return;
            }

            for (uint slot = 1; slot <= 6; slot++)
            {
                if (!equipped.ContainsKey(slot))
                {
                    equipped.Add(slot, itemData[id]);
                    Debug.Log("Item id: '" + id + "' equipped to slot " + slot + ".");
                    return;
                }
            }

            Debug.Log("No available slots to equip item id: '" + id + "'.");
        }
        else
        {
            Debug.Log("Item id: '" + id + "' has not been found in Inventory.");
        }
    }

    public void UnequipItem(uint id)
    {
        if (CheckItemInEquipped(id))
        {
            foreach (var kvp in equipped)
            {
                if (kvp.Value.id == id)
                {
                    equipped.Remove(kvp.Key);
                    Debug.Log("Item id: '" + id + "' unequipped from slot " + kvp.Key + ".");
               
                    break;
                }
            }
        }
        else
        {
            Debug.Log("Item id: '" + id + "' not found in Equipped.");
        }
    }

    // Private functions, to check certain things in the ItemManager
    private bool CheckItemInInventory(uint id)
    {
        return inventory.ContainsKey(id) && inventory[id] > 0;
    }

    private bool CheckItemInEquipped(uint id)
    {
        foreach (var item in equipped.Values)
        {
            if (item.id == id)
            {
                return true;
            }
        }
        return false;
    }

    private bool CheckItemInItemData(uint id)
    {
        Item item;
        if (itemData.TryGetValue(id, out item)) return true;

        return false;
    }
}