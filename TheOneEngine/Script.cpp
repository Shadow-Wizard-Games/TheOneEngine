#include "Script.h"
#include "MonoManager.h"
#include "GameObject.h"
#include "Camera.h"

#include "EngineCore.h"
#include "N_SceneManager.h"

Script::Script(std::shared_ptr<GameObject> containerGO, std::string name) : Component(containerGO, ComponentType::Script), scriptName(name)
{
	enabled = false;

	//Init things
	monoBehaviourInstance = MonoManager::InstantiateClass(name.c_str(), containerGO.get());
}

Script::Script(std::shared_ptr<GameObject> containerGO, Script* ref) : Component(containerGO, ComponentType::Script), scriptName(ref->scriptName)
{
	enabled = false;

	//Init things
	monoBehaviourInstance = MonoManager::InstantiateClass(ref->scriptName.c_str(), containerGO.get());
}

Script::~Script()
{
	//delete monoBehaviourInstance;
	monoBehaviourInstance = nullptr;
}

void Script::Start()
{
	MonoManager::CallScriptFunction(monoBehaviourInstance, "Start");
}

void Script::Enable()
{
	if (!enabled)
	{
		Start();
		enabled = true;
	}
}

void Script::Update()
{
	MonoManager::CallScriptFunction(monoBehaviourInstance, "Update");
}

json Script::SaveComponent()
{
	json scriptJSON;

	scriptJSON["Name"] = name;
	scriptJSON["Type"] = type;
	if (auto pGO = containerGO.lock())
	{
		scriptJSON["ParentUID"] = pGO.get()->GetUID();
	}
	scriptJSON["UID"] = UID;
	scriptJSON["ScriptName"] = scriptName;


	return scriptJSON;
}

void Script::LoadComponent(const json& scriptJSON)
{

	if (scriptJSON.contains("Name"))
	{
		name = scriptJSON["Name"];
	}

	if (scriptJSON.contains("Type"))
	{
		type = scriptJSON["Type"];
	}
	
	if (scriptJSON.contains("ScriptName"))
	{
		scriptName = scriptJSON["ScriptName"];
	}

	if (scriptJSON.contains("UID"))
	{
		UID = scriptJSON["UID"];
	}
}
