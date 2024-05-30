using System;

class Item_Impaciente : Item
{
    public readonly uint damage = 10;
    public readonly uint maxAmmo = 999;
    public readonly float fireRate = 0.05f;
    public readonly float knockBackPotency = -3.0f;
    public readonly float slowAmount = 0.25f;

    public readonly float activeTime = 25.0f;
    public readonly float cooldownTime = 45.0f;

    public Item_Impaciente()
    {
        this.id = 4;
        this.name = "Impaciente";
        this.description = "This is an alien killing machine. It shoots bullets at a tremendous speed destroying everything infront of it.";
        this.type = ItemType.ABILITY;
    }

    public override void ApplyEffect(IGameObject player)
    {

    }
}