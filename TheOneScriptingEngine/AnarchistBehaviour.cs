using System;

internal class AnarchistBehaviour : MonoBehaviour
{
    IGameObject playerGO;

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
    }

    public override void Update()
    {

    }
}