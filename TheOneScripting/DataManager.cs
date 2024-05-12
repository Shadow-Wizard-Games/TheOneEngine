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

    public static string AccessFileDataString(string filepath, IntPtr[] dataPath, int dataPathLenght, string dataName)
    {
        return InternalCalls.AccessFileDataString(filepath, dataPath, dataPathLenght, dataName);
    }

    public static int AccessFileDataInt(string filepath, IntPtr[] dataPath, int dataPathLenght, string dataName)
    {
        return InternalCalls.AccessFileDataInt(filepath, dataPath, dataPathLenght, dataName);
    }

    public static float AccessFileDataFloat(string filepath, IntPtr[] dataPath, int dataPathLenght, string dataName)
    {
        return InternalCalls.AccessFileDataFloat(filepath, dataPath, dataPathLenght, dataName);
    }

    public static bool AccessFileDataBool(string filepath, IntPtr[] dataPath, int dataPathLenght, string dataName)
    {
        return InternalCalls.AccessFileDataBool(filepath, dataPath, dataPathLenght, dataName);
    }

    public static void WriteFileDataString(string filepath, IntPtr[] dataPath, int dataPathLenght, string dataName, string data)
    {
        InternalCalls.WriteFileDataString(filepath, dataPath, dataPathLenght, dataName, data);
    }

    public static void WriteFileDataInt(string filepath, IntPtr[] dataPath, int dataPathLenght, string dataName, int data)
    {
        InternalCalls.WriteFileDataInt(filepath, dataPath, dataPathLenght, dataName, data);
    }

    public static void WriteFileDataFloat(string filepath, IntPtr[] dataPath, int dataPathLenght, string dataName, float data)
    {
        InternalCalls.WriteFileDataFloat(filepath, dataPath, dataPathLenght, dataName, data);
    }

    public static void WriteFileDataBool(string filepath, IntPtr[] dataPath, int dataPathLenght, string dataName, bool data)
    {
        InternalCalls.WriteFileDataBool(filepath, dataPath, dataPathLenght, dataName, data);
    }
}