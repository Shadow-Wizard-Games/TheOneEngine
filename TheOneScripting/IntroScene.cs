class IntroScene : MonoBehaviour
{
    public ICanvas canvasText;

    private float timerScene = 0.0f;
    private float durationScene = 5.0f;

    public override void Start()
    {
        canvasText = new ICanvas(containerGOptr);
    }

    public override void Update()
    {
        timerScene += Time.deltaTime;
        
        if ((Input.GetKeyboardButton(Input.KeyboardCode.RETURN) ||
            Input.GetKeyboardButton(Input.KeyboardCode.SPACEBAR) ||
            Input.GetControllerButton(Input.ControllerButtonCode.X)) &&
            timerScene >= durationScene)
        {
            SceneManager.LoadScene("L1R1");
        }
    }
}

