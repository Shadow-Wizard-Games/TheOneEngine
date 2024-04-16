using System;
using System.Runtime.CompilerServices;

public class SceneManager {

    public static void LoadScene(string sceneName) {
        InternalCalls.LoadScene(sceneName);
    }
    
    public static string GetCurrentSceneName() {
        return InternalCalls.GetCurrentSceneName();
    }
}