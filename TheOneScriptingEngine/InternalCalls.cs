using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using static AudioManager;
using static ICamera;

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
    internal extern static Vector3 GetScale(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetScale(IntPtr GOptr, ref Vector3 scale);

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
    internal extern static IGameObject InstantiateXenomorph(Vector3 initialPosition, Vector3 direction, Vector3 scale);

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

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Disable(IntPtr GOtoDisable);

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
    internal extern static int GetSelectedButton(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void ChangeSectImg(IntPtr GOptr, string name, int x, int y, int w, int h);


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
    
    //Audio
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void PlaySource(IntPtr GOptr, EventIDs audio);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void StopSource(IntPtr GOptr, EventIDs audio);

    //Collider2D
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static float GetColliderRadius(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetColliderRadius(IntPtr GOptr, ref float radiusToSet);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector2 GetColliderBoxSize(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetColliderBoxSize(IntPtr GOptr, ref Vector2 sizeToSet);

    //Camera
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static double GetFov(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetFov(IntPtr GOptr, ref double fov);
    
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static double GetAspect(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetAspect(IntPtr GOptr, ref double aspect);
    
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static float GetYaw(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetYaw(IntPtr GOptr, ref float yaw);
    
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static float GetPitch(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetPitch(IntPtr GOptr, ref float pitch);
    
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static CameraType GetCameraType(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetCameraType(IntPtr GOptr, ref CameraType cameraType);
    
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool GetPrimaryCam(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetPrimaryCam(IntPtr GOptr, ref bool cameraType);
}