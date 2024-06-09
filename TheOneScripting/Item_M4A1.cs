using System;

public class Item_M4A1 : Item
{
    public readonly int damage = 15;
    public readonly int maxLoaderAmmo = 32;
    public readonly float fireRate = 0.15f;

    public readonly float reloadTime = 0.8f;

    public Item_M4A1()
    {
        this.id = 1;
        this.name = "M4A1";
        this.description = "This is the main weapon. You have found the item in a guard's corpse.";
        this.type = ItemType.WEAPON;
    }

    public override void ApplyEffect(IGameObject player)
    {

    }
}
