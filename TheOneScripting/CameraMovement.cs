using System;

public class CameraMovement : MonoBehaviour
{
    IGameObject playerGO;
    PlayerScript playerScript;
    readonly Vector3 camOffset = new Vector3(90, 220, 90);
    //float camJoyDisplacement = 10.0f;

    readonly float cameraMargin = 10.0f;

    bool isCameraMoving = false;

    Easing cameraMovementEasing;

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
        playerScript = playerGO.GetComponent<PlayerScript>();
        attachedGameObject.transform.Position = playerGO.transform.Position + camOffset;
        attachedGameObject.transform.CamLookAt(playerGO.transform.Position);

        cameraMovementEasing = new Easing(0.2, 0, false);
        cameraMovementEasing.Play();
    }

    public override void Update()
    {
        if (!attachedGameObject.transform.ComponentCheck()) { return; }

        DoCameraMovement();
    }

    void DoCameraMovement()
    {
        Vector3 difference = attachedGameObject.transform.Position - playerGO.transform.Position;

        Vector3 displacement = difference - camOffset;

        Vector3 camMovement = Vector3.zero;

        bool hasMoved = false;

        if (Math.Abs(displacement.x) > cameraMargin || Math.Abs(displacement.y) > cameraMargin || Math.Abs(displacement.z) > cameraMargin)
        {
            hasMoved = true;
        }

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

        if (hasMoved != isCameraMoving)
        {
            cameraMovementEasing.Replay();
        }

        if (hasMoved)
        {
            attachedGameObject.transform.Translate(camMovement.Normalize() * (playerScript.speed * (float)cameraMovementEasing.UpdateEasing(0, 1, Time.deltaTime, Easing.EasingType.EASE_IN_SIN)) * Time.deltaTime);
        }
        else {
            attachedGameObject.transform.Translate(camMovement.Normalize() * (playerScript.speed * (float)cameraMovementEasing.UpdateEasing(0, 1, Time.deltaTime, Easing.EasingType.EASE_OUT_SIN)) * Time.deltaTime);
        }
        
        //attachedGameObject.transform.Translate(camMovement.Normalize() * (playerScript.speed * (float)cameraMovementEasing.UpdateEasing(0, 1, Time.deltaTime, Easing.EasingType.EASE_IN_SIN)) * Time.deltaTime);

        isCameraMoving = hasMoved;

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
