using System;

public class Grenade : MonoBehaviour
{
    IGameObject playerGO;
    PlayerScript player;

    Vector3 velocity;
    readonly float lifeTime = 10.0f;
    float currentTime = 0.0f;
    Vector3 gravity = new Vector3(0f, -110f, 0f);

    bool hasCollided = false;

    public override void Start()
    {
        // Calculate initial velocity for 10 meters range
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();
    }

    public override void Update()
    {
        currentTime += Time.deltaTime;

        if (attachedGameObject.transform.Position.y <= 0 || hasCollided || currentTime > lifeTime)
        {
            Vector3 newPos = new Vector3(attachedGameObject.transform.Position.x, 0.0f, attachedGameObject.transform.Position.z);
            player.explosionPos = newPos;
            InternalCalls.InstantiateExplosion(newPos + Vector3.up * 4, attachedGameObject.transform.Rotation, player.grenadeExplosionRadius);
            attachedGameObject.Destroy();
        }

        Vector3 position = attachedGameObject.transform.Position + player.grenadeInitialVelocity * Time.deltaTime;
        player.grenadeInitialVelocity += gravity * Time.deltaTime;

        attachedGameObject.transform.SetPosition(position);
    }

    public void Impact()
    {
        hasCollided = true;
    }
}
