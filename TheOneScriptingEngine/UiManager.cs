using System;

public class UiManager : MonoBehaviour
{
    enum MenuState
    {
        Hud,
        Inventory,
        Death,
        Pause
    }

    IGameObject inventoryGo;
    IGameObject deathScreenGo;
    IGameObject pauseMenuGo;
    IGameObject hudGo;

    IGameObject playerGO;
    PlayerScript playerScript;

    MenuState state;

    float cooldown = 0;
    bool onCooldown = false;

    public override void Start()
    {
        inventoryGo = IGameObject.Find("Canvas_Inventory");
        deathScreenGo = IGameObject.Find("Canvas_Death");
        pauseMenuGo = IGameObject.Find("Canvas_PauseMenu");
        hudGo = IGameObject.Find("Canvas_Hud");

        playerGO = IGameObject.Find("SK_MainCharacter");
        playerScript = playerGO.GetComponent<PlayerScript>();

        inventoryGo.Disable();
        deathScreenGo.Disable();
        pauseMenuGo.Disable();
        state = MenuState.Hud;
        playerScript.onPause = false;
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

        MenuState previousState = state;

        if (!onCooldown)
        {
            if (state == MenuState.Death)
            {
                if (Input.GetKeyboardButton(Input.KeyboardCode.RETURN))
                {
                    if (playerScript.currentID == AudioManager.EventIDs.A_COMBAT_1)
                    {
                        playerGO.source.StopAudio(AudioManager.EventIDs.A_COMBAT_1);
                    }
                    if (playerScript.currentID == AudioManager.EventIDs.A_AMBIENT_1)
                    {
                        playerGO.source.StopAudio(AudioManager.EventIDs.A_AMBIENT_1);
                    }
                    SceneManager.LoadScene("MainMenu");
                }
            }
            else
            {
                if (Input.GetKeyboardButton(Input.KeyboardCode.I))
                {
                    if (previousState == MenuState.Inventory)
                    {
                        hudGo.Enable();
                        state = MenuState.Hud;
                        playerScript.onPause = false;
                    }
                    else
                    {
                        inventoryGo.Enable();
                        state = MenuState.Inventory;
                        playerScript.onPause = true;
                    }
                    onCooldown = true;
                }
                else if (Input.GetKeyboardButton(Input.KeyboardCode.ESCAPE))
                {
                    if (previousState == MenuState.Pause)
                    {
                        hudGo.Enable();
                        state = MenuState.Hud;
                        playerScript.onPause = false;
                    }
                    else if (previousState == MenuState.Inventory)
                    {
                        hudGo.Enable();
                        state = MenuState.Hud;
                        playerScript.onPause = false;
                    }
                    else
                    {
                        pauseMenuGo.Enable();
                        state = MenuState.Pause;
                        playerScript.onPause = true;
                    }
                    onCooldown = true;
                }
            }

            if (playerScript.isDead && previousState != MenuState.Death)
            {
                deathScreenGo.Enable();
                state = MenuState.Death;
                playerScript.onPause = true;
                onCooldown = true;
            }

            if (state != previousState)
            {
                switch (previousState)
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
                }
            }
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
        }
    }
}