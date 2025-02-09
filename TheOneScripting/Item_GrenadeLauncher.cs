﻿using System;

class Item_GrenadeLauncher : Item
{
    public readonly uint damage = 50;
    public readonly float grenadeVelocity = 250f;

    public readonly float cooldownTime = 8.0f;

    public float grenadeExplosionRadius = 50.0f;

    public Item_GrenadeLauncher()
    {
        this.id = 2;
        this.name = "GrenadeLauncher";
        this.description = "This is an attachement for the main weapon. Throw powerful grenades which explode on impact";
        this.type = ItemType.WEAPON;
    }

    public override void ApplyEffect(IGameObject player)
    {

    }
}
