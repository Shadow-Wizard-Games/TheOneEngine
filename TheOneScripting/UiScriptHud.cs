using System;

public class UiScriptHud : MonoBehaviour
{
    public ICanvas canvas;
    PlayerScript playerScript;

    public UiScriptHud()
    {
        canvas = new ICanvas(InternalCalls.GetGameObjectPtr());
    }

    public override void Start()
    {
        playerScript = IGameObject.Find("SK_MainCharacter").GetComponent<PlayerScript>();
    }
    public override void Update()
    {
        if(playerScript != null && canvas != null)
        {
            float currLife = playerScript.CurrentLife();
            int sliderMax = canvas.GetSliderMaxValue("LifeSlider");
            int life = (sliderMax * ((int)currLife)) / 100;
            
            if ((life != canvas.GetSliderValue("LifeSlider")) && (canvas.GetSliderValue("LifeSlider") != -1))
            {
                canvas.SetSliderValue(life, "LifeSlider");
            }
        }
    }
}