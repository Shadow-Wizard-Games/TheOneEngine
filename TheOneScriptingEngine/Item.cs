using System;
using System.Runtime.CompilerServices;

public abstract class Item
{
    public enum ItemType
    {
        UNKNOWN = 0,
        CONSUMABLE,
        SHIELD,
        M4A1,
        GREN_LAUNCHER,
        AD_RUSH,
        FLAMETHROWER,
        QUEST_ITEM,
    }

    public int id { get; protected set; }
    public string name { get; protected set; }
    public string description { get; protected set; }
    public int price { get; protected set; }
    public ItemType type { get; protected set; }
    public string texturePath { get; protected set; }

    public abstract void ApplyEffect(IGameObject player);
}
