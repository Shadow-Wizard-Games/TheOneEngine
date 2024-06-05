using System;
using System.Runtime.CompilerServices;

public class MonoBehaviour : IComponent
{
    public IGameObject attachedGameObject;
    public MonoBehaviour() : base() { attachedGameObject = new IGameObject(containerGOptr); }

    public Managers managers = new Managers();

    public object GetClassInstance()
    {
        return this;
    }

    public virtual void Start() { }
    public virtual void Update() { }
}

public class Managers
{
    public GameManager gameManager;
    public ItemManager itemManager;
    public QuestManager questManager;

    public void Start()
    {
        gameManager = IGameObject.Find("GameManager")?.GetComponent<GameManager>();
        itemManager = IGameObject.Find("ItemManager")?.GetComponent<ItemManager>();
        questManager = IGameObject.Find("QuestManager")?.GetComponent<QuestManager>();
    }
}