using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

class Bullet : MonoBehaviour
{
    float bulletSpeed = 300.0f;
    float lifeTime = 0.5f;
    float currentTime = 0.0f;

    public override void Update()
    {
        currentTime += Time.deltaTime;

        if (currentTime > lifeTime)
        {
            currentTime = 0.0f;
            attachedGameObject.Destroy();
            return;
        }

        attachedGameObject.transform.Translate(attachedGameObject.transform.forward * bulletSpeed * Time.deltaTime);
    }
}
