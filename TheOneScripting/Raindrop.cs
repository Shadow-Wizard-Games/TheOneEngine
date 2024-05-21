using System.Collections;

public class Raindrop : MonoBehaviour
{
    float bulletSpeed = 50.0f;
    float lifeTime = 2.0f;
    float currentTime = 0.0f;

    public override void Update()
    {
        currentTime += Time.deltaTime;

        if (currentTime > lifeTime)
        {
            currentTime = 0.0f;
            attachedGameObject.parent.Destroy();
            return;
        }

        attachedGameObject.transform.position += Vector3.up * -bulletSpeed * Time.deltaTime;
    }
}