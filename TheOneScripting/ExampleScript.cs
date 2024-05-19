using System;

public class ExampleScript : MonoBehaviour
{
    bool goingRight = true;
    bool goingUp = true;

    public override void Update()
    {
        //Horizontal
        if (attachedGameObject.transform.Position.x > 5.0f) goingRight = false;
        else if (attachedGameObject.transform.Position.x < -5.0f) goingRight = true;

        if (goingRight)
        {
            attachedGameObject.transform.Translate(new Vector3(0.1f, 0, 0));
        }
        else
        {
            attachedGameObject.transform.Translate(new Vector3(-0.1f, 0, 0));
        }

        //Vertical
        if (attachedGameObject.transform.Position.y > 5.0f) goingUp = false;
        else if (attachedGameObject.transform.Position.y < -5.0f) goingUp = true;

        if (goingUp)
        {
            attachedGameObject.transform.Translate(new Vector3(0, 0.1f, 0));
        }
        else
        {
            attachedGameObject.transform.Translate(new Vector3(0, -0.1f, 0));
        }
    }
}
