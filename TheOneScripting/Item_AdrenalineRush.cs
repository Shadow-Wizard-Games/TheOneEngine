using System;

class Item_AdrenalineRush : Item
{
    public readonly float damageBoost = 0.5f;
    public readonly float velocityBoost = 0.5f;
    public readonly float healAmount = 0.3f;

    public readonly float healthRegenTime = 3f;
    public readonly float activeTime = 8f;
    public readonly float cooldownTime = 20f;

    public Item_AdrenalineRush()
    {
        this.id = 6;
        this.name = "AdrenalineRush";
        this.description = "Special medicine which boosts velocity and damage and heals for a short duration of time";
        this.type = ItemType.CONSUMABLE;
    }

    public override void ApplyEffect(IGameObject player)
    {

    }
}
