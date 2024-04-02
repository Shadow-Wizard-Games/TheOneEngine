using System;
using System.Runtime.CompilerServices;

public class Item
{
    public enum ItemType
    {
        CONSUMABLE,
        ABILITY,
        JUNK,
        // maybe add/remove things...?
    }

    public string name { get; }
    public string description { get; }
    public ItemType type { get; }

    // maybe a reference to the item icon texture?

    Item(string name, string description, ItemType type)
    {
        this.name = name;
        this.description = description;
        this.type = type;
    }

    


}
