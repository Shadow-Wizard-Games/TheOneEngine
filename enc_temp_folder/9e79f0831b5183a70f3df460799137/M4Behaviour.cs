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
        attachedGameObject.transform = playerTransform;

        ChangeWeaponState();

        switch (M4State)
        {
            case State.IDLE:
                attachedGameObject.animator.Play("Idle");
                break;
            case State.RUN:
                attachedGameObject.animator.Play("Run");
                break;
            case State.SHOOT:
                attachedGameObject.animator.Play("Shoot");
                break;
            case State.RUNSHOOT:
                attachedGameObject.animator.Play("Run and Shoot");
                break;
        }

        attachedGameObject.animator.UpdateAnimation();
    }

    private void ChangeWeaponState()
    {
        if (player.currentWeapon == PlayerScript.CurrentWeapon.M4)
        {
            switch (player.currentAction)
            {
                case PlayerScript.CurrentAction.IDLE:
                    M4State = State.IDLE;
                    break;
                case PlayerScript.CurrentAction.RUN:
                    M4State = State.RUN;
                    break;
                case PlayerScript.CurrentAction.SHOOT:
                    M4State = State.SHOOT;
                    break;
                case PlayerScript.CurrentAction.RUNSHOOT:
                    M4State = State.RUNSHOOT;
                    break;
            }
        }
    }
}

