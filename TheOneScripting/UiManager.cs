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

    IGameObject playerGO;
    PlayerScript playerScript;

    IGameObject GameManagerGO;
    GameManager gameManager;

    MenuState state;

    float cooldown = 0;
    bool onCooldown = false;

    float saveCooldown = 0;
    bool saveOnCooldown = false;

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

        if (saveOnCooldown && saveCooldown < 4.5f)
        {
            saveCooldown += dt;
            if (state != MenuState.Hud)
            {
                saveOnCooldown = false;
                saveCooldown = 0.0f;
                savingSceneGo.Disable();
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

        MenuState previousState = state;

        if (!onCooldown)
        {
            if (state == MenuState.Death)
            {
                if (Input.GetKeyboardButton(Input.KeyboardCode.RETURN))
                {
                    if (playerGO.source.currentID == IAudioSource.EventIDs.A_COMBAT_1)
                    {
                        playerGO.source.StopAudio(IAudioSource.EventIDs.A_COMBAT_1);
                    }
                    if (playerGO.source.currentID == IAudioSource.EventIDs.A_AMBIENT_1)
                    {
                        playerGO.source.StopAudio(IAudioSource.EventIDs.A_AMBIENT_1);
                    }
                    gameManager.UpdateLevel();
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
                else if(Input.GetKeyboardButton(Input.KeyboardCode.K))
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
                    attachedGameObject.source.PlayAudio(IAudioSource.EventIDs.UI_PAUSEGAME);
                    if (previousState == MenuState.Pause)
                    {
                        ResumeGame();
                    }
                    else
                    {
                        OpenMenu(MenuState.Pause);
                    }
                    onCooldown = true;
                }
                else if (Input.GetKeyboardButton(Input.KeyboardCode.F1))//for the moment for debug porpuses
                {
                    savingSceneGo.Enable();
                    saveOnCooldown = true;
                }
                else if (Input.GetKeyboardButton(Input.KeyboardCode.F2))//for the moment for debug porpuses
                {
                    pickUpFeedbackGo.Enable();
                    pickUpFeedbackOnCooldown = true;
                }
                else if (Input.GetKeyboardButton(Input.KeyboardCode.F3))//for the moment for debug porpuses
                {
                    dialogueGo.Enable();
                    dialogueOnCooldown = true;
                }
            }

            if (playerScript.isDead && previousState != MenuState.Death)
            {
                deathScreenGo.Enable();
                state = MenuState.Death;
                playerScript.onPause = true;
                onCooldown = true;
            }

            //if (state != previousState)
            //{
            //    switch (previousState)
            //    {
            //        case MenuState.Hud:
            //            hudGo.Disable();
            //            break;
            //        case MenuState.Inventory:
            //            inventoryGo.Disable();
            //            break;
            //        case MenuState.Pause:
            //            pauseMenuGo.Disable();
            //            break;
            //        case MenuState.Debug:
            //            debugGo.Disable();
            //            break;
            //        case MenuState.Settings:
            //            settingsGo.Disable();
            //            break;
            //        case MenuState.Stats:
            //            statsGo.Disable();
            //            break;
            //        case MenuState.SavingScene:
            //            savingSceneGo.Disable();
            //            break;
            //        case MenuState.Dialogue:
            //            dialogueGo.Disable();
            //            break;
            //        case MenuState.Missions:
            //            missionsGo.Disable();
            //            break;
            //        case MenuState.PickUpFeedback:
            //            pickUpFeedbackGo.Disable();
            //            break;
            //    }
            //}
        }
    }

    public void ResumeGame()
    {
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
            this.state = state;
        }
    }
}