using System;

public class UiScriptInventory : MonoBehaviour
{
    public ICanvas canvas;
    ItemManager itemManager;
    IGameObject iManagerGO;
    bool hasWeapon = false;

    float cooldown = 0;
    bool onCooldown = false;

    int x = 2495, y = 445, width = 200, height = 90;

    public UiScriptInventory()
    {
        canvas = new ICanvas(InternalCalls.GetGameObjectPtr());
    }

    public override void Start()
    {
        iManagerGO = IGameObject.Find("ItemManager");
        itemManager = iManagerGO.GetComponent<ItemManager>();

        onCooldown = true;
    }
    public override void Update()
    {
        float dt = InternalCalls.GetAppDeltaTime();

        if (onCooldown && cooldown < 0.2f)
        {
            cooldown += dt;
        }
        else
        {
            cooldown = 0.0f;
            onCooldown = false;
        }

        //Input Updates
        if (!onCooldown)
        {
            
        }

        //Update UI
        if (itemManager != null)
        {
            if (itemManager.hasInitial && !hasWeapon)
            {
                canvas.ChangeSectImg("Weapon", x, y, width, height);
                hasWeapon = true;
            }
        }
    }
}