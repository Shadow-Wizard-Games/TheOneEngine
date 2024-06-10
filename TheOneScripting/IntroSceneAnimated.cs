using static IAudioSource;
using System;

class IntroSceneAnimated : MonoBehaviour
{
    IGameObject toMoveLights;
    IGameObject anarchist1;
    IGameObject anarchist2;
    IGameObject mainCharacter;
    IntroScene introSceneStatic;

    bool alarmTriggered = false;
    bool guardsLeft = false;

    bool anarchistLeaving = false;

    bool anim3Playing = false;
    float anim2toidleCounter = 0.0f;

    public override void Start()
    {
        toMoveLights = IGameObject.Find("ToMoveLights");
        anarchist1 = IGameObject.Find("SK_Anarchist (1)");
        anarchist2 = IGameObject.Find("SK_Anarchist (2)");
        mainCharacter = IGameObject.Find("SK_MainCharacter");

        introSceneStatic = IGameObject.Find("IntroScene").GetComponent<IntroScene>();

        mainCharacter.animator.Play("PushUps");
        mainCharacter.animator.Blend = true; ;
        mainCharacter.animator.TransitionTime = 0.3f;;
        anarchist1.animator.Play("Scan");
        anarchist1.animator.Blend = true; ;
        anarchist1.animator.TransitionTime = 0.3f;;
        anarchist2.animator.Play("Scan");
        anarchist2.animator.Blend = true; ;
        anarchist2.animator.TransitionTime = 0.3f;;
    }

    public override void Update()
    {
        float dt = Time.deltaTime;

        mainCharacter.animator.UpdateAnimation();
        anarchist1.animator.UpdateAnimation();
        anarchist2.animator.UpdateAnimation();

        if (!alarmTriggered && (introSceneStatic.dialogNum == 5))
        {
            TriggerAlarm();
            alarmTriggered = true;
        }

        if (!guardsLeft && (introSceneStatic.dialogNum == 6))
        {
            TriggerGuardsLeft();
            guardsLeft = true;
        }

        if (anarchistLeaving)
        {
            anarchist1.transform.Position = new Vector3(anarchist1.transform.Position.x, anarchist1.transform.Position.y, anarchist1.transform.Position.z + dt * 10);
            anarchist2.transform.Position = new Vector3(anarchist2.transform.Position.x, anarchist2.transform.Position.y, anarchist2.transform.Position.z + dt * 10);
        }

        if (!anim3Playing && alarmTriggered) anim2toidleCounter += dt;

        if (anim2toidleCounter >= 2.7f)
        {
            mainCharacter.animator.Play("Idle");
            anim2toidleCounter = 0.0f;
            anim3Playing = true;
        }
    }

    public void TriggerAlarm()
    {
        //turn on alarm red lights
        toMoveLights.transform.Position = new Vector3(-200.0f, 0.0f, 0.0f);

        //player change animation
        mainCharacter.animator.Play("PushUpToIdle");
    }

    public void TriggerGuardsLeft()
    {
        //anarchists leaving
        anarchistLeaving = true;
    }
}

