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

    int killsAmount = 0;

    string currLoadout = "m4a1";
    int currAmmo = 20;
    int maxAmmo = 20;

    bool grenadeOnCooldown = false;
    bool grenadeHoveredOnCooldown = false;
    float grenadeCooldown = 5.0f;
    float grenadeTimer = 0.0f;

    bool painlessOnCooldown = false;
    bool painlessHoveredOnCooldown = false;
    float painlessCooldown = 5.0f;
    float painlessTimer = 0.0f;

    bool flameThrowerOnCooldown = false;
    bool flameThrowerHoveredOnCooldown = false;
    float flameThrowerCooldown = 5.0f;
    float flameThrowerTimer = 0.0f;

    bool adrenalineOnCooldown = false;
    bool adrenalineHoveredOnCooldown = false;
    float adrenalineCooldown = 5.0f;
    float adrenalineTimer = 0.0f;

    bool consumibleOnCooldown = false;
    bool consumibleHoveredOnCooldown = false;
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

        UpdateTimers(dt);

        if (!onCooldown && (playerScript != null && canvas != null))
        {

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
                UpdateAbilityCanvas(PlayerAbility.GRENADE, ICanvas.UiState.SELECTED);

                //debug text of ammo
                currAmmo -= 1;
                UpdateString(HudStrings.AMMOSTRING);

                onCooldown = true;
            }

            if (!painlessOnCooldown && (Input.GetControllerButton(Input.ControllerButtonCode.R2) || Input.GetKeyboardButton(Input.KeyboardCode.F)))//remember to add also keyboard button
            {
                UpdateAbilityCanvas(PlayerAbility.PAINLESS, ICanvas.UiState.SELECTED);
                onCooldown = true;
            }

            if (!flameThrowerOnCooldown && (Input.GetControllerButton(Input.ControllerButtonCode.L2) || Input.GetKeyboardButton(Input.KeyboardCode.T)))//remember to add also keyboard button
            {
                UpdateAbilityCanvas(PlayerAbility.FLAMETHROWER, ICanvas.UiState.SELECTED);
                onCooldown = true;
            }

            if (!adrenalineOnCooldown && (Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.G)))//remember to add also keyboard button
            {
                UpdateAbilityCanvas(PlayerAbility.ADRENALINE, ICanvas.UiState.SELECTED);
                onCooldown = true;
            }

            if (!consumibleOnCooldown && Input.GetControllerButton(Input.ControllerButtonCode.Y) || Input.GetKeyboardButton(Input.KeyboardCode.E))//remember to add also keyboard button
            {
                UpdateAbilityCanvas(PlayerAbility.CONSUMIBLE, ICanvas.UiState.SELECTED);
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
                if (state == ICanvas.UiState.HOVERED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponIcon2");
                    canvas.SetUiItemState(ICanvas.UiState.SELECTED, "Button_WeaponSlot2");
                    grenadeHoveredOnCooldown = false;
                }
                else if (state == ICanvas.UiState.IDLE)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponIcon2");
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponSlot2");
                    grenadeOnCooldown = false;
                }
                else if (state == ICanvas.UiState.SELECTED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponIcon2");
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponSlot2");
                    grenadeHoveredOnCooldown = true;
                    grenadeOnCooldown = true;
                }
                break;
            case PlayerAbility.PAINLESS:
                if (state == ICanvas.UiState.HOVERED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponIcon4");
                    canvas.SetUiItemState(ICanvas.UiState.SELECTED, "Button_WeaponSlot4");
                    painlessHoveredOnCooldown = false;
                }
                else if (state == ICanvas.UiState.IDLE)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponIcon4");
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponSlot4");
                    painlessOnCooldown = false;
                }
                else if (state == ICanvas.UiState.SELECTED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponIcon4");
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponSlot4");
                    painlessHoveredOnCooldown = true;
                    painlessOnCooldown = true;
                }
                break;
            case PlayerAbility.FLAMETHROWER:
                if (state == ICanvas.UiState.HOVERED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponIcon3");
                    canvas.SetUiItemState(ICanvas.UiState.SELECTED, "Button_WeaponSlot3");
                    flameThrowerHoveredOnCooldown = false;
                }
                else if (state == ICanvas.UiState.IDLE)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponIcon3");
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponSlot3");
                    flameThrowerOnCooldown = false;
                }
                else if (state == ICanvas.UiState.SELECTED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponIcon3");
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponSlot3");
                    flameThrowerHoveredOnCooldown = true;
                    flameThrowerOnCooldown = true;
                }
                break;
            case PlayerAbility.ADRENALINE:
                if (state == ICanvas.UiState.HOVERED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponIcon1");
                    canvas.SetUiItemState(ICanvas.UiState.SELECTED, "Button_WeaponSlot1");
                    adrenalineHoveredOnCooldown = false;
                }
                else if (state == ICanvas.UiState.IDLE)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponIcon1");
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponSlot1");
                    adrenalineOnCooldown = false;
                }
                else if (state == ICanvas.UiState.SELECTED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponIcon1");
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponSlot1");
                    adrenalineHoveredOnCooldown = true;
                    adrenalineOnCooldown = true;
                }
                break;
            case PlayerAbility.CONSUMIBLE:
                if (state == ICanvas.UiState.HOVERED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_ConsumibleIcon");
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_ConsumibleLayout");
                    consumibleHoveredOnCooldown = false;
                }
                else if (state == ICanvas.UiState.IDLE)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_ConsumibleIcon");
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_ConsumibleLayout");
                    consumibleOnCooldown = false;
                }
                else if (state == ICanvas.UiState.SELECTED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_ConsumibleIcon");
                    canvas.SetUiItemState(ICanvas.UiState.SELECTED, "Button_ConsumibleLayout");
                    consumibleHoveredOnCooldown = true;
                    consumibleOnCooldown = true;
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
                canvas.SetTextString("", "Text_CurrencyAmount", currencyAmount);
                break;
            case HudStrings.KILLSTRING:
                canvas.SetTextString("", "Text_KillsAmount", killsAmount);
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
    void UpdateTimers(float dt)
    {
        //grenade
        if (grenadeHoveredOnCooldown && grenadeTimer >= 0.5f)
        {
            UpdateAbilityCanvas(PlayerAbility.GRENADE, ICanvas.UiState.HOVERED);
        }

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
        if (painlessHoveredOnCooldown && painlessTimer >= 0.5f)
        {
            UpdateAbilityCanvas(PlayerAbility.PAINLESS, ICanvas.UiState.HOVERED);
        }

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
        if (flameThrowerHoveredOnCooldown && flameThrowerTimer >= 0.5f)
        {
            UpdateAbilityCanvas(PlayerAbility.FLAMETHROWER, ICanvas.UiState.HOVERED);
        }

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
        if (adrenalineHoveredOnCooldown && adrenalineTimer >= 0.5f)
        {
            UpdateAbilityCanvas(PlayerAbility.ADRENALINE, ICanvas.UiState.HOVERED);
        }

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
        if (consumibleHoveredOnCooldown && consumibleTimer >= 0.5f)
        {
            UpdateAbilityCanvas(PlayerAbility.CONSUMIBLE, ICanvas.UiState.HOVERED);
        }

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