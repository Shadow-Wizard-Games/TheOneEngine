using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using static AudioManager;

class InternalCalls
{
    //Constructors
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static IntPtr GetGameObjectPtr();

    //Input
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool GetKeyboardButton(int id);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool GetControllerButton(int controllerButton, int gamePad);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void GetControllerJoystick(int joystick, ref Vector2 joyResult, int gamePad);

    //Transform
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool TransformCheck(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetPosition(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetPosition(IntPtr GOptr, ref Vector3 position);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetRotation(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetRotation(IntPtr GOptr, ref Vector3 rotation);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Translate(IntPtr GOptr, ref Vector3 increment);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Rotate(IntPtr GOptr, ref Vector3 increment);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetTransformForward(IntPtr GOptr);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetTransformRight(IntPtr GOptr);

    //GameObjects
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static IGameObject InstantiateBullet(Vector3 initialPosition, Vector3 direction);
    
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static string GetGameObjectName(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void DestroyGameObject(IntPtr GOtoDestroy);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static IntPtr FindGameObject(string name);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static IntPtr ComponentCheck(IntPtr gameObject, int componentType, string scriptName = null);
    
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static TComponent GetScript<TComponent>(IntPtr gameObject, string scriptName);

    //Component
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal extern TComponent GetComponent<TComponent>() where TComponent : IComponent;

    //Scene Manager
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void LoadScene(string sceneName);

    //User Interfaces
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void CanvasEnableToggle(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void MoveSelectedButton(IntPtr GOptr, int direction);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static int GetSelectiedButton(IntPtr GOptr);


    //Helpers
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static float GetAppDeltaTime();
    
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void ExitApplication();

    //Debug
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void ScriptingLog(string message, int logType);
    
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void DrawWireCircle(Vector3 position, float radius, Vector3 color);
    
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void DrawWireSphere(Vector3 position, float radius, Vector3 color);

    //Particle Systems
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void PlayPS(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void StopPS(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void ReplayPS(IntPtr GOptr);
    
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void PlaySource(IntPtr GOptr, EventIDs audio);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void StopSource(IntPtr GOptr, EventIDs audio);
}