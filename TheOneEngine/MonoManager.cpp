#include "MonoManager.h"
#include "MonoRegisterer.h"
#include "Renderer2D.h"

#include <iostream>
#include <fstream>

std::string GetAssemblyPath()
{
    std::string resultingAssembly;

    // Check if the VisualStudioVersion environment variable is set

    char* vsVersion = nullptr;
    size_t len = 0;
    errno_t err = _dupenv_s(&vsVersion, &len, "VisualStudioVersion");

    if (err == 0 && vsVersion != nullptr)
    {
        // Running from Visual Studio
#ifdef _DEBUG

        resultingAssembly = "../TheOneScripting/bin/Debug/TheOneScripting.dll";

#else

        resultingAssembly = "../TheOneScripting/bin/Release/TheOneScripting.dll";

#endif
    }
    else
    {
        // Running from generated .exe file
        resultingAssembly = "TheOneScripting.dll";
    }

    free(vsVersion); // Remember to free the memory allocated by _dupenv_s
    return resultingAssembly;
}
std::string GetMonoAssembliesPath()
{
    std::string resultingPath;

    // Check if the VisualStudioVersion environment variable is set

    char* vsVersion = nullptr;
    size_t len = 0;
    errno_t err = _dupenv_s(&vsVersion, &len, "VisualStudioVersion");

    if (err == 0 && vsVersion != nullptr)
    {
        // Running from Visual Studio
        resultingPath = "../mono/lib/4.5";
    }
    else 
    {
        // Running from generated .exe file
        resultingPath = "mono/lib/4.5";
    }

    free(vsVersion); // Remember to free the memory allocated by _dupenv_s
    return resultingPath;
}

MonoManager::MonoManagerData MonoManager::monoData;

void MonoManager::InitMono()
{
    mono_set_assemblies_path(GetMonoAssembliesPath().c_str());

    MonoDomain* rootDomain = mono_jit_init("MyScriptRuntime");
	if (rootDomain == nullptr)
	{
        LOG(LogType::LOG_ERROR, "MonoDomain has not been initialised correctly.");
		return;
	}

	// Store the root domain pointer
	monoData.monoRootDomain = rootDomain;

	char appDomainName[] = "MyAppDomain";
    monoData.monoAppDomain = mono_domain_create_appdomain(appDomainName, nullptr);
	mono_domain_set(monoData.monoAppDomain, true);

    monoData.mainAssembly = LoadCSharpAssembly(GetAssemblyPath().c_str());

    MonoRegisterer::RegisterFunctions();
}

void MonoManager::ShutDownMono()
{
	mono_domain_set(mono_get_root_domain(), false);

	mono_domain_unload(monoData.monoAppDomain);
    monoData.monoAppDomain = nullptr;

	mono_jit_cleanup(monoData.monoRootDomain);
    monoData.monoRootDomain = nullptr;

    monoData.mainAssembly = nullptr;
}

MonoObject* MonoManager::InstantiateClass(const char* className, GameObject* containerGOptr)
{
    monoData.currentGameObjectPtr = containerGOptr;

    // Get a reference to the class we want to instantiate
    MonoClass* classToInstantiate = GetClassInAssembly(monoData.mainAssembly, "", className);

    if (classToInstantiate == nullptr)
    {
        LOG(LogType::LOG_WARNING, "Could not find a C# class named %s", className);
        return nullptr;
    }

    // Allocate an instance of our class
    MonoObject* classInstance = mono_object_new(monoData.monoAppDomain, classToInstantiate);

    if (classInstance == nullptr)
    {
        LOG(LogType::LOG_ERROR, "Could not create instance of C# class %s", className);
        return nullptr;
    }

    // Call the parameterless (default) constructor
    mono_runtime_object_init(classInstance);
    LOG(LogType::LOG_MONO, "Instance of %s created and initialized with GO name %s", className, containerGOptr->GetName().c_str());

    monoData.currentGameObjectPtr = nullptr;

    return classInstance;
}

void* MonoManager::CallScriptFunction(MonoObject* monoBehaviourInstance, std::string functionToCall)
{
    // Get the MonoClass pointer from the instance
    MonoClass* instanceClass = mono_object_get_class(monoBehaviourInstance);
    MonoClass* currentClass = instanceClass;

    MonoMethod* method = nullptr;

    // Traverse the class hierarchy to find the method
    while (currentClass != nullptr && method == nullptr)
    {
        // Get a reference to the method in the current class
        method = mono_class_get_method_from_name(currentClass, functionToCall.c_str(), 0);

        if (method == nullptr)
        {
            // Move to the parent class
            currentClass = mono_class_get_parent(currentClass);
        }
    }

    if (method == nullptr)
    {
        //Used for handling virtual functions errors
        for (auto& checkFunction : functionsToIgnore)
        {
            if (functionToCall == checkFunction) return nullptr;
        }

        LOG(LogType::LOG_ERROR, "Could not find method %s in %s", functionToCall.c_str(), mono_class_get_name(instanceClass));
        return nullptr;
    }

    // Call the C# method on the objectInstance instance, and get any potential exceptions
    MonoObject* exception = nullptr;
    MonoObject* returnResult = mono_runtime_invoke(method, monoBehaviourInstance, nullptr, &exception);
    void* functionToReturn = reinterpret_cast<void*>(returnResult);

    //Handle the exception
    if (exception != nullptr)
    {
        LOG(LogType::LOG_ERROR, "Exception occurred with %s from %s", functionToCall.c_str(), mono_class_get_name(instanceClass));
        return nullptr;
    }

    return functionToReturn;
}

void* MonoManager::CallScriptFunction(MonoObject* monoBehaviourInstance, std::string functionToCall, void* params[], int parameterCount)
{
    // Get the MonoClass pointer from the instance
    MonoClass* instanceClass = mono_object_get_class(monoBehaviourInstance);

    // Get a reference to the method in the class
    MonoMethod* method = mono_class_get_method_from_name(instanceClass, functionToCall.c_str(), parameterCount);

    if (method == nullptr)
    {
        // Get the parent class (MonoBehaviour's base class)
        MonoClass* parentClass = mono_class_get_parent(instanceClass);

        method = mono_class_get_method_from_name(parentClass, functionToCall.c_str(), 0);
    }

    if (method == nullptr)
    {
        LOG(LogType::LOG_ERROR, "Could not find method %s", functionToCall);
        return nullptr;
    }

    // Call the C# method on the objectInstance instance, and get any potential exceptions
    MonoObject* exception = nullptr;
    MonoObject* returnResult = mono_runtime_invoke(method, monoBehaviourInstance, params, &exception);
    void* functionToReturn = reinterpret_cast<void*>(returnResult);

    //Handle the exception
    if (exception != nullptr)
    {
        LOG(LogType::LOG_ERROR, "Exception occurred");
        mono_print_unhandled_exception(exception);
        return nullptr;
    }

    return functionToReturn;
}

bool MonoManager::IsClassInMainAssembly(const char* className)
{
    MonoImage* image = mono_assembly_get_image(monoData.mainAssembly);
    MonoClass* cSharpClass = mono_class_from_name(image, "", className);

    if (cSharpClass != nullptr) { return true; }

    return false;
}

void MonoManager::RenderShapesQueue()
{
    if (debugShapesQueue.empty())
        return;

    for (auto& shape : debugShapesQueue)
    {
        glm::mat4 translate = glm::translate(glm::mat4(1), shape.center);

        for (int i = 0; i < shape.points.size(); i++)
            Renderer2D::DrawLine(BT::WORLD, glm::vec4(shape.points[i], 1.0f) * translate,
                                 glm::vec4(&shape.points[i + 1] == nullptr ? shape.points[0] : shape.points[i + 1], 1.0f) * translate,
                                 glm::vec4(shape.color, 1.0f));
    }

    debugShapesQueue.clear();
}

char* MonoManager::ReadBytes(const std::string& filepath, uint32_t* outSize)
{
    std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

    if (!stream)
    {
        // Failed to open the file
        return nullptr;
    }

    std::streampos end = stream.tellg();
    stream.seekg(0, std::ios::beg);
    uint32_t size = end - stream.tellg();

    if (size == 0)
    {
        // File is empty
        return nullptr;
    }

    char* buffer = new char[size];
    stream.read((char*)buffer, size);
    stream.close();

    *outSize = size;
    return buffer;
}

MonoAssembly* MonoManager::LoadCSharpAssembly(const std::string& assemblyPath)
{
    uint32_t fileSize = 0;
    char* fileData = ReadBytes(assemblyPath, &fileSize);

    // NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
    MonoImageOpenStatus status;
    MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

    if (status != MONO_IMAGE_OK)
    {
        const char* errorMessage = mono_image_strerror(status);
        LOG(LogType::LOG_ERROR, "Mono image had an error when loading assembly. Error: %s", errorMessage);
        return nullptr;
    }

    MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
    mono_image_close(image);

    // Don't forget to free the file data
    delete[] fileData;

    return assembly;
}

MonoClass* MonoManager::GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className)
{
    MonoImage* image = mono_assembly_get_image(assembly);
    MonoClass* cSharpClass = mono_class_from_name(image, namespaceName, className);

    return cSharpClass;
}
