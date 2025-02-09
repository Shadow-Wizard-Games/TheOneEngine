﻿using System;
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
        MISSIONDESCRIPTIONSTRING,
        CDGRENADE,
        CDFLAMETHROWER,
        CDPAINLESS,
        CDADRENALINE,
        CDCONSUMIBLE
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

    bool grenadeUnlocked = false;
    bool grenadeOnCooldown = false;
    bool grenadeHoveredOnCooldown = false;
    float grenadeCooldown = 5.0f;
    float grenadeTimer = 0.0f;

    bool painlessUnlocked = false;
    bool painlessOnCooldown = false;
    bool painlessHoveredOnCooldown = false;
    float painlessCooldown = 5.0f;
    float painlessTimer = 0.0f;

    bool flameThrowerUnlocked = false;
    bool flameThrowerOnCooldown = false;
    bool flameThrowerHoveredOnCooldown = false;
    float flameThrowerCooldown = 5.0f;
    float flameThrowerTimer = 0.0f;

    bool adrenalineUnlocked = false;
    bool adrenalineOnCooldown = false;
    bool adrenalineHoveredOnCooldown = false;
    float adrenalineCooldown = 5.0f;
    float adrenalineTimer = 0.0f;

    bool consumibleUnlocked = false;
    bool consumibleOnCooldown = false;
    bool consumibleHoveredOnCooldown = false;
    float consumibleCooldown = 5.0f;
    float consumibleTimer = 0.0f;

    string currentMissionTitle = "escape the ship";
    string currentMissionDescription = "find a gun";

    public override void Start()
    {
        playerScript = IGameObject.Find("SK_MainCharacter").GetComponent<PlayerScript>();

        UpdateAbilityCanvas(PlayerAbility.GRENADE, ICanvas.UiState.DISABLED);
        UpdateAbilityCanvas(PlayerAbility.PAINLESS, ICanvas.UiState.DISABLED);
        UpdateAbilityCanvas(PlayerAbility.ADRENALINE, ICanvas.UiState.DISABLED);
        UpdateAbilityCanvas(PlayerAbility.FLAMETHROWER, ICanvas.UiState.DISABLED);
        UpdateAbilityCanvas(PlayerAbility.CONSUMIBLE, ICanvas.UiState.DISABLED);

        grenadeUnlocked = true;
        grenadeOnCooldown = false;
        grenadeCooldown = 5.0f;

        painlessUnlocked = false;
        painlessOnCooldown = false;
        painlessCooldown = 5.0f;

        flameThrowerUnlocked = false;
        flameThrowerOnCooldown = false;
        flameThrowerCooldown = 5.0f;

        adrenalineUnlocked = false;
        adrenalineOnCooldown = false;
        adrenalineCooldown = 5.0f;

        consumibleUnlocked = true;
        consumibleOnCooldown = false;
        consumibleCooldown = 5.0f;

        canvas.PrintItemUI(grenadeUnlocked, "Button_WeaponIcon2");
        canvas.PrintItemUI(painlessUnlocked, "Button_WeaponIcon4");
        canvas.PrintItemUI(flameThrowerUnlocked, "Button_WeaponIcon3");
        canvas.PrintItemUI(adrenalineUnlocked, "Button_WeaponIcon1");
        canvas.PrintItemUI(consumibleUnlocked, "Button_ConsumibleIcon");


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

        canvas.PrintItemUI(grenadeUnlocked, "Button_WeaponIcon2");
        canvas.PrintItemUI(painlessUnlocked, "Button_WeaponIcon4");
        canvas.PrintItemUI(flameThrowerUnlocked, "Button_WeaponIcon3");
        canvas.PrintItemUI(adrenalineUnlocked, "Button_WeaponIcon1");
        canvas.PrintItemUI(consumibleUnlocked, "Button_ConsumibleIcon");

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

            if (grenadeUnlocked && !grenadeOnCooldown && (Input.GetControllerButton(Input.ControllerButtonCode.L1) || Input.GetKeyboardButton(Input.KeyboardCode.TWO)))//remember to add also keyboard button
            {
                UpdateAbilityCanvas(PlayerAbility.GRENADE, ICanvas.UiState.SELECTED);

                //debug text of ammo
                currAmmo -= 1;
                UpdateString(HudStrings.AMMOSTRING);

                onCooldown = true;
            }

            if (painlessUnlocked && !painlessOnCooldown && (Input.GetControllerButton(Input.ControllerButtonCode.R2) || Input.GetKeyboardButton(Input.KeyboardCode.FOUR)))//remember to add also keyboard button
            {
                UpdateAbilityCanvas(PlayerAbility.PAINLESS, ICanvas.UiState.SELECTED);
                onCooldown = true;
            }

            if (flameThrowerUnlocked && !flameThrowerOnCooldown && (Input.GetControllerButton(Input.ControllerButtonCode.L2) || Input.GetKeyboardButton(Input.KeyboardCode.THREE)))//remember to add also keyboard button
            {
                UpdateAbilityCanvas(PlayerAbility.FLAMETHROWER, ICanvas.UiState.SELECTED);
                onCooldown = true;
            }

            if (adrenalineUnlocked && !adrenalineOnCooldown && (Input.GetControllerButton(Input.ControllerButtonCode.X) || Input.GetKeyboardButton(Input.KeyboardCode.ONE)))//remember to add also keyboard button
            {
                UpdateAbilityCanvas(PlayerAbility.ADRENALINE, ICanvas.UiState.SELECTED);
                onCooldown = true;
            }

            if (consumibleUnlocked && !consumibleOnCooldown && Input.GetControllerButton(Input.ControllerButtonCode.Y) || Input.GetKeyboardButton(Input.KeyboardCode.Q))//remember to add also keyboard button
            {
                UpdateAbilityCanvas(PlayerAbility.CONSUMIBLE, ICanvas.UiState.SELECTED);
                onCooldown = true;
            }
        }
    }

    //for the moment, only for canvas states porpuses
    //idle for normal, ready to be clicked
    //selected when it just has been clicked
    //hovered when its on cooldown
    //disabled when its locked
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
                    canvas.PrintItemUI(true, "Text_WeaponCd2");
                }
                else if (state == ICanvas.UiState.IDLE)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponIcon2");
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponSlot2");
                    grenadeOnCooldown = false;
                    grenadeUnlocked = true;
                    canvas.PrintItemUI(false, "Text_WeaponCd2");
                }
                else if (state == ICanvas.UiState.SELECTED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponIcon2");
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponSlot2");
                    grenadeHoveredOnCooldown = true;
                    grenadeOnCooldown = true;
                    canvas.PrintItemUI(true, "Text_WeaponCd2");
                }
                else if (state == ICanvas.UiState.DISABLED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponIcon2");
                    canvas.SetUiItemState(ICanvas.UiState.SELECTED, "Button_WeaponSlot2");
                    grenadeUnlocked = false;
                    canvas.PrintItemUI(false, "Text_WeaponCd2");
                }
                break;
            case PlayerAbility.PAINLESS:
                if (state == ICanvas.UiState.HOVERED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponIcon4");
                    canvas.SetUiItemState(ICanvas.UiState.SELECTED, "Button_WeaponSlot4");
                    painlessHoveredOnCooldown = false;
                    canvas.PrintItemUI(true, "Text_WeaponCd4");
                }
                else if (state == ICanvas.UiState.IDLE)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponIcon4");
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponSlot4");
                    painlessOnCooldown = false;
                    painlessUnlocked = true;
                    canvas.PrintItemUI(false, "Text_WeaponCd4");
                }
                else if (state == ICanvas.UiState.SELECTED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponIcon4");
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponSlot4");
                    painlessHoveredOnCooldown = true;
                    painlessOnCooldown = true;
                    canvas.PrintItemUI(true, "Text_WeaponCd4");
                }
                else if (state == ICanvas.UiState.DISABLED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponIcon4");
                    canvas.SetUiItemState(ICanvas.UiState.SELECTED, "Button_WeaponSlot4");
                    painlessUnlocked = false;
                    canvas.PrintItemUI(false, "Text_WeaponCd4");
                }
                break;
            case PlayerAbility.FLAMETHROWER:
                if (state == ICanvas.UiState.HOVERED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponIcon3");
                    canvas.SetUiItemState(ICanvas.UiState.SELECTED, "Button_WeaponSlot3");
                    flameThrowerHoveredOnCooldown = false;
                    canvas.PrintItemUI(true, "Text_WeaponCd3");
                }
                else if (state == ICanvas.UiState.IDLE)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponIcon3");
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponSlot3");
                    flameThrowerOnCooldown = false;
                    flameThrowerUnlocked = true;
                    canvas.PrintItemUI(false, "Text_WeaponCd3");
                }
                else if (state == ICanvas.UiState.SELECTED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponIcon3");
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponSlot3");
                    flameThrowerHoveredOnCooldown = true;
                    flameThrowerOnCooldown = true;
                    canvas.PrintItemUI(true, "Text_WeaponCd3");
                }
                else if (state == ICanvas.UiState.DISABLED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponIcon3");
                    canvas.SetUiItemState(ICanvas.UiState.SELECTED, "Button_WeaponSlot3");
                    flameThrowerUnlocked = false;
                    canvas.PrintItemUI(false, "Text_WeaponCd3");
                }
                break;
            case PlayerAbility.ADRENALINE:
                if (state == ICanvas.UiState.HOVERED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponIcon1");
                    canvas.SetUiItemState(ICanvas.UiState.SELECTED, "Button_WeaponSlot1");
                    adrenalineHoveredOnCooldown = false;
                    canvas.PrintItemUI(true, "Text_WeaponCd1");
                }
                else if (state == ICanvas.UiState.IDLE)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponIcon1");
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponSlot1");
                    adrenalineOnCooldown = false;
                    adrenalineUnlocked = true;
                    canvas.PrintItemUI(false, "Text_WeaponCd1");
                }
                else if (state == ICanvas.UiState.SELECTED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponIcon1");
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_WeaponSlot1");
                    adrenalineHoveredOnCooldown = true;
                    adrenalineOnCooldown = true;
                    canvas.PrintItemUI(true, "Text_WeaponCd1");
                }
                else if (state == ICanvas.UiState.DISABLED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_WeaponIcon1");
                    canvas.SetUiItemState(ICanvas.UiState.SELECTED, "Button_WeaponSlot1");
                    adrenalineUnlocked = false;
                    canvas.PrintItemUI(false, "Text_WeaponCd1");
                }
                break;
            case PlayerAbility.CONSUMIBLE:
                if (state == ICanvas.UiState.HOVERED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_ConsumibleIcon");
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_ConsumibleLayout");
                    consumibleHoveredOnCooldown = false;
                    canvas.PrintItemUI(true, "Text_ConsumibleCd");
                }
                else if (state == ICanvas.UiState.IDLE)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_ConsumibleIcon");
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_ConsumibleLayout");
                    consumibleOnCooldown = false;
                    consumibleUnlocked = true;
                    canvas.PrintItemUI(false, "Text_ConsumibleCd");
                }
                else if (state == ICanvas.UiState.SELECTED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.HOVERED, "Button_ConsumibleIcon");
                    canvas.SetUiItemState(ICanvas.UiState.SELECTED, "Button_ConsumibleLayout");
                    consumibleHoveredOnCooldown = true;
                    consumibleOnCooldown = true;
                    canvas.PrintItemUI(true, "Text_ConsumibleCd");
                }
                else if (state == ICanvas.UiState.DISABLED)
                {
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_ConsumibleIcon");
                    canvas.SetUiItemState(ICanvas.UiState.IDLE, "Button_ConsumibleLayout");
                    consumibleUnlocked = false;
                    canvas.PrintItemUI(false, "Text_ConsumibleCd");
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
            case HudStrings.CDGRENADE:
                canvas.SetTextString("", "Text_WeaponCd2", (int)(grenadeCooldown - grenadeTimer));
                break;
            case HudStrings.CDFLAMETHROWER:
                canvas.SetTextString("", "Text_WeaponCd3", (int)(flameThrowerCooldown - flameThrowerTimer));
                break;
            case HudStrings.CDPAINLESS:
                canvas.SetTextString("", "Text_WeaponCd4", (int)(painlessCooldown - painlessTimer));
                break;
            case HudStrings.CDADRENALINE:
                canvas.SetTextString("", "Text_WeaponCd1", (int)(adrenalineCooldown - adrenalineTimer));
                break;
            case HudStrings.CDCONSUMIBLE:
                canvas.SetTextString("", "Text_ConsumibleCd", (int)(consumibleCooldown - consumibleTimer));
                break;
        }

    }
    void UpdateTimers(float dt)
    {
        //grenade
        if (grenadeUnlocked)
        {
            UpdateString(HudStrings.CDGRENADE);
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
        }
        //painless
        if (painlessUnlocked)
        {
            UpdateString(HudStrings.CDPAINLESS);
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
        }
        //flamethrower
        if (flameThrowerUnlocked)
        {
            UpdateString(HudStrings.CDFLAMETHROWER);
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
        }
        //adrenaline
        if (adrenalineUnlocked)
        {
            UpdateString(HudStrings.CDADRENALINE);
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
        }
        //consumible
        if (consumibleUnlocked)
        {
            UpdateString(HudStrings.CDCONSUMIBLE);
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
}