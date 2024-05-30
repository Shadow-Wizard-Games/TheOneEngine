using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class AbilityImpaciente : Ability
{
    IGameObject playerGO;
    PlayerScript player;

    readonly int damage = 10;

    readonly float slowAmount = 0.25f;
    readonly float impacienteShootingCd = 0.05f;
    readonly float knockbackPotency = -3f;

    public override void Start()
    {
        abilityName = "Impaciente";
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        activeTime = 25.0f;
        activeTimeCounter = activeTime;
        cooldownTime = 45.0f;
        cooldownTimeCounter = cooldownTime;
    }

    // put update and call the abilityStatUpdate from there or 
    public override void Update()
    {
        switch (state)
        {
            case AbilityState.CHARGING:
                break;
            case AbilityState.READY:
                if (Input.GetKeyboardButton(Input.KeyboardCode.FOUR))
                {
                    Activated();
                    break;
                }
                // controller input
                break;
            case AbilityState.ACTIVE:
                WhileActive();
                Debug.Log("Impaciente active time -> " + activeTimeCounter.ToString("F2"));
                break;
            case AbilityState.COOLDOWN:
                OnCooldown();
                Debug.Log("Impaciente cooldown time -> " + cooldownTimeCounter.ToString("F2"));
                break;
        }
    }

    public override void Activated()
    {
        player.currentWeapon = PlayerScript.CurrentWeapon.IMPACIENTE;

        // rpm
        player.shootingCooldown = impacienteShootingCd;

        float speedReduce = player.baseSpeed * slowAmount;
        player.speed -= speedReduce;

        player.currentWeoponDamage = damage;

        state = AbilityState.ACTIVE;

        attachedGameObject.source.Play(IAudioSource.AudioEvent.A_LI);

        Debug.Log("Ability Impaciente Activated");
    }

    public override void WhileActive()
    {

        if (activeTimeCounter > 0)
        {
            // update time
            activeTimeCounter -= Time.deltaTime;

            if (player.isShooting)
                player.attachedGameObject.transform.Translate(player.lastMovementDirection * knockbackPotency * Time.deltaTime);

            if (Input.GetKeyboardButton(Input.KeyboardCode.FOUR) || player.currentWeapon != PlayerScript.CurrentWeapon.IMPACIENTE || player.impacienteBulletCounter >= player.impacienteBullets)
            {
                // reset stats
                player.shootingCooldown = player.mp4ShootingCd;
                player.speed = player.baseSpeed;
                player.currentWeapon = PlayerScript.CurrentWeapon.M4;
                player.currentWeoponDamage = damage;
                player.impacienteBulletCounter = 0;
                
                activeTimeCounter = activeTime;
                state = AbilityState.COOLDOWN;

                Debug.Log("Ability Impaciente on Cooldown");
            }
        }
        else
        {
            // reset stats
            player.shootingCooldown = player.mp4ShootingCd;
            player.speed = player.baseSpeed;
            player.currentWeapon = PlayerScript.CurrentWeapon.M4;
            player.currentWeoponDamage = damage;
            activeTimeCounter = activeTime;
            player.impacienteBulletCounter = 0;

            state = AbilityState.COOLDOWN;

            Debug.Log("Ability Impaciente on Cooldown");
        }
    }

    public override void OnCooldown()
    {
        if (cooldownTimeCounter > 0)
        {
            // update time
            cooldownTimeCounter -= Time.deltaTime;
        }
        else
        {
            cooldownTimeCounter = cooldownTime;
            state = AbilityState.READY;

            Debug.Log("Ability Impaciente Ready");
        }

    }
}
