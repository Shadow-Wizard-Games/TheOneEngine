using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


public abstract class Event : MonoBehaviour
{
    public enum EventType
    {
        UNKNOWN = 0,
        COLLECTIBLE,
        NEXTROOM,
        //Add events
    }
    public EventType eventType {  get; set; }

    public abstract bool CheckEventIsPossible();
    public abstract bool DoEvent();
    public abstract void DrawEventDebug();
}
