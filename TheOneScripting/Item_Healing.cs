public class Item_Healing : Item
{
    public readonly uint maxHeals = 3;
    public readonly float bandageHealingAmount = 0.6f;
    public readonly float syringeHealingAmount = 0.8f;
    public readonly float slowAmount = 0.4f;
    public bool upgraded = false;

    public readonly float activeTime = 1f;
    public readonly float cooldownTime = 4f;

    public Item_Healing()
    {
        this.id = 3;
        this.name = "Bandage";
        this.description = "A healing bandage. Upon use, you recover 60% of your Health.";
        type = ItemType.CONSUMABLE;
    }

    public override void ApplyEffect(IGameObject player)
    {
        throw new System.NotImplementedException();
    }

    public void HealUpgraded()
    {
        this.name = "Syringe";
        this.description = "A healing syringe. Upon use, you recover 80% of your Health.";
    }
}
