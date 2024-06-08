using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

class AlienMeleeAttack : MonoBehaviour
{
    float lifeTimer = 0.0f;
    const float lifeCooldown = 0.1f;

    public override void Update()
    {
        lifeTimer += Time.deltaTime;

        if (lifeTimer >= lifeCooldown)
        {
            lifeTimer = 0.0f;
            attachedGameObject.Destroy();
            return;
        }
    }
}
