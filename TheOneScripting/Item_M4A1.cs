using System;

public class Item_M4A1 : Item
{
    public readonly int damage = 5;
    public readonly int maxLoaderAmmo = 20;
    public readonly float fireRate = 0.15f;

    public readonly float reloadTime = 2.0f;

    // Grenade Launcher
    public readonly int grenadeDamage = 50;
    public readonly float grenadeVelocity = 250f;

    public readonly float grenadeCooldownTime = 8.0f;

    public float grenadeExplosionRadius = 50.0f;

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
