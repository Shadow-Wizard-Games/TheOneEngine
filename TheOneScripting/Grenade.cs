using System;

public class Grenade : MonoBehaviour
{
    IGameObject playerGO;
    PlayerScript player;

    IGameObject grenadeExplosionGO;
    IParticleSystem grenadeExplosion;

    Vector3 velocity;
    readonly float lifeTime = 6.0f;
    float currentTime = 0.0f;
    Vector3 gravity = new Vector3(0f, -200f, 0f);

    bool hasCollided = false;

    bool forceStart = true;
    bool alreadyCreated = false;

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        // Don't know why but in start the following are null
        //grenadeExplosion = attachedGameObject.FindInChildren("Explosion")?.GetComponent<IParticleSystem>();
        //grenadeExplosionGO = attachedGameObject.FindInChildren("Explosion");
    }

    public override void Update()
    {
        if(forceStart)
        {
            grenadeExplosion = attachedGameObject.FindInChildren("Explosion")?.GetComponent<IParticleSystem>();
            grenadeExplosionGO = attachedGameObject.FindInChildren("Explosion");
            forceStart = false;
        }

        currentTime += Time.deltaTime;

        Vector3 position = attachedGameObject.transform.Position + player.grenadeInitialVelocity * Time.deltaTime;
        player.grenadeInitialVelocity += gravity * Time.deltaTime;

        attachedGameObject.transform.Position = position;

        Vector3 rotation = attachedGameObject.transform.Position + player.grenadeInitialVelocity * Time.deltaTime;
        attachedGameObject.transform.Rotation = rotation;

        if ((attachedGameObject.transform.Position.y <= 0 || hasCollided) && !alreadyCreated)
        {
            Vector3 newPos = new Vector3(attachedGameObject.transform.Position.x, 0.0f, attachedGameObject.transform.Position.z);
            grenadeExplosion?.Replay();
            InternalCalls.CreatePrefab("GrenadeExplosion", newPos + Vector3.up * 4, attachedGameObject.transform.Rotation);
            alreadyCreated = true;
        }
        if(currentTime > lifeTime) { attachedGameObject.Destroy(); }
    }

    public void Impact()
    {
        hasCollided = true;
    }
}
