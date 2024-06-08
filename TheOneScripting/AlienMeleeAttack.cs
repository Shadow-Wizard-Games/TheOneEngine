using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

class AlienMeleeAttack : MonoBehaviour
{
    float lifeTime = 0.1f;
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
    }
}
