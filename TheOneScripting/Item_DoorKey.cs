using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

class Item_DoorKey : Item
{
    uint damage = 6;

    public Item_DoorKey()
    {
        this.id = 9;
        this.name = "Ship Key";
        this.description = "This is the KeyPass to open the ship. You have found the item in an alien corpse.";
        this.type = ItemType.QUEST_ITEM;
    }

    public override void ApplyEffect(IGameObject player)
    {

    }
}
