using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

internal class M4Behaviour : MonoBehaviour
{
    State M4State;

    PlayerScript player;
    ITransform playerTransform;

    enum State
    {
        IDLE,
        RUN,
        SHOOT,
        RUNSHOOT,
        NOTM4
    }

    public override void Start()
    {
        player = IGameObject.Find("SK_MainCharacter").GetComponent<PlayerScript>();
        
        playerTransform = player.attachedGameObject.GetComponent<ITransform>();
    }

    public override void Update()
    {
        attachedGameObject.transform.SetPosition(playerTransform.Position);

        ChangeWeaponState();

        switch (M4State)
        {
            case State.IDLE:
                attachedGameObject.animator.Play("Idle");
                break;
            case State.RUN:

                break;
            case State.SHOOT:

                break;
            case State.RUNSHOOT:

                break;
        }
    }

    private void ChangeWeaponState()
    {
        if (player.currentWeapon == PlayerScript.CurrentWeapon.M4)
        {
            M4State = State.IDLE;


        }
    }
}

