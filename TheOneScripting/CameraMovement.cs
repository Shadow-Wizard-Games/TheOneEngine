using System;

public class CameraMovement : MonoBehaviour
{
	IGameObject playerGO;
    PlayerScript playerScript;
	Vector3 camOffset = new Vector3(90, 220, 90);
	//float camJoyDisplacement = 10.0f;

    float cameraMargin = 15.0f;

	public override void Start()
	{
		playerGO = IGameObject.Find("SK_MainCharacter");
        playerScript = playerGO.GetComponent<PlayerScript>();
        attachedGameObject.transform.position = playerGO.transform.position + camOffset;
        attachedGameObject.transform.LookAt(playerGO.transform.position);
    }

    public override void Update()
	{
        if (!attachedGameObject.transform.ComponentCheck()) { return; }

        DoCameraMovement();
    }

    void DoCameraMovement()
    {
        Vector3 difference = attachedGameObject.transform.position - playerGO.transform.position;

        Vector3 displacement = difference - camOffset;

        Vector3 camMovement = Vector3.zero;

        if (displacement.x > cameraMargin)
        {
            camMovement -= Vector3.right;
        }
        else if (displacement.x < -cameraMargin)
        {
            camMovement += Vector3.right;
        }
        if (displacement.z > cameraMargin)
        {
            camMovement -= Vector3.forward;
        }
        else if (displacement.z < -cameraMargin)
        {
            camMovement += Vector3.forward;
        }

        attachedGameObject.transform.Translate(camMovement.Normalize() * playerScript.speed * Time.deltaTime);

        //Debug.Log("X: " + displacement.x + " | " + " Z: " + displacement.z);

        #region Old Behaviour
        /*
        attachedGameObject.transform.position = playerGO.transform.position + camOffset;

        Vector2 lookVector = Input.GetControllerJoystick(Input.ControllerJoystickCode.JOY_RIGHT);

        if (Math.Abs(lookVector.x) > 0.1f || Math.Abs(lookVector.y) > 0.1f)
        {
            Vector3 cameraDisplacement = new Vector3(-lookVector.x, 0.0f, -lookVector.y);
            attachedGameObject.transform.position += cameraDisplacement * camJoyDisplacement;
        }
        */
        #endregion
    }
}
