using System;

public class Item_GrenadeLauncher : Item
{
    public readonly int damage = 100;
    public readonly float grenadeVelocity = 200f;

    public readonly float cooldownTime = 8.0f;

    public float grenadeExplosionRadius = 42.0f;

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
