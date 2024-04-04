using System;
using System.Collections.Generic;
using System.IO;
using static Item;

public class ItemManager : MonoBehaviour
{
    Dictionary<uint, Item> itemData; // id, Item
    Dictionary<uint, uint> inventory; // id, quantity
    Dictionary<uint, Item> equipped; // slot (1 ~ 4), Item
    IGameObject player;

    public override void Start()
    {
        player = IGameObject.Find("Player");
        InitData();
    }

    public override void Update()
    {

    }

    void InitData()
    {

        // Specify the path to the CSV file
        string filePath = @"C:\path\to\your\file.csv";

        try
        {
            // Check if the file exists
            if (File.Exists(filePath))
            {
                // Read all lines from the CSV file
                string[] lines = File.ReadAllLines(filePath);

                // Process each line
                foreach (string line in lines)
                {
                    // Split the line by comma
                    string[] values = line.Split(',');

                    // Output the values
                    foreach (string value in values)
                    {
                        Console.Write(value + "\t"); // Use \t for tab separation
                    }
                    Console.WriteLine(); // Move to the next line
                }
            }
            else
            {
                Console.WriteLine("File not found: " + filePath);
            }
        }
        catch (Exception e)
        {
            Console.WriteLine("An error occurred: " + e.Message);
        }
    }

    public void AddItemToInventory(uint id, uint quantity)
    {

    }

    public void UseItem(uint id)
    {


    }

    public void EquipItem(uint id, uint slot)
    {

    }

    public void ThrowItem(uint id)
    {

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

    private void SaveData()
    {

    }

    private void LoadData() { }

}