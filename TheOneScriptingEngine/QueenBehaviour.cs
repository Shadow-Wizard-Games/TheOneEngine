using System;

internal class QueenBehaviour : MonoBehaviour
{
    IGameObject playerGO;

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
    }
    public override void Update()
    {

    }

    void SpawnChesyBusters()
    {

    }
}