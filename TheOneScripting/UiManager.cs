using System;

public class UiManager : MonoBehaviour
{
    public enum MenuState
    {
        Hud,
        Inventory,
        Death,
        Pause,
        Debug,
        Settings,
        Missions,
        Stats,
    }

    public enum HudPopUpMenu
    {
        SaveScene = 0,
        PickUpFeedback,
        Dialogue
    }

    public enum Dialoguer
    {
        ShopKeeper = 0,
        Medic,
        CampLeader,
        Sargeant
    }

    IGameObject inventoryGo;
    IGameObject deathScreenGo;
    IGameObject pauseMenuGo;
    IGameObject hudGo;
    IGameObject debugGo;
    IGameObject settingsGo;
    IGameObject settingsControlsGo;
    IGameObject settingsDisplayGo;
    IGameObject missionsGo;
    IGameObject dialogueGo;
    IGameObject statsGo;
    IGameObject savingSceneGo;
    IGameObject pickUpFeedbackGo;

    UiScriptSettings settingsCanvas;

    IGameObject playerGO;
    PlayerScript playerScript;

    IGameObject GameManagerGO;
    GameManager gameManager;

    ICanvas savingCanvas;
    ICanvas dialogCanvas;
    ICanvas pickupCanvas;

    MenuState state;
    MenuState previousState;

    public MenuState GetMenuState() { return this.state; }

    float cooldown = 0;
    bool onCooldown = false;

    float saveCooldown = 0;
    bool saveOnCooldown = false;
    float changeSaveTextCooldown = 0;

    float pickUpFeedbackCooldown = 0;
    bool pickUpFeedbackOnCooldown = false;

    float dialogueCooldown = 0;
    bool dialogueOnCooldown = false;

    public override void Start()
    {
        inventoryGo = IGameObject.Find("Canvas_Inventory");
        deathScreenGo = IGameObject.Find("Canvas_Death");
        pauseMenuGo = IGameObject.Find("Canvas_PauseMenu");
        hudGo = IGameObject.Find("Canvas_Hud");
        debugGo = IGameObject.Find("Canvas_Debug");
        settingsGo = IGameObject.Find("Canvas_Settings");
        settingsControlsGo = IGameObject.Find("Canvas_SettingsControls");
        settingsDisplayGo = IGameObject.Find("Canvas_SettingsDisplay");
        missionsGo = IGameObject.Find("Canvas_Missions");
        dialogueGo = IGameObject.Find("Canvas_Dialogue");
        statsGo = IGameObject.Find("Canvas_Stats");
        savingSceneGo = IGameObject.Find("Canvas_SavingScene");
        pickUpFeedbackGo = IGameObject.Find("Canvas_PickUpFeedback");

        playerGO = IGameObject.Find("SK_MainCharacter");
        playerScript = playerGO.GetComponent<PlayerScript>();

        hudGo.Enable();

        inventoryGo.Disable();
        deathScreenGo.Disable();
        pauseMenuGo.Disable();
        debugGo.Disable();
        settingsGo.Disable();
        settingsControlsGo.Disable();
        settingsDisplayGo.Disable();
        missionsGo.Disable();
        dialogueGo.Disable();
        statsGo.Disable();
        savingSceneGo.Disable();
        pickUpFeedbackGo.Disable();

        state = MenuState.Hud;
        playerScript.onPause = false;

        GameManagerGO = IGameObject.Find("GameManager");
        gameManager = GameManagerGO.GetComponent<GameManager>();
        savingCanvas = savingSceneGo.GetComponent<ICanvas>();
        dialogCanvas = dialogueGo.GetComponent<ICanvas>();
        pickupCanvas = pickUpFeedbackGo.GetComponent<ICanvas>();

        settingsCanvas = settingsGo.GetComponent<UiScriptSettings>();
    }

    public override void Update()
    {
        float dt = Time.realDeltaTime;

        if (onCooldown && cooldown < 0.2f)
        {
            cooldown += dt;
        }
        else
        {
            cooldown = 0.0f;
            onCooldown = false;
        }

        if (saveOnCooldown && saveCooldown < 4.5f)
        {
            saveCooldown += dt;
            changeSaveTextCooldown += dt;

            if (changeSaveTextCooldown > 2.0f)
            {
                savingCanvas.SetTextString("saving progress", "Text_SavingProgress");
                changeSaveTextCooldown = 0.0f;
            }
            else if (changeSaveTextCooldown > 1.5f)
            {
                savingCanvas.SetTextString("saving progress...", "Text_SavingProgress");
            }
            else if (changeSaveTextCooldown > 1.0f)
            {
                savingCanvas.SetTextString("saving progress..", "Text_SavingProgress");
            }
            else if (changeSaveTextCooldown > 0.5f)
            {
                savingCanvas.SetTextString("saving progress.", "Text_SavingProgress");
            }

            if (state != MenuState.Hud)
            {
                saveOnCooldown = false;
                saveCooldown = 0.0f;
                savingSceneGo.Disable();
                changeSaveTextCooldown = 0.0f;
            }
        }
        else if (saveOnCooldown && saveCooldown >= 4.5f)
        {
            savingSceneGo.Disable();
            saveOnCooldown = false;
        }
        else
        {
            saveCooldown = 0.0f;
            changeSaveTextCooldown = 0.0f;
        }

        if (pickUpFeedbackOnCooldown && pickUpFeedbackCooldown < 4.5f)
        {
            pickUpFeedbackCooldown += dt;
            if (state != MenuState.Hud)
            {
                pickUpFeedbackOnCooldown = false;
                pickUpFeedbackCooldown = 0.0f;
                pickUpFeedbackGo.Disable();
            }
        }
        else if (pickUpFeedbackOnCooldown && pickUpFeedbackCooldown >= 4.5f)
        {
            pickUpFeedbackGo.Disable();
            pickUpFeedbackOnCooldown = false;
        }
        else
        {
            pickUpFeedbackCooldown = 0.0f;
        }

        if (dialogueOnCooldown && dialogueCooldown < 4.5f)
        {
            dialogueCooldown += dt;
            if (state != MenuState.Hud)
            {
                dialogueOnCooldown = false;
                dialogueCooldown = 0.0f;
                dialogueGo.Disable();
            }
        }
        else if (dialogueOnCooldown && dialogueCooldown >= 4.5f)
        {
            dialogueGo.Disable();
            dialogueOnCooldown = false;
        }
        else
        {
            dialogueCooldown = 0.0f;
        }

        //previousState = state;

        if (!onCooldown)
        {
            if (state == MenuState.Death)
            {
                if (Input.GetKeyboardButton(Input.KeyboardCode.RETURN))
                {
                    playerGO.source.Play(IAudioSource.AudioEvent.STOPMUSIC);

                    SceneManager.LoadScene("MainMenu");
                }
            }
            else
            {
                if (Input.GetKeyboardButton(Input.KeyboardCode.I))
                {
                    if (previousState == MenuState.Inventory)
                    {
                        ResumeGame();
                    }
                    else
                    {
                        OpenMenu(MenuState.Inventory);
                    }
                    onCooldown = true;
                }
                else if (Input.GetKeyboardButton(Input.KeyboardCode.K))
                {
                    if (previousState == MenuState.Debug)
                    {
                        ResumeGame();
                    }
                    else
                    {
                        OpenMenu(MenuState.Debug);
                    }
                    onCooldown = true;
                }
                else if (Input.GetKeyboardButton(Input.KeyboardCode.ESCAPE))
                {
                    attachedGameObject.source.Play(IAudioSource.AudioEvent.UI_PAUSEGAME);
                    if (!settingsCanvas.editing)
                    {
                        if (state == MenuState.Pause)
                        {
                            ResumeGame();
                        }
                        else
                        {
                            OpenMenu(previousState);
                            previousState = MenuState.Pause;
                        }
                    }
                    onCooldown = true;
                }
                else if (Input.GetKeyboardButton(Input.KeyboardCode.F1))//for the moment for debug porpuses
                {
                    OpenHudPopUpMenu(HudPopUpMenu.SaveScene, "saving progress");
                }
                else if (Input.GetKeyboardButton(Input.KeyboardCode.F2))//for the moment for debug porpuses
                {
                    OpenHudPopUpMenu(HudPopUpMenu.PickUpFeedback, "shoulder laser");
                }
                else if (Input.GetKeyboardButton(Input.KeyboardCode.F3))//for the moment for debug porpuses
                {
                    OpenHudPopUpMenu(HudPopUpMenu.Dialogue, "Tu madre tiene una polla\n que ya la quisiera yo, me \ndio pena por tu padre el\n dia que se entero", Dialoguer.Sargeant);
                }
            }

            if (playerScript.isDead && previousState != MenuState.Death)
            {
                deathScreenGo.Enable();
                state = MenuState.Death;
                playerScript.onPause = true;
                onCooldown = true;
            }
        }
    }

    public void ResumeGame()
    {
        this.previousState = this.state;
        switch (state)
        {
            case MenuState.Hud:
                break;
            case MenuState.Inventory:
                inventoryGo.Disable();
                hudGo.Enable();
                state = MenuState.Hud;
                playerScript.onPause = false;
                break;
            case MenuState.Pause:
                pauseMenuGo.Disable();
                hudGo.Enable();
                state = MenuState.Hud;
                playerScript.onPause = false;
                break;
            case MenuState.Debug:
                debugGo.Disable();
                hudGo.Enable();
                state = MenuState.Hud;
                playerScript.onPause = false;
                break;
            case MenuState.Settings:
                settingsGo.Disable();
                hudGo.Enable();
                state = MenuState.Hud;
                playerScript.onPause = false;
                break;
            case MenuState.Stats:
                statsGo.Disable();
                hudGo.Enable();
                state = MenuState.Hud;
                playerScript.onPause = false;
                break;
            case MenuState.Missions:
                missionsGo.Disable();
                hudGo.Enable();
                state = MenuState.Hud;
                playerScript.onPause = false;
                break;
        }
    }

    public void OpenMenu(MenuState state)
    {
        if (this.state != state)
        {
            switch (this.state)
            {
                case MenuState.Hud:
                    hudGo.Disable();
                    break;
                case MenuState.Inventory:
                    inventoryGo.Disable();
                    break;
                case MenuState.Pause:
                    pauseMenuGo.Disable();
                    break;
                case MenuState.Debug:
                    debugGo.Disable();
                    break;
                case MenuState.Settings:
                    settingsGo.Disable();
                    break;
                case MenuState.Stats:
                    statsGo.Disable();
                    break;
                case MenuState.Missions:
                    missionsGo.Disable();
                    break;
            }

            switch (state)
            {
                case MenuState.Hud:
                    hudGo.Enable();
                    state = MenuState.Hud;
                    playerScript.onPause = false;
                    break;
                case MenuState.Inventory:
                    inventoryGo.Enable();
                    state = MenuState.Inventory;
                    playerScript.onPause = true;
                    break;
                case MenuState.Pause:
                    pauseMenuGo.Enable();
                    state = MenuState.Pause;
                    playerScript.onPause = true;
                    break;
                case MenuState.Debug:
                    debugGo.Enable();
                    state = MenuState.Debug;
                    playerScript.onPause = true;
                    break;
                case MenuState.Settings:
                    settingsGo.Enable();
                    settingsCanvas.firstFrameUpdate = false;
                    playerScript.onPause = true;
                    break;
                case MenuState.Missions:
                    missionsGo.Enable();
                    playerScript.onPause = true;
                    break;
                case MenuState.Stats:
                    statsGo.Enable();
                    playerScript.onPause = true;
                    break;
            }
            this.previousState = this.state;
            this.state = state;
        }
    }

    public void OpenHudPopUpMenu(HudPopUpMenu type, string text = "", Dialoguer dialoguer = Dialoguer.ShopKeeper)
    {
        switch (type)
        {
            case HudPopUpMenu.SaveScene:
                if (saveOnCooldown)
                    break;
                savingSceneGo.Enable();
                if (text == "") text = "saving progress";
                savingCanvas.SetTextString(text, "Text_SavingProgress");
                saveOnCooldown = true;
                break;
            case HudPopUpMenu.PickUpFeedback:
                if (pickUpFeedbackOnCooldown)
                    break;
                pickUpFeedbackGo.Enable();
                pickupCanvas.SetTextString(text, "Text_PickedItem");
                pickUpFeedbackOnCooldown = true;
                break;
            case HudPopUpMenu.Dialogue:
                if (dialogueOnCooldown)
                    break;
                dialogueGo.Enable();
                dialogCanvas.SetTextString(text, "Text_Dialogue");
                dialogCanvas.PrintItemUI(false, "Img_ShopKeeper");
                dialogCanvas.PrintItemUI(false, "Img_Medic");
                dialogCanvas.PrintItemUI(false, "Img_CampLeader");
                dialogCanvas.PrintItemUI(false, "Img_Sargeant");
                switch (dialoguer)
                {
                    case Dialoguer.ShopKeeper:
                        dialogCanvas.PrintItemUI(true, "Img_ShopKeeper");
                        break;
                    case Dialoguer.Medic:
                        dialogCanvas.PrintItemUI(true, "Img_Medic");
                        break;
                    case Dialoguer.CampLeader:
                        dialogCanvas.PrintItemUI(true, "Img_CampLeader");
                        break;
                    case Dialoguer.Sargeant:
                        dialogCanvas.PrintItemUI(true, "Img_Sargeant");
                        break;
                    default:
                        break;
                }
                dialogueOnCooldown = true;
                break;
            default:
                break;
        }
    }

    public bool IsOnCooldown(HudPopUpMenu type)
    {
        switch (type)
        {
            case HudPopUpMenu.SaveScene:
                return saveOnCooldown;
            case HudPopUpMenu.PickUpFeedback:
                return pickUpFeedbackOnCooldown;
            case HudPopUpMenu.Dialogue:
                return dialogueOnCooldown;
        }

        return false;
    }
}