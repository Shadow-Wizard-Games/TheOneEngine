using System;

public class InventoryMenuManager : MonoBehaviour
{
    public ICanvas canvas;
    ItemManager itemManager;
    IGameObject iManagerGO;
    bool hasWeapon = false;

    float cooldown = 0;
    bool onCooldown = false;
    bool inventoryEnabled = false;

    int x = 2495, y = 445, width = 200, height = 90;

    public InventoryMenuManager()
    {
        canvas = new ICanvas(InternalCalls.GetGameObjectPtr());
    }

    public override void Start()
    {
        iManagerGO = IGameObject.Find("Manager");
        itemManager = iManagerGO.GetComponent<ItemManager>();
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

        if (!onCooldown)
        {
            if (Input.GetKeyboardButton(Input.KeyboardCode.I))
            {
                //attachedGameObject.source.PlayAudio(AudioManager.EventIDs.CLICK);
                canvas.ToggleEnable();
                inventoryEnabled = true;
                onCooldown = true;
            }
        }

        if (inventoryEnabled)
        {
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
}