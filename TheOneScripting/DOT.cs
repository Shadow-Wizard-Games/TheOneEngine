using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

class DOT : MonoBehaviour
{
    float lifeTime = 0.1f;
    float currentTime = 0.0f;

    IGameObject player;

    public override void Start()
    {
        player = IGameObject.Find("SK_MainCharacter");
    }

    public override void Update()
    {
        currentTime += Time.deltaTime;

        if (currentTime > lifeTime)
        {
            currentTime = 0.0f;
            //attachedGameObject.Destroy();
            return;
        }
        attachedGameObject.transform.Position = player.transform.Position + player.transform.Forward * 50f + new Vector3(0,30,0);
    }

    public void DestroyCollider()
    {
        attachedGameObject.Destroy();
    }
}
