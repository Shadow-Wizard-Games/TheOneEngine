using System;

public class CameraMovement : MonoBehaviour
{
    IGameObject playerGO;
    private Vector3 cameraOffset = new Vector3(90, 220, 90);
    //float camJoyDisplacement = 10.0f;

    readonly Vector3 cameraMargins = new Vector3(10, 0, 10);

    bool isCameraMoving = false;

    Easing cameraMovementEasing;

    public override void Start()
    {
        managers.Start();

        playerGO = IGameObject.Find("SK_MainCharacter");
        attachedGameObject.transform.Position = playerGO.transform.Position + cameraOffset;
        attachedGameObject.transform.CamLookAt(playerGO.transform.Position);

        cameraMovementEasing = new Easing(1, 0, false);
        cameraMovementEasing.Play();
    }

    public override void Update()
    {
        if (!attachedGameObject.transform.ComponentCheck()) { return; }

        if (managers.gameManager.GetGameState() == GameManager.GameStates.DIALOGING)
            cameraOffset = new Vector3(45, 110, 45);
        else
            cameraOffset = new Vector3(90, 220, 90);

        DoCameraMovement();
    }

    void DoCameraMovement()
    {
        Vector3 distanceCameraToPlayer = attachedGameObject.transform.Position - playerGO.transform.Position;

        Vector3 distanceDifferenceThanIdle = distanceCameraToPlayer - cameraOffset;

        Vector3 cameraMovement = Vector3.zero;

        bool hasMoved = false;

        if (Math.Abs(distanceDifferenceThanIdle.x) > cameraMargins.x)
        {
            hasMoved = true;

            if (distanceDifferenceThanIdle.x > 0)
            {
                cameraMovement = new Vector3(-(Math.Abs(distanceDifferenceThanIdle.x) - cameraMargins.x), cameraMovement.y, cameraMovement.z);
            }
            else
            {
                cameraMovement = new Vector3(Math.Abs(distanceDifferenceThanIdle.x) - cameraMargins.x, cameraMovement.y, cameraMovement.z);
            }
        }
        if (Math.Abs(distanceDifferenceThanIdle.y) > cameraMargins.y)
        {
            hasMoved = true;

            if (distanceDifferenceThanIdle.y > 0)
            {
                cameraMovement = new Vector3(cameraMovement.x, -(Math.Abs(distanceDifferenceThanIdle.y) - cameraMargins.y), cameraMovement.z);
            }
            else
            {
                cameraMovement = new Vector3(cameraMovement.x, Math.Abs(distanceDifferenceThanIdle.y) - cameraMargins.y, cameraMovement.z);
            }

        }
        if (Math.Abs(distanceDifferenceThanIdle.z) > cameraMargins.z)
        {
            hasMoved = true;

            if (distanceDifferenceThanIdle.z > 0)
            {
                cameraMovement = new Vector3(cameraMovement.x, cameraMovement.y, -(Math.Abs(distanceDifferenceThanIdle.z) - cameraMargins.z));
            }
            else
            {
                cameraMovement = new Vector3(cameraMovement.x, cameraMovement.y, Math.Abs(distanceDifferenceThanIdle.z) - cameraMargins.z);
            }

        }

        //if (distanceDifferenceThanIdle.x > cameraMargins.x)
        //{
        //    cameraMovement -= Vector3.right;
        //}
        //else if (distanceDifferenceThanIdle.x < -cameraMargins.x)
        //{
        //    cameraMovement += Vector3.right;
        //}

        //if (distanceDifferenceThanIdle.z > cameraMargins.z)
        //{
        //    cameraMovement -= Vector3.forward;
        //}
        //else if (distanceDifferenceThanIdle.z < -cameraMargins.z)
        //{
        //    cameraMovement += Vector3.forward;
        //}

        if (hasMoved != isCameraMoving)
        {
            cameraMovementEasing.Replay();
        }

        if (hasMoved)
        {
            attachedGameObject.transform.Translate(cameraMovement * (float)cameraMovementEasing.UpdateEasing(0, 1, Time.deltaTime, Easing.EasingType.EASE_IN_SIN));
        }
        else
        {
            attachedGameObject.transform.Translate(cameraMovement * (float)cameraMovementEasing.UpdateEasing(0, 1, Time.deltaTime, Easing.EasingType.EASE_OUT_SIN));

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
