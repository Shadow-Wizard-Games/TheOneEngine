

public class Item_Shield : Item
{
    public readonly int maxKills = 2;

    public readonly float activeTime = 6f;

    public Item_Shield()
    {
        this.id = 5;
        this.name = "Shield";
        this.description = "It charges by killing enemies. Use it and you'll be invincible for " + (int)activeTime + " seconds";
        this.type = ItemType.ABILITY;
    }

    public override void ApplyEffect(IGameObject player)
    {
        throw new System.NotImplementedException();
    }
}
