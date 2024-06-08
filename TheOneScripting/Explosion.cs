using System;

public class Explosion : MonoBehaviour
{
    readonly int lifeTime = 1;
    int counter = 0;

    public override void Start()
    {
        attachedGameObject.Destroy();
    }
}

