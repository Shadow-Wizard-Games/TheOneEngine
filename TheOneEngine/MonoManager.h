#pragma once
#include "GameObject.h"
#include "DebugShape.h"

#include "mono\jit\jit.h"
#include "mono\metadata\assembly.h"
#include "mono\metadata\attrdefs.h"

#include <string>
#include <vector>

class MonoManager
{
    struct MonoManagerData
    {
        MonoDomain* monoRootDomain = nullptr;
        MonoDomain* monoAppDomain = nullptr;

        MonoAssembly* mainAssembly = nullptr;

        GameObject* currentGameObjectPtr = nullptr;
    };

private:

    static MonoManagerData monoData;

    static inline std::vector<std::string> functionsToIgnore = { "Start", "Update" };

public:
    MonoManager() {}
    ~MonoManager() {}

    void InitMono();
    void ShutDownMono();

    static GameObject* GetCurrentGameObjectPtr() { return monoData.currentGameObjectPtr; }
    static MonoObject* InstantiateClass(const char* className, GameObject* containerGOptr);

    static void CallScriptFunction(MonoObject* monoBehaviourInstance, std::string functionToCall);
    static void CallScriptFunction(MonoObject* monoBehaviourInstance, std::string functionToCall, void** params, int parameterCount);

    static bool IsClassInMainAssembly(const char* className);

    void RenderShapesQueue();

private:
    char* ReadBytes(const std::string& filepath, uint32_t* outSize);
    MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath);
    static MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className);

public:
    std::vector<DebugShape> debugShapesQueue;
};

