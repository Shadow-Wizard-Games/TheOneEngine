using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;

public class EventNextRoom : Event
{
    IGameObject playerGO;
    PlayerScript player;

    Vector3 directorVector;
    float playerDistance;

    float tpRange = 100.0f;
    bool inRange = false;

    string currentSceneName;
    uint sceneLevel = 0;
    uint roomNumber = 0;
    bool goNext = false;

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
        player = playerGO.GetComponent<PlayerScript>();

        eventType = EventType.NEXTROOM;
        currentSceneName = SceneManager.GetCurrentSceneName();
    }

    public override void Update()
    {
        if (CheckEventIsPossible())
        {
            DoEvent();
        }
        DrawEventDebug();
    }

    public override bool CheckEventIsPossible()
    {
        playerDistance = Vector3.Distance(playerGO.transform.position, attachedGameObject.transform.position);

        if (playerDistance < tpRange)
        {
            inRange = true;
        }
        else
        {
            inRange = false;
        }

        return inRange;
    }

    public override bool DoEvent()
    {
        bool ret = true;

        if(Input.GetKeyboardButton(Input.KeyboardCode.E))
        {
            sceneLevel = ExtractNumberFromSceneName('L');
            roomNumber = ExtractNumberFromSceneName('R');

            Debug.LogWarning(currentSceneName);
            Debug.LogWarning(sceneLevel.ToString());
            Debug.LogWarning(roomNumber.ToString());

            if(attachedGameObject.name == "GoNext")
            {
                goNext = true;
            }

            if (playerGO.source.currentID == IAudioSource.EventIDs.A_COMBAT_1)
            {
                playerGO.source.StopAudio(IAudioSource.EventIDs.A_COMBAT_1);
            }
            if (playerGO.source.currentID == IAudioSource.EventIDs.A_AMBIENT_1)
            {
                playerGO.source.StopAudio(IAudioSource.EventIDs.A_AMBIENT_1);
            }

            switch (sceneLevel)
            {
                case 1:
                    switch (roomNumber)
                    {
                        case 1:
                            SceneManager.LoadScene("L1R2");
                            break;
                        case 2:
                            if (goNext) SceneManager.LoadScene("L1R3");
                            else SceneManager.LoadScene("L1R1");
                            break;
                        case 3:
                            if (goNext) SceneManager.LoadScene("L1R4");
                            else SceneManager.LoadScene("L1R2");
                            break;
                        case 4:
                            if (goNext) SceneManager.LoadScene("L1R5");
                            else SceneManager.LoadScene("L1R3");
                            break;
                        case 5:
                            SceneManager.LoadScene("L1R4");
                            break;
                    }
                    break;
                case 2:
                    break;
                case 3:
                    break;
            }
        }

        return ret;
    }

    public override void DrawEventDebug()
    {
        if (!inRange)
        {
            Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, tpRange, new Vector3(1.0f, 0.8f, 0.0f)); //Yellow
        }
        else
        {
            Debug.DrawWireCircle(attachedGameObject.transform.position + Vector3.up * 4, tpRange, new Vector3(0.9f, 0.0f, 0.9f)); //Purple
        }
    }

    public uint ExtractNumberFromSceneName(char identifier)
    {
        string pattern = $"{identifier}(\\d+)";

        Match match = Regex.Match(currentSceneName, pattern);

        if (!match.Success)
        {
            throw new ArgumentException($"Identifier {identifier} not found in the string.");
        }

        string numberStr = match.Groups[1].Value;

        return uint.Parse(numberStr);
    }
}