using System;

public class Debug
{
    public static void Log(string message)
    {
        InternalCalls.ScriptingLog(message, 0);
    }
    public static void LogCheck(string message)
    {
        InternalCalls.ScriptingLog(message, 4);
    }
    public static void LogWarning(string message)
    {
        InternalCalls.ScriptingLog(message, 5);
    }
    public static void LogError(string message)
    {
        InternalCalls.ScriptingLog(message, 6);
    }
    public static void DrawWireCircle(Vector3 centerPosition, float radius, Vector3 colorNormalized)
    {
        InternalCalls.DrawWireCircle(centerPosition, radius, colorNormalized);
    }
    public static void DrawWireSphere(Vector3 centerPosition, float radius, Vector3 colorNormalized)
    {
        InternalCalls.DrawWireSphere(centerPosition, radius, colorNormalized);
    }
}