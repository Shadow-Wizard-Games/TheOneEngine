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

    public Item_FlameThrower FlamethrowerItem;

    public string abilityName;
    public float activeTime;
    public float cooldownTime;
    public float activeTimeCounter;
    public float cooldownTimeCounter;

    public float receiveDmgIntervalTime;

    public float waitToReset = 0.5f;

    public int damage;
    public float slowAmount;

    public bool shooting;
    public bool playParticle;

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

        receiveDmgIntervalTime = FlamethrowerItem.intervalTime;

        shooting = false;
        playParticle = false;

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
                break;

            case AbilityState.COOLDOWN:

                OnCooldown();
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

            if(Input.GetKeyboardButtonUp(Input.KeyboardCode.SPACEBAR)
               || Input.GetControllerButtonUp(Input.ControllerButtonCode.R2) 
               || Input.GetMouseButtonUp(Input.MouseButtonCode.LEFT))
            {
                shooting = false;
                player.flameThrowerLight.SwitchOff();
                player.flameThrowerPS.Stop();
                if(IGameObject.Find("DOT") != null)
                    IGameObject.Find("DOT").GetComponent<DOT>().DestroyCollider();
            }


            if(playParticle)
            {
                player.flameThrowerPS.Play();
                playParticle = false;
            }

            if ((Input.GetKeyboardButton(Input.KeyboardCode.THREE) || Input.GetControllerButton(Input.ControllerButtonCode.R1)) && activeTimeCounter < activeTime - waitToReset)
            {
                // reset stats

                player.currentWeaponType = PlayerScript.CurrentWeapon.SHOULDERLASER;
                activeTimeCounter = activeTime;
                player.flameThrowerPS.Stop();
                player.FlamethrowerGO.Disable();
                player.flameThrowerLight.SwitchOff();
                player.ShoulderLaserGO.Enable();

                if (IGameObject.Find("DOT") != null)
                    IGameObject.Find("DOT").GetComponent<DOT>().DestroyCollider();

                player.currentWeaponDamage = damage;

                state = AbilityState.COOLDOWN;

                player.hudScript.SetFlameThrowerOnCD();

                Debug.Log("Flamethrower on Cooldown");
            }

        }
        else
        {
            player.currentWeaponType = PlayerScript.CurrentWeapon.SHOULDERLASER;
            activeTimeCounter = activeTime;
            player.flameThrowerPS.Stop();
            player.FlamethrowerGO.Disable();
            player.flameThrowerLight.SwitchOff();
            player.ShoulderLaserGO.Enable();

            if (IGameObject.Find("DOT") != null)
                IGameObject.Find("DOT").GetComponent<DOT>().DestroyCollider();

            player.currentWeaponDamage = damage;

            state = AbilityState.COOLDOWN;

            Debug.Log("Flamethrower on Cooldown");
        }
    }

    public void OnCooldown()
    {
        if (cooldownTimeCounter > 0.0f)
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

