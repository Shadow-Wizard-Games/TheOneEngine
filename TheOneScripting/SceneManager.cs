using System;
using System.Runtime.CompilerServices;

public class SceneManager {

    public static void LoadScene(string sceneName, bool keep = true) {
        InternalCalls.LoadScene(sceneName, keep);
    }
    
    public static string GetCurrentSceneName() {
        return InternalCalls.GetCurrentSceneName();
    }

    public static void CreatePrefab(string prefabName, Vector3 position)
    {
        InternalCalls.CreatePrefab(prefabName, position);
    }
}