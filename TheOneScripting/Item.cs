using System;
using System.Runtime.CompilerServices;

public abstract class Item
{
    public enum ItemType
    {
        UNKNOWN = 0,
        CONSUMABLE,
        ABILITY,
        WEAPON,
        QUEST_ITEM,
    }

    public int id { get; protected set; }
    public string name { get; protected set; }
    public string description { get; protected set; }
    public ItemType type { get; protected set; }

    public abstract void ApplyEffect(IGameObject player);
}
