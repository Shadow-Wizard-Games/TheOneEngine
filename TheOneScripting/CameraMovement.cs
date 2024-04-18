using System;

public class CameraMovement : MonoBehaviour
{
	IGameObject playerGO;
    PlayerScript playerScript;
	Vector3 camOffset = new Vector3(0, 130, 60);
	//float camJoyDisplacement = 10.0f;

    float cameraMargin = 15.0f;

	public override void Start()
	{
		playerGO = IGameObject.Find("SK_MainCharacter");
        playerScript = playerGO.GetComponent<PlayerScript>();
        attachedGameObject.transform.position = playerGO.transform.position + camOffset;
        attachedGameObject.transform.LookAt(playerGO.transform.position);
        attachedGameObject.transform.Rotate(attachedGameObject.transform.right * 65);
    }

    public override void Update()
	{
        if (!attachedGameObject.transform.ComponentCheck()) { return; }

        Vector3 difference = attachedGameObject.transform.position - playerGO.transform.position;

        Vector3 displacement = difference - camOffset;

        if (Math.Abs(displacement.x) > cameraMargin || Math.Abs(displacement.z) > cameraMargin)
        {
            attachedGameObject.transform.Translate(playerScript.movement.Normalize() * playerScript.speed * Time.deltaTime);
        }
        //Debug.Log("X: " + displacement.x + " Y: " + displacement.y + " Z: " + displacement.z);

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
