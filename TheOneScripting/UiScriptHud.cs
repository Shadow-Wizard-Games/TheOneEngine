using System;
using System.Collections.Generic;

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

    public enum HudStrings
    {
        CURRENCYSTRING,
        KILLSTRING,
        AMMOSTRING,
        LOADOUTSTRING,
        MAINMISSIONSTRING,
        MISSIONDESCRIPTIONSTRING
    }

    public ICanvas canvas;
    PlayerScript playerScript;

    public UiScriptHud()
    {
        canvas = new ICanvas(InternalCalls.GetGameObjectPtr());
    }

    float cooldown = 0.0f;
    bool onCooldown = false;


    float currLife = 100;
    float playerMaxLife = 100;

    int currencyAmount = 200;
    string currency;

    int killsAmount = 0;
    string kills;

    string currLoadout = "m4a1";
    int currAmmo = 20;
    int maxAmmo = 20;

    bool grenadeOnCooldown = false;
    float grenadeCooldown = 5.0f;
    float grenadeTimer = 0.0f;

    bool painlessOnCooldown = false;
    float painlessCooldown = 5.0f;
    float painlessTimer = 0.0f;

    bool flameThrowerOnCooldown = false;
    float flameThrowerCooldown = 5.0f;
    float flameThrowerTimer = 0.0f;

    bool adrenalineOnCooldown = false;
    float adrenalineCooldown = 5.0f;
    float adrenalineTimer = 0.0f;

    bool consumibleOnCooldown = false;
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
        killsAmount = 0;

        currLoadout = "m4a1";
        currAmmo = 20;
        maxAmmo = 20;

        currentMissionTitle = "escape the ship";
        currentMissionDescription = "find a gun";
    }
    public override void Update()
    {
        float dt = Time.realDeltaTime;


        onCooldown = true;

        if (onCooldown && cooldown < 0.2f)
        {
            cooldown += dt;
        }
        else
        {
            cooldown = 0.0f;
            onCooldown = false;
        }

        if (!onCooldown && (playerScript != null && canvas != null))
        {
            UpdateTimers();

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

            if (!grenadeOnCooldown && (Input.GetControllerButton(Input.ControllerButtonCode.L1) || Input.GetKeyboardButton(Input.KeyboardCode.R)))//remember to add also keyboard button
            {
                UpdateAbilityCanvas(PlayerAbility.GRENADE, ICanvas.UiState.HOVERED);

                //debug text of ammo
                currAmmo -= 1;
                UpdateString(HudStrings.AMMOSTRING);

                onCooldown = true;
            }

            if (!painlessOnCooldown && (Input.GetControllerButton(Input.ControllerButtonCode.R2) || Input.GetKeyboardButton(Input.KeyboardCode.F)))//remember to add also keyboard button
            {
                UpdateAbilityCanvas(PlayerAbility.PAINLESS, ICanvas.UiState.HOVERED);
                onCooldown = true;
            }

            if (!flameThrowerOnCooldown && (Input.GetControllerButton(Input.ControllerButtonCode.L2) || Input.GetKeyboardButton(Input.KeyboardCode.T)))//remember to add also keyboard button
            {
                UpdateAbilityCanvas(PlayerAbility.FLAMETHROWER, ICanvas.UiState.HOVERED);
                onCooldown = true;
            }

            if (!adrenalineOnCooldown && (Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.G)))//remember to add also keyboard button
            {
                UpdateAbilityCanvas(PlayerAbility.ADRENALINE, ICanvas.UiState.HOVERED);
                onCooldown = true;
            }

            if (Input.GetControllerButton(Input.ControllerButtonCode.Y) || Input.GetKeyboardButton(Input.KeyboardCode.E))//remember to add also keyboard button
            {
                UpdateAbilityCanvas(PlayerAbility.CONSUMIBLE, ICanvas.UiState.HOVERED);
                onCooldown = true;
            }
        }
    }

    //for the moment, only for canvas states porpuses
    public void UpdateAbilityCanvas(PlayerAbility ability, ICanvas.UiState state)
    {
        switch (ability)
        {
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

    void UpdateString(HudStrings type)
    {
        switch (type)
        {
            case HudStrings.CURRENCYSTRING:
                currency = currencyAmount.ToString();
                canvas.SetTextString(currency, "Text_CurrencyAmount");
                break;
            case HudStrings.KILLSTRING:
                kills = killsAmount.ToString();
                canvas.SetTextString(kills, "Text_KillsAmount");
                break;
            case HudStrings.AMMOSTRING:
                canvas.SetTextString("", "Text_AmmoAmount", currAmmo);
                canvas.SetTextString("", "Text_MaxAmmo", maxAmmo);
                break;
            case HudStrings.LOADOUTSTRING:
                canvas.SetTextString(currLoadout, "Text_CurrentLoadoutName");
                break;
            case HudStrings.MAINMISSIONSTRING:
                canvas.SetTextString(currentMissionTitle, "Text_MissionName");
                break;
            case HudStrings.MISSIONDESCRIPTIONSTRING:
                canvas.SetTextString(currentMissionDescription, "Text_MissionDescription");
                break;
        }

    }
    void UpdateTimers()
    {
        float dt = Time.realDeltaTime;
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