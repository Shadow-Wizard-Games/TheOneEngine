using System.Collections;

public class Raindrop : MonoBehaviour
{
    float bulletSpeed = 200.0f;
    float lifeTime = 2.0f;
    float currentTime = 0.0f;

    public override void Start()
    {
        //attachedGameObject.transform.Position = new Vector3(0.0f, 50.0f, 0.0f);
    }

    public override void Update()
    {
        currentTime += Time.deltaTime;

        if (currentTime > lifeTime)
        {
            currentTime = 0.0f;
            attachedGameObject.Destroy();
            return;
        }

        attachedGameObject.transform.Translate(Vector3.up * -bulletSpeed * Time.deltaTime);
    }
}