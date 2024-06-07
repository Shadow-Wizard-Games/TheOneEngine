using System;

public class Item_FlameThrower : Item
{
    public readonly int damage = 9;
    public readonly float slowAmountEnemy = 0.3f;

    public readonly float activeTime = 10f;
    public readonly float cooldownTime = 20f;

    public Item_FlameThrower()
    {
        this.id = 8;
        this.name = "Flamethrower";
        this.description = "Carbonize all enemies that stand in front of you";
        this.type = ItemType.ABILITY;
    }

    public override void ApplyEffect(IGameObject player)
    {

    }
}
