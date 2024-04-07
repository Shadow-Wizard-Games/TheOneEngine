using System;

internal class ChestBusterBehaviour : MonoBehaviour
{
    enum States
    {
        Idle,
        Attack,
        Chase,
        Patrol,
        Dead
    }

    IGameObject playerGO;

    public override void Update()
    {

    }
}