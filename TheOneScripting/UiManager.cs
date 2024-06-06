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
        None = 0,
        Medic,
        CampLeader,
        Sargeant,
        Default,
        ShopKeeper        
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
        managers.Start();

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

        previousState = MenuState.Pause;
        state = MenuState.Hud;

        savingCanvas = savingSceneGo.GetComponent<ICanvas>();
        dialogCanvas = dialogueGo.GetComponent<ICanvas>();
        pickupCanvas = pickUpFeedbackGo.GetComponent<ICanvas>();

        settingsCanvas = settingsGo.GetComponent<UiScriptSettings>();

        // Check if it is paused and put it running if so
        // Disabled because do not know if it's necesary
        if (managers.gameManager.GetGameState() == GameManager.GameStates.PAUSED) { managers.gameManager.SetGameState(GameManager.GameStates.RUNNING); }
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

        if (pickUpFeedbackOnCooldown && pickUpFeedbackCooldown > 0.0f)
        {
            pickUpFeedbackCooldown -= dt;
            if (state != MenuState.Hud)
            {
                pickUpFeedbackOnCooldown = false;
                pickUpFeedbackGo.Disable();
            }
        }
        else if (pickUpFeedbackOnCooldown && pickUpFeedbackCooldown <= 0.0f)
        {
            pickUpFeedbackGo.Disable();
            pickUpFeedbackOnCooldown = false;
        }

        if (dialogueOnCooldown && dialogueCooldown > 0.0f)
        {
            dialogueCooldown -= dt;
            if (state != MenuState.Hud)
            {
                dialogueOnCooldown = false;
                dialogueGo.Disable();
            }
        }
        else if (dialogueOnCooldown && dialogueCooldown <= 0.0f)
        {
            dialogueGo.Disable();
            dialogueOnCooldown = false;
        }

        //previousState = state;

        if (!onCooldown)
        {
            if (state == MenuState.Death)
            {
                if (Input.GetControllerButton(Input.ControllerButtonCode.A) || Input.GetKeyboardButton(Input.KeyboardCode.RETURN))
                {
                    playerGO.source.Play(IAudioSource.AudioEvent.STOPMUSIC);
                    SceneManager.LoadScene("MainMenu");
                }
            }
            else if ((managers.gameManager.GetGameState() != GameManager.GameStates.DIALOGING) && (managers.gameManager.GetGameState() != GameManager.GameStates.PAUSED))
            {
                if (Input.GetControllerButton(Input.ControllerButtonCode.BACK) || Input.GetKeyboardButton(Input.KeyboardCode.I))
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
                else if (Input.GetControllerButton(Input.ControllerButtonCode.START) && state == MenuState.Hud ||
                    Input.GetControllerButton(Input.ControllerButtonCode.B) && state != MenuState.Hud ||
                    Input.GetKeyboardButton(Input.KeyboardCode.ESCAPE))
                {
                    if (state == MenuState.Hud) Debug.Log("Equivalent of pressing START");
                    if (state != MenuState.Hud) Debug.Log("Equivalent of pressing B");
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
            }

            if (playerScript.isDead && previousState != MenuState.Death)
            {
                deathScreenGo.Enable();
                state = MenuState.Death;
                managers.gameManager.SetGameState(GameManager.GameStates.ONMENUS);
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
                break;
            case MenuState.Pause:
                pauseMenuGo.Disable();
                hudGo.Enable();
                state = MenuState.Hud;
                break;
            case MenuState.Debug:
                debugGo.Disable();
                hudGo.Enable();
                state = MenuState.Hud;
                break;
            case MenuState.Settings:
                settingsGo.Disable();
                hudGo.Enable();
                state = MenuState.Hud;
                break;
            case MenuState.Stats:
                statsGo.Disable();
                hudGo.Enable();
                state = MenuState.Hud;
                break;
            case MenuState.Missions:
                missionsGo.Disable();
                hudGo.Enable();
                state = MenuState.Hud;
                break;
        }

        if(state != MenuState.Hud) { managers.gameManager.SetGameState(GameManager.GameStates.ONMENUS); }
    }

    public void OpenMenu(MenuState state)
    {
        if (this.state != state)
        {
            switch (this.state)
            {
                case MenuState.Hud:
                    managers.gameManager.SetGameState(GameManager.GameStates.ONMENUS);
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
                    managers.gameManager.SetGameState(GameManager.GameStates.RUNNING);
                    hudGo.Enable();
                    state = MenuState.Hud;
                    break;
                case MenuState.Inventory:
                    inventoryGo.Enable();
                    state = MenuState.Inventory;
                    break;
                case MenuState.Pause:
                    pauseMenuGo.Enable();
                    state = MenuState.Pause;
                    break;
                case MenuState.Debug:
                    debugGo.Enable();
                    state = MenuState.Debug;
                    break;
                case MenuState.Settings:
                    settingsGo.Enable();
                    break;
                case MenuState.Missions:
                    missionsGo.Enable();
                    break;
                case MenuState.Stats:
                    statsGo.Enable();
                    break;
            }
            this.previousState = this.state;
            this.state = state;
        }
    }

    public void OpenHudPopUpMenu(HudPopUpMenu type, string text1 = "", string text = "", Dialoguer dialoguer = Dialoguer.None, float cooldown = 4.5f)
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
                pickupCanvas.SetTextString(text1, "Text_TitleNewItem");
                pickUpFeedbackOnCooldown = true;
                pickUpFeedbackCooldown = cooldown;
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
                dialogCanvas.PrintItemUI(false, "Img_Default");
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
                    case Dialoguer.Default:
                        dialogCanvas.PrintItemUI(true, "Img_Default");
                        break;
                    default:
                        break;
                }
                dialogueOnCooldown = true;
                dialogueCooldown = cooldown;
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