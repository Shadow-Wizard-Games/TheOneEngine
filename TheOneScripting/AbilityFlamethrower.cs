using System.Security.AccessControl;

public class AbilityFlamethrower : MonoBehaviour
{
    public enum AbilityState
    {
        CHARGING,
        READY,
        ACTIVE,
        COOLDOWN,
    }

    Item_FlameThrower FlamethrowerItem;

    public string abilityName;
    public float activeTime;
    public float cooldownTime;
    public float activeTimeCounter;
    public float cooldownTimeCounter;

    public uint damage;
    public float slowAmount;

    public AbilityState state;

    IGameObject playerGO;
    PlayerScript player;

    public override void Start()
    {
        FlamethrowerItem = new Item_FlameThrower();

        name = FlamethrowerItem.name;

        activeTime = FlamethrowerItem.activeTime;
        activeTimeCounter = activeTime;
        cooldownTime = FlamethrowerItem.cooldownTime;
        cooldownTimeCounter = cooldownTime;

        damage = FlamethrowerItem.damage;

        playerGO = attachedGameObject.parent;
        player = playerGO.GetComponent<PlayerScript>();

        state = AbilityState.READY;
    }

    public override void Update()
    {
        switch (state)
        {
            case AbilityState.READY:

                //if (Input.GetKeyboardButton(Input.KeyboardCode.THREE))
                //{
                //    Activated();
                //
                //    break;
                //}
                break;

            case AbilityState.ACTIVE:

                WhileActive();
                Debug.Log("Active time = " + activeTimeCounter.ToString("F2"));
                break;

            case AbilityState.COOLDOWN:

                OnCooldown();
                Debug.Log("Cooldown time = " + cooldownTimeCounter.ToString("F2"));
                break;

        }
    }

    public void Activated()
    {
        // Set current weapon to the flamethrower

        attachedGameObject.source.Play(IAudioSource.AudioEvent.A_FT);

        state = AbilityState.ACTIVE;
    }

    public void WhileActive()
    {
        if (activeTimeCounter > 0.0f)
        {
            // update time
            activeTimeCounter -= Time.deltaTime;
        }
        else
        {
            player.currentWeaponType = PlayerScript.CurrentWeapon.SHOULDERLASER;
            player.FlamethrowerGO.Disable();
            player.ShoulderLaserGO.Enable();
            activeTimeCounter = activeTime;

            state = AbilityState.COOLDOWN;
        }
    }

    public void OnCooldown()
    {
        if (cooldownTime > 0.0f)
        {
            // update time
            cooldownTimeCounter -= Time.deltaTime;
        }
        else
        {

            cooldownTimeCounter = cooldownTime;
            state = AbilityState.READY;

            Debug.Log("Flamethrower Ready");
        }
    }
}

