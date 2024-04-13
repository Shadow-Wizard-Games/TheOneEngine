public class Item_Healing : Item
{
    uint amountHealing = 0;
    bool upgraded = false;

    public Item_Healing(bool upgraded)
    {
        this.upgraded = upgraded;
        if (upgraded)
        {
            amountHealing = 80;
            this.description = "A healing syringe. Upon use, you recover 80% of your Health.";
            this.price = 200;

        }
        else
        {
            amountHealing = 60;
            this.description = "A healing bandage. Upon use, you recover 60% of your Health.";
            this.price = 100;
        }
        type = ItemType.CONSUMABLE;
    }

    public override void ApplyEffect(IGameObject player)
    {
        throw new System.NotImplementedException();
    }
}
