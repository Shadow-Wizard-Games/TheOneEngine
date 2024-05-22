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
        this.id = 2;
        this.name = "Ship Key";
        this.description = "This is the KeyPass to open the ship. You have found the item in an alien corpse.";
        this.price = 1;
        this.type = ItemType.QUEST_ITEM;
        this.texturePath = "";
    }

    public override void ApplyEffect(IGameObject player)
    {

    }
}
