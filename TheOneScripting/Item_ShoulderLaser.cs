using System;

public class Item_ShoulderLaser : Item
{
    public readonly int damage = 60;
    public readonly float fireRate = 0.7f;

    public Item_ShoulderLaser()
    {
        this.id = 3;
        this.name = "ShoulderLaser";
        this.description = "Shoulder attachement which shoots automatically and pierces through enemies";
        this.type = ItemType.WEAPON;
    }

    public override void ApplyEffect(IGameObject player)
    {

    }
}
