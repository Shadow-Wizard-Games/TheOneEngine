using System;

public class PlayerScript : MonoBehaviour
{
	float speed = 40.0f;	
	bool lastFrameToMove = false;

	public override void Update()
	{
		bool toMove = false;
		Vector3 movement = Vector3.zero;

		//Keyboard

		if (Input.GetKeyboardButton(Input.KeyboardCode.K)) {
			SceneManager.LoadScene("Scene");
		}

		if (Input.GetKeyboardButton(Input.KeyboardCode.W))
		{
			movement = movement + Vector3.forward;
			toMove = true;
		}
		if (Input.GetKeyboardButton(Input.KeyboardCode.D))
		{
			movement = movement - Vector3.right;
			toMove = true;
		}
		if (Input.GetKeyboardButton(Input.KeyboardCode.S))
		{
			movement = movement - Vector3.forward;
			toMove = true;
		}
		if (Input.GetKeyboardButton(Input.KeyboardCode.A))
		{
			movement = movement + Vector3.right;
			toMove = true;
        }

        if (Input.GetKeyboardButton(Input.KeyboardCode.UP))
		{
			attachedGameObject.transform.rotation = Vector3.zero;
		}
		if (Input.GetKeyboardButton(Input.KeyboardCode.LEFT))
		{
			attachedGameObject.transform.rotation = new Vector3(0, 1.57f, 0);
		}
		if (Input.GetKeyboardButton(Input.KeyboardCode.RIGHT))
		{
			attachedGameObject.transform.rotation = new Vector3(0, 4.71f, 0);
		}
		if (Input.GetKeyboardButton(Input.KeyboardCode.DOWN))
		{
			attachedGameObject.transform.rotation = new Vector3(0, 3.14f, 0);
		}

		if (Input.GetKeyboardButton(Input.KeyboardCode.SPACEBAR))
		{
            Vector3 height = new Vector3(0.0f, 30.0f, 0.0f);

            InternalCalls.InstantiateBullet(attachedGameObject.transform.position + attachedGameObject.transform.forward * 13.5f + height, attachedGameObject.transform.rotation);
            attachedGameObject.source.PlayAudio(AudioManager.EventIDs.GUNSHOT);
            // call particleSystem.Replay()
        }
      
		if (Input.GetKeyboardButton(Input.KeyboardCode.LSHIFT)) { speed = 80.0f; }
		else { speed = 40.0f; }

        if (toMove)
		{
			attachedGameObject.transform.Translate(movement.Normalize() * speed * Time.deltaTime);
        }

        //Controller
        Vector2 movementVector = Input.GetControllerJoystick(Input.ControllerJoystickCode.JOY_LEFT);

		if (movementVector.x != 0.0f || movementVector.y != 0.0f)
		{
			movement = new Vector3(-movementVector.x, 0.0f, -movementVector.y);

			attachedGameObject.transform.Translate(movement * speed * Time.deltaTime);

			toMove = true;
		}

		Vector2 lookVector = Input.GetControllerJoystick(Input.ControllerJoystickCode.JOY_RIGHT);

		if (lookVector.x != 0.0f || lookVector.y != 0.0f)
		{
			float characterRotation = (float)Math.Atan2(-lookVector.x, -lookVector.y);
			attachedGameObject.transform.rotation = new Vector3(0.0f, characterRotation, 0.0f);
		}

		if (Input.GetControllerButton(Input.ControllerButtonCode.R1))
        {
            Vector3 height = new Vector3( 0.0f, 30.0f, 0.0f );

            InternalCalls.InstantiateBullet(attachedGameObject.transform.position + attachedGameObject.transform.forward * 13.5f + height, attachedGameObject.transform.rotation);
            attachedGameObject.source.PlayAudio(AudioManager.EventIDs.GUNSHOT);
            // call particleSystem.Replay()
        }

        // Play steps
        if (lastFrameToMove != toMove)
        {
            if (toMove)
            {
                attachedGameObject.source.PlayAudio(AudioManager.EventIDs.STEP);
            }
            else
            {
                attachedGameObject.source.StopAudio(AudioManager.EventIDs.STEP);
            }
            lastFrameToMove = toMove;
        }
    }
}
