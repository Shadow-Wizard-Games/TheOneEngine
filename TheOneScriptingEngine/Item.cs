using System;
using System.Runtime.CompilerServices;

public abstract class Item
{
    public enum ItemType
    {
        UNKNOWN = 0,
        CONSUMABLE,
        SHIELD,
        GREN_LAUNCHER,
        AD_RUSH,
        FLAMETHROWER,
        QUEST_ITEM,
    }

    public int id { get; }
    public string name { get; }
    public string description { get; }
    public int price { get; }
    public ItemType type { get; protected set; }
    public string texturePath { get; }

    public abstract void ApplyEffect(IGameObject player);
}
