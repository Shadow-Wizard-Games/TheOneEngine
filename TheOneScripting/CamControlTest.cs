public class CamControlTest : MonoBehaviour
{

	float speed = 0.2f;
    public override void Update()
    {
        if (Input.GetKeyboardButton(Input.KeyboardCode.UP)) {
            attachedGameObject.transform.Translate(Vector3.forward * speed);
        }

        if (Input.GetKeyboardButton(Input.KeyboardCode.RIGHT)) {
            attachedGameObject.transform.Translate(Vector3.right * -speed);
        }

        if (Input.GetKeyboardButton(Input.KeyboardCode.DOWN)) {
            attachedGameObject.transform.Translate(Vector3.forward * -speed);
        }

        if (Input.GetKeyboardButton(Input.KeyboardCode.LEFT)) {
            attachedGameObject.transform.Translate(Vector3.right * speed);
        }

        if (Input.GetKeyboardButton(Input.KeyboardCode.W)) {
            attachedGameObject.transform.Translate(Vector3.up * speed);
        }
    }
}