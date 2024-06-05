using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using static IAudioSource;
using static ICamera;

class InternalCalls
{
    #region Constructors
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static IntPtr GetGameObjectPtr();
    #endregion

    #region Input
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool GetKeyboardButton(int id);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool GetControllerButton(int controllerButton, int gamePad);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void GetControllerJoystick(int joystick, ref Vector2 joyResult, int gamePad);
    #endregion

    #region Transform
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
    #endregion

    #region GameObject
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static IGameObject InstantiateBullet(Vector3 initialPosition, Vector3 direction);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static IGameObject InstantiateGrenade(Vector3 initialPosition, Vector3 direction);
    
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static IGameObject InstantiateExplosion(Vector3 initialPosition, Vector3 direction, float radius);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static IGameObject InstantiateXenomorph(Vector3 initialPosition, Vector3 direction, Vector3 scale);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static string GetGameObjectName(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void DestroyGameObject(IntPtr GOtoDestroy);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static IntPtr FindGameObject(string name);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static IntPtr GetParent(IntPtr currentGameObject);
    
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static IntPtr FindGameObjectInChildren(IntPtr gameObject, string name);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static IntPtr ComponentCheck(IntPtr gameObject, int componentType, string scriptName = null);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static TComponent GetScript<TComponent>(IntPtr gameObject, string scriptName);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Disable(IntPtr GOtoDisable);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Enable(IntPtr GOtoEnable);
    #endregion

    #region Component
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal extern TComponent GetComponent<TComponent>() where TComponent : IComponent;
    #endregion

    #region Scene Manager
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void LoadScene(string sceneName, bool keep = false, string path = "GameData/Scenes/");

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void CreateSaveFromScene(string filepath, string sceneName);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void RemoveFile(string filepath);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static string AccessFileDataString(string filepath, string[] dataPath, string dataName);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static int AccessFileDataInt(string filepath, string[] dataPath, string dataName);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool AccessFileDataBool(string filepath, string[] dataPath, string dataName);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static float AccessFileDataFloat(string filepath, string[] dataPath, string dataName);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void WriteFileDataString(string filepath, string[] dataPath, string dataName, string data);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void WriteFileDataInt(string filepath, string[] dataPath, string dataName, int data);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void WriteFileDataBool(string filepath, string[] dataPath, string dataName, bool data);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void WriteFileDataFloat(string filepath, string[] dataPath, string dataName, float data);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static string GetCurrentSceneName();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void CreatePrefab(string prefabName, Vector3 position, Vector3 rotation);
    #endregion

    #region User Interfaces
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void CanvasEnableToggle(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void MoveSelectedButton(IntPtr GOptr, int direction, bool children = false);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void MoveSelection(IntPtr GOptr, int direction, bool children = false);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static int GetSelectedButton(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static int SetUiItemState(IntPtr GOptr, int state, string name);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static int ToggleChecker(IntPtr GOptr, bool value, string nameM);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static int PrintItemUI(IntPtr GOptr, bool value, string nameM);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static int GetSelected(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void ChangeSectImg(IntPtr GOptr, string name, int x, int y, int w, int h);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetSliderValue(IntPtr GOptr, int value, string name);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static int GetSliderValue(IntPtr GOptr, string name);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static int GetSliderMaxValue(IntPtr GOptr, string name);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetTextString(IntPtr GOptr, string text, string name, int num = -1);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static string GetTextString(IntPtr GOptr, string name);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void CanvasFlicker(IntPtr GOptr, bool flicker);
    #endregion

    #region Helpers
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static float GetAppDeltaTime();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void ExitApplication();
    #endregion

    #region Debug
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void ScriptingLog(string message, int logType);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void DrawWireCircle(Vector3 position, float radius, Vector3 color);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void DrawWireSphere(Vector3 position, float radius, Vector3 color);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void ToggleCollidersDraw();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void ToggleGridDraw();
    #endregion

    #region Particle System
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void PlayPS(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void PausePS(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void ReplayPS(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void StopPS(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void EndPS(IntPtr GOptr);

    #endregion

    #region Audio
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void PlayAudioSource(IntPtr GOptr, AudioEvent audio);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void StopAudioSource(IntPtr GOptr, AudioEvent audio);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetState(AudioStateGroup stateGroup, AudioStateID stateID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetSwitch(IntPtr GOptr, AudioSwitchGroup switchGroup, AudioSwitchID switchID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetMasterVolume(int volume);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetSFXVolume(int volume);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetMusicVolume(int volume);
    #endregion

    #region Collider2D
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static float GetColliderRadius(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetColliderRadius(IntPtr GOptr, ref float radiusToSet);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector2 GetColliderBoxSize(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetColliderBoxSize(IntPtr GOptr, ref Vector2 sizeToSet);
    #endregion

    #region Camera
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
    #endregion

    #region Animation
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void PlayAnimation(IntPtr GOptr, string name);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void StopAnimation(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool AnimationHasFinished(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool GetTransitionBlend(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetTransitionBlend(IntPtr GOptr, ref bool blend);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static float GetTransitionTime(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetTransitionTime(IntPtr GOptr, ref float time);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void UpdateAnimation(IntPtr GOptr, ref float dt);
    #endregion

    #region Light
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetLightColor(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetLightColor(IntPtr GOptr, ref Vector3 color);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static float GetLightIntensity(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetLightIntensity(IntPtr GOptr, ref float intensity);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static float GetLightRadius(IntPtr GOptr);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetLightRadius(IntPtr GOptr, ref float radius);
    #endregion
}