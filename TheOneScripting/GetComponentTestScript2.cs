using System;

public class GetComponentTestScript2 : MonoBehaviour
{
    public int life = 500;

    public void ReduceLife1()
    {
        Debug.LogCheck("TestScript2 func1 current life is: " + life.ToString());
        life -= 50;
    }
    public void ReduceLife2()
    {
        Debug.LogError("TestScript2 func2 current life is: " + life.ToString());
        life -= 10;
    }
}