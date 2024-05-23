using System;

public class Explosion : MonoBehaviour
{
    readonly uint lifeTime = 1;
    uint counter = 0;

    public override void Update()
    {
        counter++;

        if (counter >= lifeTime)
        {
            attachedGameObject.Destroy();
        }
    }
}

