using System;
using System.Runtime.CompilerServices;

public class SceneManager
{
    public static void LoadScene(string sceneName, bool keep = true, string path = "GameData/Scenes/")
    {
        InternalCalls.LoadScene(sceneName, keep, path);
    }

    public static string GetCurrentSceneName()
    {
        return InternalCalls.GetCurrentSceneName();
    }

    public static void CreatePrefab(string prefabName, Vector3 position)
    {
        InternalCalls.CreatePrefab(prefabName, position);
    }
}