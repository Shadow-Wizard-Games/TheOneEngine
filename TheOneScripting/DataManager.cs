using System;
using System.Runtime.CompilerServices;

public class DataManager
{

    public static void SaveGame()
    {
        InternalCalls.CreateSaveFromScene("GameData/", "LastSave_" + InternalCalls.GetCurrentSceneName());
    }

    public static void RemoveFile(string filepath)
    {
        InternalCalls.RemoveFile(filepath);
    }

    public static string AccessFileDataString(string filepath, string[] dataPath, string dataName)
    {
        return InternalCalls.AccessFileDataString(filepath, dataPath, dataName);
    }

    public static int AccessFileDataInt(string filepath, string[] dataPath, string dataName)
    {
        return InternalCalls.AccessFileDataInt(filepath, dataPath, dataName);
    }

    public static float AccessFileDataFloat(string filepath, string[] dataPath, string dataName)
    {
        return InternalCalls.AccessFileDataFloat(filepath, dataPath, dataName);
    }

    public static bool AccessFileDataBool(string filepath, string[] dataPath, string dataName)
    {
        return InternalCalls.AccessFileDataBool(filepath, dataPath, dataName);
    }

    public static void WriteFileDataString(string filepath, string[] dataPath, string dataName, string data)
    {
        InternalCalls.WriteFileDataString(filepath, dataPath, dataName, data);
    }

    public static void WriteFileDataInt(string filepath, string[] dataPath, string dataName, int data)
    {
        InternalCalls.WriteFileDataInt(filepath, dataPath, dataName, data);
    }

    public static void WriteFileDataFloat(string filepath, string[] dataPath, string dataName, float data)
    {
        InternalCalls.WriteFileDataFloat(filepath, dataPath, dataName, data);
    }

    public static void WriteFileDataBool(string filepath, string[] dataPath, string dataName, bool data)
    {
        InternalCalls.WriteFileDataBool(filepath, dataPath, dataName, data);
    }

    public static bool IsValidEnumValue<T>(int value) where T : Enum
    {
        return Enum.IsDefined(typeof(T), value);
    }
}