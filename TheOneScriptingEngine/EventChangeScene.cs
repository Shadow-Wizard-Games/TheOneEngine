using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static AudioManager;

public class EventChangeScene : Event
{
    IGameObject playerGO;
    Vector3 directorVector;
    float playerDistance;

    float tpRange = 30.0f;
    bool inRange = false;

    AudioManager.EventIDs currentID = 0;    //change to correct audio ID

    public override void Start()
    {
        playerGO = IGameObject.Find("SK_MainCharacter");
        eventType = EventType.NEXTROOM;
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
            Debug.Log("TPinRange");
        }
        else
        {
            inRange = false;
            Debug.Log("TPoutRange");
        }

        return inRange;
    }

    public override bool DoEvent()
    {
        bool ret = true;

        SceneManager.LoadScene("MainMenu");

        return ret;
    }

    public override void DrawEventDebug()
    {

    }
}