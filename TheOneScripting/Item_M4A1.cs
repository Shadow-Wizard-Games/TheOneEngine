using System;

class Item_M4A1 : Item
{
    public readonly uint damage = 5;
    public readonly uint maxAmmoPerLoader = 100;
    public readonly float fireRate = 0.15f;

    // Grenade Launcher
    public readonly uint damageGrenade = 50;
    public readonly float grenadeVelocity = 250f;

    public readonly float grenadeCooldownTime = 8f;

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
