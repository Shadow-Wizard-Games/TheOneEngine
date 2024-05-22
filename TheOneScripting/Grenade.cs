using System;

public class Grenade : MonoBehaviour
{
    IGameObject playerGO;
    PlayerScript player;

    Vector3 velocity;
    readonly float lifeTime = 20.0f;
    float currentTime = 0.0f;
    Vector3 gravity = new Vector3(0f, -110f, 0f);

    public override void Start()
    {
        // Calculate initial velocity for 10 meters range
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();
    }

    public override void Update()
    {
        currentTime += Time.deltaTime;

        if (currentTime > lifeTime || attachedGameObject.transform.Position.y <= 0)
        {
            currentTime = 0.0f;
            velocity = player.grenadeInitialVelocity;
            attachedGameObject.Destroy();
            return;
        }

        Vector3 position = attachedGameObject.transform.Position + player.grenadeInitialVelocity * Time.deltaTime;
        player.grenadeInitialVelocity += gravity * Time.deltaTime;

        attachedGameObject.transform.SetPosition(position);
        Debug.Log(" vel starto 3 = x> " + player.grenadeInitialVelocity.x + " z>" + player.grenadeInitialVelocity.z);
        Debug.LogWarning("Position -> " + position.x + " " + position.y + " " + position.z);
        Debug.LogWarning("Velocity -> " + velocity.x + " " + velocity.y + " " + velocity.z);
    }
}
