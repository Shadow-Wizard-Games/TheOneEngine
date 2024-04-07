public class Item_Healing : Item
{
    uint amountHealing = 0;
    bool upgraded = false;

    public Item_Healing(bool upgraded)
    {
        this.upgraded = upgraded;
        if (upgraded)
        {
            amountHealing = 100;
        }
        else
        {
            amountHealing = 60;
        }
        type = ItemType.CONSUMABLE;
    }

    public override void ApplyEffect(IGameObject player)
    {
        throw new System.NotImplementedException();
    }
}
