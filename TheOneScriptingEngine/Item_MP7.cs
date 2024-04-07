using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

class Item_MP7 : Item
{
    uint damage = 1;

    public Item_MP7() 
    {
        this.id = 1;
        this.name = "MP7";
        this.description = "This is the main weapon. You have found the item in a guard's corpse.";
        this.price = 1;
        this.type = ItemType.MP7;
        this.texturePath = "";
    }

    public override void ApplyEffect(IGameObject player)
    {

    }
}

