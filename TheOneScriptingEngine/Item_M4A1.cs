using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

class Item_M4A1 : Item
{
    uint damage = 6;

    public Item_M4A1() 
    {
        this.id = 1;
        this.name = "M4A1";
        this.description = "This is the main weapon. You have found the item in a guard's corpse.";
        this.price = 1;
        this.type = ItemType.M4A1;
        this.texturePath = "";
    }

    public override void ApplyEffect(IGameObject player)
    {

    }
}

