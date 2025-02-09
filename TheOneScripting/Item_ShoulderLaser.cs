﻿using System;

class Item_ShoulderLaser : Item
{
    public readonly uint damage = 15;
    public readonly float fireRate = 0.4f;

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
