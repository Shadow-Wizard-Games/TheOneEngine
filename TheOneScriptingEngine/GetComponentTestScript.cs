using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class GetComponentTestScript : MonoBehaviour
{
    IGameObject getGoTest;
    IGameObject getGoTest2;

    ITransform testTransform;
    //IParticleSystem ps;

    //Scripts
    GetComponentTestScript2 testScript;

    float speed = 40.0f;
    Vector3 rotationVector = new Vector3(0.0f, 1.0f, 0.0f);

    public override void Start()
    {
        getGoTest = IGameObject.Find("SM_Box_LowPoly");
        getGoTest2 = IGameObject.Find("Empty GameObject");

        testTransform = getGoTest.GetComponent<ITransform>();

        //Get a script class from a GameObject
        testScript = getGoTest.GetComponent<GetComponentTestScript2>();
    }

    public override void Update()
    {
        if (Input.GetKeyboardButton(Input.KeyboardCode.SPACEBAR))
        {
            //Always check if the component is not null to avoid errors
            if (testScript != null)
            {
                testScript.ReduceLife1();
            }

            if (testTransform != null)
            {
                testTransform.Rotate(rotationVector * speed * Time.deltaTime);
            }
        }
    }
}
