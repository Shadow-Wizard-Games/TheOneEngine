using System;

public class UiScriptHud : MonoBehaviour
{
    public enum PlayerAbility
    {
        GRENADE,
        PAINLESS,
        FLAMETHROWER,
        ADRENALINE,
        CONSUMIBLE
    }

    public ICanvas canvas;
    PlayerScript playerScript;

    public UiScriptHud()
    {
        canvas = new ICanvas(InternalCalls.GetGameObjectPtr());
    }

    float currLife = 100;
    float playerMaxLife = 100;

    int currencyAmount = 200;
    int kilsAmount = 0;

    string currLoadout = "m41a";
    float currAmmo = 20;
    float maxAmmo = 20;

    bool  grenadeOnCooldown = false;
    float grenadeCooldown = 5.0f;
    float grenadeTimer = 0.0f;

    bool  painlessOnCooldown = false;
    float painlessCooldown = 5.0f;
    float painlessTimer = 0.0f;

    bool  flameThrowerOnCooldown = false;
    float flameThrowerCooldown = 5.0f;
    float flameThrowerTimer = 0.0f;

    bool  adrenalineOnCooldown = false;
    float adrenalineCooldown = 5.0f;
    float adrenalineTimer = 0.0f;

    bool  consumibleOnCooldown = false;
    float consumibleCooldown = 5.0f;
    float consumibleTimer = 0.0f;

    string currentMissionTitle = "escape the ship";
    string currentMissionDescription = "find a gun";

    public override void Start()
    {
        playerScript = IGameObject.Find("SK_MainCharacter").GetComponent<PlayerScript>();

        grenadeOnCooldown = false;
        grenadeCooldown = 5.0f;

        painlessOnCooldown = false;
        painlessCooldown = 5.0f;

        flameThrowerOnCooldown = false;
        flameThrowerCooldown = 5.0f;

        adrenalineOnCooldown = false;
        adrenalineCooldown = 5.0f;

        consumibleOnCooldown = false;
        consumibleCooldown = 5.0f;


        //setting player info
        currLife = 100;
        playerMaxLife = 100;

        currencyAmount = 200;
        kilsAmount = 0;

        currLoadout = "m41a";
        currAmmo = 20;
        maxAmmo = 20;

        currentMissionTitle = "escape the ship";
        currentMissionDescription = "find a gun";
    }
    public override void Update()
    {
        if (playerScript != null && canvas != null)
        {
            UpdateTimers();

            //setting texts
            canvas.SetTextString(currAmmo.ToString() + " / " + maxAmmo.ToString(), "Text_AmmoAmount");
            canvas.SetTextString(currLoadout, "Text_CurrentLoadoutName");
            canvas.SetTextString(currentMissionTitle, "Text_MissionName");
            canvas.SetTextString(currentMissionDescription, "Text_MissionDescription");
            canvas.SetTextString(currencyAmount.ToString(), "Text_CurrencyAmount");
            canvas.SetTextString(kilsAmount.ToString(), "Text_KillsAmount");



            currLife = playerScript.CurrentLife();
            int sliderMax = canvas.GetSliderMaxValue("Slider_HP");
            float life = (float)((sliderMax * ((int)currLife)) / playerMaxLife);
            
            if ((life != canvas.GetSliderValue("Slider_HP")) && (canvas.GetSliderValue("Slider_HP") != -1))
            {
                canvas.SetSliderValue((int)life, "Slider_HP");
            }

            //implement something like this line
            //currAmmo = playerScript.GetCurrentAmmo();

            if ((currAmmo != canvas.GetSliderValue("Slider_Ammo")) && (canvas.GetSliderValue("Slider_Ammo") != -1))
            {
                canvas.SetSliderValue((int)((canvas.GetSliderMaxValue("Slider_Ammo") * currAmmo) / maxAmmo), "Slider_Ammo");
            }

            if (Input.GetControllerButton(Input.ControllerButtonCode.L1) || Input.GetKeyboardButton(Input.KeyboardCode.R))//remember to add also keyboard button
            {
                UpdateAbilityCanvas(PlayerAbility.GRENADE, ICanvas.UiState.HOVERED);
            }

            if (Input.GetControllerButton(Input.ControllerButtonCode.R2) || Input.GetKeyboardButton(Input.KeyboardCode.F))//remember to add also keyboard button
            {
                UpdateAbilityCanvas(PlayerAbility.PAINLESS, ICanvas.UiState.HOVERED);
            }

            if (Input.GetControllerButton(Input.ControllerButtonCode.L2) || Input.GetKeyboardButton(Input.KeyboardCode.T))//remember to add also keyboard button
            {
                UpdateAbilityCanvas(PlayerAbility.FLAMETHROWER, ICanvas.UiState.HOVERED);
            }

            if (Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.G))//remember to add also keyboard button
            {
                UpdateAbilityCanvas(PlayerAbility.ADRENALINE, ICanvas.UiState.HOVERED);
            }

            if (Input.GetControllerButton(Input.ControllerButtonCode.Y) || Input.GetKeyboardButton(Input.KeyboardCode.E))//remember to add also keyboard button
            {
                UpdateAbilityCanvas(PlayerAbility.CONSUMIBLE, ICanvas.UiState.HOVERED);
            }
        }
    }

    //for the moment, only for canvas states porpuses
    public void UpdateAbilityCanvas(PlayerAbility ability, ICanvas.UiState state)
    {
        switch (ability) {
            case PlayerAbility.GRENADE:
                if (!grenadeOnCooldown && state == ICanvas.UiState.HOVERED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponIcon2");
                    canvas.SetUiItemState(ICanvas.UiState.SELECTED, "Button_WeaponSlot2");
                    grenadeOnCooldown = true;
                }
                else if (state == ICanvas.UiState.IDLE)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponIcon2");
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponSlot2");
                    grenadeOnCooldown = false;
                }
                break;
            case PlayerAbility.PAINLESS:
                if (!painlessOnCooldown && state == ICanvas.UiState.HOVERED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponIcon4");
                    canvas.SetUiItemState(ICanvas.UiState.SELECTED, "Button_WeaponSlot4");
                    painlessOnCooldown = true;
                }
                else if (state == ICanvas.UiState.IDLE)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponIcon4");
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponSlot4");
                    painlessOnCooldown = false;
                }
                break;
            case PlayerAbility.FLAMETHROWER:
                if (!flameThrowerOnCooldown && state == ICanvas.UiState.HOVERED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponIcon3");
                    canvas.SetUiItemState(ICanvas.UiState.SELECTED, "Button_WeaponSlot3");
                    flameThrowerOnCooldown = true;
                }
                else if (state == ICanvas.UiState.IDLE)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponIcon3");
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponSlot3");
                    flameThrowerOnCooldown = false;
                }
                break;
            case PlayerAbility.ADRENALINE:
                if (!adrenalineOnCooldown && state == ICanvas.UiState.HOVERED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponIcon1");
                    canvas.SetUiItemState(ICanvas.UiState.SELECTED, "Button_WeaponSlot1");
                    adrenalineOnCooldown = true;
                }
                else if (state == ICanvas.UiState.IDLE)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponIcon1");
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponSlot1");
                    adrenalineOnCooldown = false;
                }
                break;
            case PlayerAbility.CONSUMIBLE:
                if (!consumibleOnCooldown && state == ICanvas.UiState.HOVERED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_ConsumibleIcon");
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_ConsumibleLayout");
                    consumibleOnCooldown = true;
                }
                else if (state == ICanvas.UiState.IDLE)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_ConsumibleIcon");
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_ConsumibleLayout");
                    consumibleOnCooldown = false;
                }
                break;
            default:
                break;
        }
    }

    void UpdateTimers()
    {
        float dt = InternalCalls.GetAppDeltaTime();
        //grenade
        if (grenadeOnCooldown && grenadeTimer < grenadeCooldown)
        {
            grenadeTimer += dt;
        }
        else
        {
            grenadeTimer = 0.0f;
            UpdateAbilityCanvas(PlayerAbility.GRENADE, ICanvas.UiState.IDLE);
        }
        //painless
        if (painlessOnCooldown && painlessTimer < painlessCooldown)
        {
            painlessTimer += dt;
        }
        else
        {
            painlessTimer = 0.0f;
            UpdateAbilityCanvas(PlayerAbility.PAINLESS, ICanvas.UiState.IDLE);
        }
        //flamethrower
        if (flameThrowerOnCooldown && flameThrowerTimer < flameThrowerCooldown)
        {
            flameThrowerTimer += dt;
        }
        else
        {
            flameThrowerTimer = 0.0f;
            UpdateAbilityCanvas(PlayerAbility.FLAMETHROWER, ICanvas.UiState.IDLE);
        }
        //adrenaline
        if (adrenalineOnCooldown && adrenalineTimer < adrenalineCooldown)
        {
            adrenalineTimer += dt;
        }
        else
        {
            adrenalineTimer = 0.0f;
            UpdateAbilityCanvas(PlayerAbility.ADRENALINE, ICanvas.UiState.IDLE);
        }
        //consumible
        if (consumibleOnCooldown && consumibleTimer < consumibleCooldown)
        {
            consumibleTimer += dt;
        }
        else
        {
            consumibleTimer = 0.0f;
            UpdateAbilityCanvas(PlayerAbility.CONSUMIBLE, ICanvas.UiState.IDLE);
        }
    }
}