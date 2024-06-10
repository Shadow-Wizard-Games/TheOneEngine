using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

class LaserBullet : MonoBehaviour
{
    float bulletSpeed = 350.0f;
    float lifeTime = 0.5f;
    float destroyLifeTime = 0.8f;
    float currentTime = 0.0f;

    public override void Update()
    {
        currentTime += Time.deltaTime;

        if (currentTime > destroyLifeTime)
        {
            currentTime = 0.0f;
            attachedGameObject.Destroy();
            return;
        }

        if (currentTime < lifeTime)
            attachedGameObject.transform.Translate(attachedGameObject.transform.Forward * bulletSpeed * Time.deltaTime);
    }
}
