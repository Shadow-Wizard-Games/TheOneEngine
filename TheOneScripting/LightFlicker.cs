using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class LightFlicker : MonoBehaviour
{
    private static Random random = new Random();

    ILight lightComponent;
    public float maxInterval = 1;
    public float maxFlicker = 0.2f;

    float defaultIntensity;
    bool isOn;
    float timer;
    float delay;


    public override void Start()
    {
        lightComponent = attachedGameObject.GetComponent<ILight>();
        defaultIntensity = lightComponent.intensity;
    }

    public override void Update()
    {
        timer += Time.deltaTime;
        if (timer > delay)
        {
            ToggleLight();
        }
    }
    void ToggleLight()
    {
        isOn = !isOn;

        if (isOn)
        {
            lightComponent.intensity = defaultIntensity;
            delay = Range(0, maxInterval);
        }
        else
        {
            lightComponent.intensity = Range(0.6f, defaultIntensity);
            delay = Range(0, maxFlicker);
        }

        timer = 0;
    }

    float Range(float r1, float r2)
    {
        // Calcula la diferencia entre los dos rangos.
        float range = r2 - r1;
        // Genera un número aleatorio entre 0.0 y 1.0, luego escala y desplaza este número.
        float randomValue = (float)random.NextDouble() * range + r1;
        return randomValue;
    }

}
