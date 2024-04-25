#include "N_SceneManager.h"
#include "EngineCore.h"
#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "Camera.h"
#include "Mesh.h"
#include "Texture.h"
#include "Collider2D.h"
#include "Listener.h"
#include "AudioSource.h"
#include "Canvas.h"
#include "ParticleSystem.h"
#include "../TheOneAudio/AudioCore.h"
#include "EngineCore.h"

#include <fstream>
#include <filesystem>
#include "ImageUI.h"

namespace fs = std::filesystem;

N_SceneManager::N_SceneManager() {}

N_SceneManager::~N_SceneManager() {}

bool N_SceneManager::Awake()
{
	fs::create_directories("Library/");
	return true;
}

bool N_SceneManager::Start()
{
	FindCameraInScene();

	loadingScreen = std::make_shared<GameObject>("loadingScreen");
	loadingScreen.get()->AddComponent<Transform>();
	loadingScreen.get()->AddComponent<Canvas>();
	loadingScreen.get()->GetComponent<Canvas>()->AddItemUI<ImageUI>("Assets/Textures/Hud/LoadingTxt.png");

	return true;
}

bool N_SceneManager::PreUpdate()
{
	if (sceneChange)
	{
		// Kiko - Here add the transition managing
		engine->collisionSolver->ClearCollisions();

		objectsToDelete.clear();

		if (previousFrameIsPlaying)
			LoadSceneFromJSON(currentScene->GetPath(), sceneChangeKeepGOs);
		else
			LoadSceneFromJSON(currentScene->GetPath());

		FindCameraInScene();
		currentScene->SetIsDirty(true);
		sceneChange = false;
		sceneChangeKeepGOs = false;
	}

	return true;
}

bool N_SceneManager::Update(double dt, bool isPlaying)
{
	// Do nothing

	// Save Scene by checking if isDirty and pressing CTRL+S
	//if (currentScene->IsDirty()) SaveScene();
	
	//this will be called when we click play
	if (!sceneIsPlaying && isPlaying)
	{
		RecursiveScriptInit(currentScene->GetRootSceneGO(), !previousFrameIsPlaying);
		//add game objects to collision solver vector
		engine->collisionSolver->LoadCollisions(currentScene->GetRootSceneGO());
	}

	if (!sceneChange)
		sceneIsPlaying = isPlaying;

	if (isPlaying)
	{
		currentScene->UpdateGOs(dt);
	}
	//this will be called when we click pause
	else if (previousFrameIsPlaying && !sceneIsPlaying)
	{
		//function to clear collision solver vector
		engine->collisionSolver->ClearCollisions();
	}

	previousFrameIsPlaying = isPlaying;

	return true;
}

bool N_SceneManager::PostUpdate()
{

	return true;
}

bool N_SceneManager::CleanUp()
{
	//delete currentScene->currentCamera;
	currentScene = nullptr;
	delete currentScene;

	return true;
}

void N_SceneManager::CreateNewScene(uint _index, std::string name)
{
	// Historn: Change currentScene when creating a new one? (First need save scene correctly for not loosing scenes)
	Scene* newScene = new Scene(_index, name);
	// Create with JSON Scene file
}

void N_SceneManager::LoadScene(uint index)
{

}

void N_SceneManager::LoadScene(std::string sceneName, bool keep)
{
	this->currentScene->SetPath("Assets/Scenes/" + sceneName + ".toe");
	this->sceneChange = true;
	sceneIsPlaying = false;
	sceneChangeKeepGOs = keep;
}

void N_SceneManager::SaveScene()
{
	//Change to save the Scene Class

	std::string fileNameExt = currentScene->GetSceneName() + ".toe";

	fs::path filename = fs::path(ASSETS_PATH) / "Scenes" / fileNameExt;
	//string filename = "Assets/Scenes/";
	fs::path folderName = fs::path(ASSETS_PATH) / "Scenes";
	fs::create_directories(folderName);

	json sceneJSON;

	sceneJSON["sceneName"] = currentScene->GetSceneName();
	sceneJSON["index"] = currentScene->GetIndex();
	sceneJSON["path"] = filename;

	json gameObjectsJSON;
	/*Save all gameobjects*/
	for (const auto& go : currentScene->GetRootSceneGO().get()->children)
	{
		gameObjectsJSON.push_back(go.get()->SaveGameObject());
	}

	sceneJSON["GameObjects"] = gameObjectsJSON;

	std::ofstream(filename) << sceneJSON.dump(2);
	LOG(LogType::LOG_OK, "SAVE SUCCESFUL");

	currentScene->SetIsDirty(false);
}

void N_SceneManager::LoadSceneFromJSON(const std::string& filename, bool keepGO)
{
	//Change to load the Scene Class

	// Check if the scene file exists
	if (!fs::exists(filename))
	{
		LOG(LogType::LOG_ERROR, "Scene file does not exist: {}", filename.data());
		return;
	}

	// Read the scene JSON from the file
	std::ifstream file(filename);
	if (!file.is_open())
	{
		LOG(LogType::LOG_ERROR, "Failed to open scene file: {}", filename.data());
		return;
	}

	json sceneJSON;
	try
	{
		file >> sceneJSON;

		// JULS: Audio Manager should delete this, but for now leave this commented
		//audioManager->DeleteAudioComponents();
	}
	catch (const json::parse_error& e)
	{
		LOG(LogType::LOG_ERROR, "Failed to parse scene JSON: {}", e.what());
		return;
	}

	// Close the file
	file.close();

	if (sceneJSON.contains("sceneName"))
	{
		currentScene->SetSceneName(sceneJSON["sceneName"]);
		currentScene->GetRootSceneGO().get()->SetName(sceneJSON["sceneName"]);
	}
	if (sceneJSON.contains("index"))
	{
		currentScene->SetIndex(sceneJSON["index"]);
	}
	if (sceneJSON.contains("path"))
	{
		currentScene->SetPath(sceneJSON["path"]);
	}

	if (keepGO)
	{
		for (auto it = currentScene->GetRootSceneGO()->children.begin(); it != currentScene->GetRootSceneGO()->children.end();)
		{
			if (!(*it)->isKeeped)
				it = currentScene->GetRootSceneGO().get()->children.erase(it);
			else
				++it;
		}
	}
	else { currentScene->GetRootSceneGO().get()->children.clear(); }

	// Load game objects from the JSON data
	if (sceneJSON.contains("GameObjects"))
	{
		const json& gameObjectsJSON = sceneJSON["GameObjects"];

		for (const auto& gameObjectJSON : gameObjectsJSON)
		{
			if (gameObjectJSON.contains("Keeped"))
			{
				if (gameObjectJSON["Keeped"] == true && previousFrameIsPlaying)
					continue;
			}
			
			// Create a new game object
			auto newGameObject = CreateEmptyGO();
			newGameObject.get()->SetName(currentScene->GetSceneName());
			// Load the game object from JSON
			newGameObject->LoadGameObject(gameObjectJSON);
		}

		LOG(LogType::LOG_OK, "LOAD SUCCESSFUL");
	}
	else
	{
		LOG(LogType::LOG_ERROR, "Scene file does not contain GameObjects information");
	}
}

void N_SceneManager::RecursiveScriptInit(std::shared_ptr<GameObject> go, bool firstInit)
{
	for (const auto gameObject : go.get()->children)
	{
		if (gameObject.get()->isKeeped && !firstInit)
			continue;

		gameObject.get()->GetAllComponents();
		for (const auto compGO : gameObject.get()->GetAllComponents())
		{
			if (compGO->GetType() == ComponentType::Script)
			{
				Script* script = dynamic_cast<Script*>(compGO);
				script->Start();
			}
		}

		RecursiveScriptInit(gameObject);
	}
}

std::string N_SceneManager::GenerateUniqueName(const std::string& baseName)
{
	std::string uniqueName = baseName;
	int counter = 1;

	while (std::any_of(
		currentScene->GetRootSceneGO().get()->children.begin(), currentScene->GetRootSceneGO().get()->children.end(),
		[&uniqueName](const std::shared_ptr<GameObject>& obj)
		{ return obj.get()->GetName() == uniqueName; }))
	{
		uniqueName = baseName + "(" + std::to_string(counter) + ")";
		++counter;
	}

	return uniqueName;
}

std::shared_ptr<GameObject> N_SceneManager::DuplicateGO(std::shared_ptr<GameObject> originalGO, bool recursive)
{
	GameObject* ref = originalGO.get();


	std::shared_ptr<GameObject> duplicatedGO = std::make_shared<GameObject>(recursive ? originalGO.get()->GetName() : GenerateUniqueName("Copy of " + originalGO.get()->GetName()));
	//meshGO.get()->GetComponent<Mesh>()->mesh = mesh;
	//meshGO.get()->GetComponent<Mesh>()->mesh.texture = textures[mesh.materialIndex];

	for (auto& item : ref->GetAllComponents())
	{
		switch (item->GetType())
		{
		case ComponentType::Transform:
			duplicatedGO.get()->AddCopiedComponent<Transform>((Transform*)item);
			break;
		case ComponentType::Camera:
			duplicatedGO.get()->AddCopiedComponent<Camera>((Camera*)item);
			break;
		case ComponentType::Mesh:
			duplicatedGO.get()->AddCopiedComponent<Mesh>((Mesh*)item);
			break;
		case ComponentType::Script:
			duplicatedGO.get()->AddCopiedComponent<Script>((Script*)item);
			break;
		case ComponentType::Collider2D:
			duplicatedGO.get()->AddCopiedComponent<Collider2D>((Collider2D*)item);
			break;
		case ComponentType::Canvas:
			duplicatedGO.get()->AddCopiedComponent<Canvas>((Canvas*)item);
			break;
		case ComponentType::Listener:
			duplicatedGO.get()->AddCopiedComponent<Listener>((Listener*)item);
			break;
		case ComponentType::AudioSource:
			duplicatedGO.get()->AddCopiedComponent<AudioSource>((AudioSource*)item);
			break;
		case ComponentType::ParticleSystem:
			duplicatedGO.get()->AddCopiedComponent<ParticleSystem>((ParticleSystem*)item);
			break;
		case ComponentType::Unknown:
			break;
		default:
			break;
		}
	}

	if (!recursive)
	{
		duplicatedGO.get()->parent = originalGO.get()->parent;

		originalGO.get()->parent.lock().get()->children.push_back(duplicatedGO);
		//GetRootSceneGO().get()->children.push_back(duplicatedGO);
	}


	for (auto& child : originalGO->children)
	{
		std::shared_ptr<GameObject> temp = DuplicateGO(child, true);
		temp.get()->parent = duplicatedGO;
		duplicatedGO.get()->children.push_back(temp);
	}

	if (originalGO.get()->IsPrefab())
	{
		duplicatedGO.get()->SetPrefab(originalGO.get()->GetPrefabID(), originalGO->GetPrefabName());
		duplicatedGO.get()->SetEditablePrefab(originalGO.get()->IsEditablePrefab());
		duplicatedGO.get()->SetPrefabDirty(originalGO.get()->IsPrefabDirty());
	}

	return duplicatedGO;
}

std::shared_ptr<GameObject> N_SceneManager::CreateEmptyGO(std::string name, bool isRoot)
{
	std::shared_ptr<GameObject> emptyGO = std::make_shared<GameObject>(name);
	emptyGO.get()->AddComponent<Transform>();

	if (isRoot)
	{
		emptyGO.get()->parent = currentScene->GetRootSceneGO().get()->weak_from_this();
		objectsToAdd.push_back(emptyGO);
	}


	if (!sceneIsPlaying) AddPendingGOs();
	return emptyGO;
}

void N_SceneManager::ReparentGO(std::shared_ptr<GameObject> originalGO, std::shared_ptr<GameObject> newParentGO)
{
	//find the go among children of his parent
	int counter = 0;
	for (const auto& go : originalGO.get()->parent.lock().get()->children)
	{
		if (go.get() == originalGO.get())
		{
			//we erase the element from the vector of his parent
			auto it = originalGO.get()->parent.lock().get()->children.begin() + counter;
			originalGO.get()->parent.lock().get()->children.erase(it);

			originalGO.get()->parent.lock().get()->children.push_back(newParentGO);
			//originalGO.get()->parent.lock().get()->children.insert(it, emptyGO);

			newParentGO.get()->parent = originalGO.get()->parent.lock();

			//and set the GO as his children
			newParentGO.get()->children.push_back(originalGO);

			//dont forget to set the new parent of the go
			originalGO.get()->parent = newParentGO;

			//if we find it, stop the searching loop
			break;
		}
		counter++;
	}
}

std::shared_ptr<GameObject> N_SceneManager::CreateCameraGO(std::string name)
{
	std::shared_ptr<GameObject> cameraGO = std::make_shared<GameObject>(name);
	cameraGO.get()->AddComponent<Transform>();
	cameraGO.get()->AddComponent<Camera>();
	cameraGO.get()->GetComponent<Camera>()->UpdateCamera();

	cameraGO.get()->parent = currentScene->GetRootSceneGO().get()->weak_from_this();

	currentScene->GetRootSceneGO().get()->children.emplace_back(cameraGO);

	return cameraGO;
}

std::shared_ptr<GameObject> N_SceneManager::CreateCanvasGO(std::string name)
{
	std::shared_ptr<GameObject> canvasGO = std::make_shared<GameObject>(name);
	canvasGO.get()->AddComponent<Transform>();
	canvasGO.get()->AddComponent<Canvas>();

	// Debug Img
	canvasGO.get()->GetComponent<Canvas>()->AddItemUI<ImageUI>();

	canvasGO.get()->parent = currentScene->GetRootSceneGO().get()->weak_from_this();

	currentScene->GetRootSceneGO().get()->children.emplace_back(canvasGO);

	return canvasGO;
}

void N_SceneManager::CreateMeshGO(std::string path)
{
	std::vector<ResourceId> meshesID = Resources::LoadMultiple<Model>(path);

	if (meshesID.empty())
		return;

	std::string name = path.substr(path.find_last_of("\\/") + 1, path.find_last_of('.') - path.find_last_of("\\/") - 1);

	name = GenerateUniqueName(name);

	// Create emptyGO parent if meshes > 1
	bool isSingleMesh = meshesID.size() > 1 ? false : true;
	std::shared_ptr<GameObject> emptyParent = isSingleMesh ? nullptr : CreateEmptyGO();
	if (!isSingleMesh) emptyParent.get()->SetName(name);

	std::vector<std::string> fileNames;

	for (auto& meshID : meshesID)
	{
		Model* mesh = Resources::GetResourceById<Model>(meshID);

		std::shared_ptr<GameObject> meshGO = std::make_shared<GameObject>(mesh->meshName);

		// Transform ---------------------------------
		meshGO.get()->AddComponent<Transform>();

		// Mesh  --------------------------------------
		meshGO.get()->AddComponent<Mesh>();

		meshGO.get()->GetComponent<Mesh>()->meshID = meshID;
		meshGO.get()->GetComponent<Mesh>()->materialID = Resources::LoadFromLibrary<Material>(mesh->GetMaterialPath());

		//Load MeshData from custom files
		for (const auto& file : fileNames)
		{
			std::string fileName = file.substr(file.find_last_of("\\/") + 1, file.find_last_of('.') - file.find_last_of("\\/") - 1);
			if (fileName == mesh->meshName)
			{
				meshGO.get()->GetComponent<Transform>()->SetTransform(mesh->meshTransform);
			}
		}

		// AABB
		meshGO.get()->GenerateAABBFromMesh();

		if (isSingleMesh)
		{
			meshGO.get()->parent = currentScene->GetRootSceneGO();
			engine->N_sceneManager->objectsToAdd.push_back(meshGO);
		}
		else
		{
			meshGO.get()->parent = emptyParent;
			emptyParent.get()->children.push_back(meshGO);
		}
	}

	if (!sceneIsPlaying) AddPendingGOs();
}

void N_SceneManager::CreateExistingMeshGO(std::string path)
{
	std::string fbxName = path.substr(path.find_last_of("\\/") + 1, path.find_last_of('.') - path.find_last_of("\\/") - 1);
	std::string folderName = Resources::PathToLibrary<Model>(fbxName);

	std::vector<std::string> fileNames = Resources::GetAllFilesFromFolder(folderName, ".mesh", ".animator");

	if (fileNames.empty())
		CreateMeshGO(path);
	else
	{
		std::string name = fbxName;
		name = GenerateUniqueName(name);

		// Create emptyGO parent if meshes >1
		bool isSingleMesh = fileNames.size() > 1 ? false : true;
		std::shared_ptr<GameObject> emptyParent = isSingleMesh ? nullptr : CreateEmptyGO();
		if (!isSingleMesh) emptyParent.get()->SetName(name);

		for (const auto& file : fileNames)
		{
			ResourceId meshID = Resources::LoadFromLibrary<Model>(file);
			Model* mesh = Resources::GetResourceById<Model>(meshID);

			std::shared_ptr<GameObject> meshGO = std::make_shared<GameObject>(mesh->meshName);
			meshGO.get()->AddComponent<Transform>();
			meshGO.get()->GetComponent<Transform>()->SetTransform(mesh->meshTransform);
			meshGO.get()->AddComponent<Mesh>();

			meshGO.get()->GetComponent<Mesh>()->meshID = meshID;
			meshGO.get()->GetComponent<Mesh>()->materialID = Resources::LoadFromLibrary<Material>(mesh->GetMaterialPath());

			meshGO.get()->GenerateAABBFromMesh();

			if (isSingleMesh)
			{
				meshGO.get()->parent = currentScene->GetRootSceneGO();
				currentScene->GetRootSceneGO().get()->children.push_back(meshGO);
			}
			else
			{
				meshGO.get()->parent = emptyParent;
				emptyParent.get()->children.push_back(meshGO);
			}
		}
	}
}

std::shared_ptr<GameObject> N_SceneManager::CreateCube()
{
	std::shared_ptr<GameObject> cubeGO = std::make_shared<GameObject>("Cube");
	cubeGO.get()->AddComponent<Transform>();
	cubeGO.get()->AddComponent<Mesh>();

	cubeGO.get()->parent = currentScene->GetRootSceneGO().get()->weak_from_this();

	currentScene->GetRootSceneGO().get()->children.emplace_back(cubeGO);

	return cubeGO;
}

std::shared_ptr<GameObject> N_SceneManager::CreateSphere()
{
	std::shared_ptr<GameObject> sphereGO = std::make_shared<GameObject>("Sphere");
	sphereGO.get()->AddComponent<Transform>();
	sphereGO.get()->AddComponent<Mesh>();

	sphereGO.get()->parent = currentScene->GetRootSceneGO().get()->weak_from_this();

	currentScene->GetRootSceneGO().get()->children.emplace_back(sphereGO);

	return nullptr;
}

void N_SceneManager::CreateMF()
{
	CreateMeshGO("Assets/Meshes/mf.fbx");
}

void N_SceneManager::CreateTeapot()
{
	CreateMeshGO("Assets/Meshes/teapot.fbx");
}

void N_SceneManager::AddPendingGOs()
{
	for (const auto& objToAdd : engine->N_sceneManager->objectsToAdd)
	{
		engine->N_sceneManager->currentScene->GetRootSceneGO().get()->children.push_back(objToAdd);
	}

	engine->N_sceneManager->objectsToAdd.clear();
}

void N_SceneManager::DeletePendingGOs()
{
	for (auto object : objectsToDelete)
		object->Delete();

	objectsToDelete.clear();
}

void N_SceneManager::OverrideScenePrefabs(uint32_t prefabID)
{
	for (auto& child : currentScene->GetRootSceneGO()->children)
	{
		if (child->GetPrefabID() == prefabID)
		{
			OverrideGameobjectFromPrefab(child);
		}

		OverridePrefabsRecursive(child, prefabID);
	}
}

void N_SceneManager::OverridePrefabsRecursive(std::shared_ptr<GameObject> parent, uint32_t prefabID)
{
	for (auto& child : parent->children)
	{
		if (child->GetPrefabID() == prefabID)
		{
			OverrideGameobjectFromPrefab(child);
		}

		OverridePrefabsRecursive(child, prefabID);
	}
}

void N_SceneManager::OverrideGameobjectFromPrefab(std::shared_ptr<GameObject> goToModify)
{
	fs::path filename = ASSETS_PATH;
	filename += "Prefabs\\" + goToModify->GetPrefabName() + ".prefab";

	std::ifstream file(filename);
	if (!file.is_open())
	{
		LOG(LogType::LOG_ERROR, "Failed to open prefab file: {}", filename);
		return;
	}

	json prefabJSON;
	try
	{
		file >> prefabJSON;
	}
	catch (const json::parse_error& e)
	{
		LOG(LogType::LOG_ERROR, "Failed to parse prefab JSON: {}", e.what());
		return;
	}

	// Close the file
	file.close();

	if (prefabJSON.contains("Components")) {

		// particles, camera, collider, --canvas--
		const json& componentsJSON = prefabJSON["Components"];

		for (const auto& componentJSON : componentsJSON)
		{
			ComponentType type = static_cast<ComponentType>(componentJSON["Type"]);

			switch (type)
			{
			case ComponentType::Collider2D:
			{
				if (goToModify->GetComponent<Collider2D>() == nullptr) goToModify->AddComponent<Collider2D>();

				auto collider = goToModify->GetComponent<Collider2D>();
				collider->LoadComponent(componentJSON);
				break;
			}
			case ComponentType::Camera:
			{
				if (goToModify->GetComponent<Camera>() == nullptr) goToModify->AddComponent<Camera>();

				auto camera = goToModify->GetComponent<Camera>();
				camera->LoadComponent(componentJSON);
				break;
			}
			case ComponentType::Canvas:
			{
				if (goToModify->GetComponent<Canvas>() == nullptr) goToModify->AddComponent<Canvas>();

				auto canvas = goToModify->GetComponent<Canvas>();
				canvas->LoadComponent(componentJSON);
				break;
			}
			case ComponentType::AudioSource:
			{
				if (goToModify->GetComponent<AudioSource>() == nullptr) goToModify->AddComponent<AudioSource>();

				auto audioSource = goToModify->GetComponent<AudioSource>();
				audioSource->LoadComponent(componentJSON);
				break;
			}
			case ComponentType::Listener:
			{
				if (goToModify->GetComponent<Listener>() == nullptr) goToModify->AddComponent<Listener>();

				auto listener = goToModify->GetComponent<Listener>();
				listener->LoadComponent(componentJSON);
				break;
			}
			case ComponentType::ParticleSystem:
			{
				if (goToModify->GetComponent<ParticleSystem>() == nullptr) goToModify->AddComponent<ParticleSystem>();

				auto particleSystem = goToModify->GetComponent<ParticleSystem>();
				particleSystem->LoadComponent(componentJSON);
				break;
			}
			default:
				break;
			}
		}
	}

	// Update other prefab instances children
	/*
	if (prefabJSON.contains("GameObjects"))
	{
		const json& childrenJSON = prefabJSON["GameObjects"];

		for (const auto& childJSON : childrenJSON)
		{
			if(childJSON["UID"] == 0)
			{

			}
		}
	}
	*/
}

void N_SceneManager::CreatePrefabFromFile(std::string prefabName, const vec3f& position)
{
	auto newGameObject = CreateEmptyGO();
	newGameObject.get()->SetName(currentScene->GetSceneName());

	// Load the game object from 
	fs::path filename = ASSETS_PATH;
	filename += "Prefabs\\" + prefabName + ".prefab";

	std::ifstream file(filename);
	if (!file.is_open())
	{
		LOG(LogType::LOG_ERROR, "Failed to open prefab file: {}", filename);
		return;
	}

	json prefabJSON;
	try
	{
		file >> prefabJSON;
	}
	catch (const json::parse_error& e)
	{
		LOG(LogType::LOG_ERROR, "Failed to parse prefab JSON: {}", e.what());
		return;
	}

	// Close the file
	file.close();

	newGameObject->LoadGameObject(prefabJSON);

	newGameObject->GetComponent<Transform>()->SetPosition(position);
}

void N_SceneManager::CreatePrefabFromPath(std::string prefabPath, const vec3f& position)
{
	auto newGameObject = CreateEmptyGO();
	newGameObject.get()->SetName(currentScene->GetSceneName());

	std::ifstream file(prefabPath);
	if (!file.is_open())
	{
		LOG(LogType::LOG_ERROR, "Failed to open prefab file: {}", prefabPath);
		return;
	}

	json prefabJSON;
	try
	{
		file >> prefabJSON;
	}
	catch (const json::parse_error& e)
	{
		LOG(LogType::LOG_ERROR, "Failed to parse prefab JSON: {}", e.what());
		return;
	}

	// Close the file
	file.close();

	newGameObject->LoadGameObject(prefabJSON);

	newGameObject->GetComponent<Transform>()->SetPosition(position);
}

uint N_SceneManager::GetNumberGO() const
{
	return static_cast<uint>(currentScene->GetRootSceneGO().get()->children.size());
}

std::vector<std::shared_ptr<GameObject>> N_SceneManager::GetGameObjects()
{
	return currentScene->GetRootSceneGO().get()->children;
}

void N_SceneManager::SetSelectedGO(std::shared_ptr<GameObject> gameObj)
{
	selectedGameObject = gameObj;
}

void N_SceneManager::FindCameraInScene()
{
	for (const auto& GO : GetGameObjects())
	{
		if (GO->HasCameraComponent())
		{
			currentScene->currentCamera = GO->GetComponent<Camera>();
			break;
		}
	}
}

std::shared_ptr<GameObject> N_SceneManager::GetSelectedGO() const
{
	return selectedGameObject;
}

void Scene::ChangePrimaryCamera(GameObject* newPrimaryCam)
{
	for (const auto& gameCam : rootSceneGO->children)
	{
		if (gameCam->GetComponent<Camera>() == nullptr) continue;

		if (gameCam.get() != newPrimaryCam && gameCam->GetComponent<Camera>()->primaryCam)
			gameCam->GetComponent<Camera>()->primaryCam = false;
	}
	newPrimaryCam->GetComponent<Camera>()->primaryCam = true;

	currentCamera = newPrimaryCam->GetComponent<Camera>();
}

void Scene::RecurseSceneDraw(std::shared_ptr<GameObject> parentGO, Camera* cam)
{
	if (cam != nullptr) {
		for (const auto& gameObject : parentGO.get()->children)
		{
			float distance = glm::length((vec3)gameObject->GetComponent<Transform>()->CalculateWorldTransform()[3] - cam->GetContainerGO()->GetComponent<Transform>()->GetPosition());
			zSorting.insert(std::pair<float, GameObject*>(distance, gameObject.get()));
			RecurseSceneDraw(gameObject, cam);
		}

	}
	else {
		for (const auto& gameObject : parentGO.get()->children)
		{
			float distance = glm::length((vec3)gameObject->GetComponent<Transform>()->CalculateWorldTransform()[3]);
			zSorting.insert(std::pair<float, GameObject*>(distance, gameObject.get()));
			RecurseSceneDraw(gameObject);
		}

	}
}

void Scene::UpdateGOs(double dt)
{
	engine->N_sceneManager->AddPendingGOs();

	for (const auto& gameObject : rootSceneGO->children)
	{
		gameObject->Update(dt);
	}

	engine->N_sceneManager->DeletePendingGOs();
}

void Scene::RecurseUIDraw(std::shared_ptr<GameObject> parentGO, DrawMode mode)
{
	for (const auto& gameObject : parentGO.get()->children)
	{
		gameObject.get()->DrawUI(currentCamera, mode);
		RecurseUIDraw(gameObject, mode);
	}
}

void Scene::Draw(DrawMode mode, Camera* cam)
{
	zSorting.clear();

	RecurseSceneDraw(rootSceneGO, cam);

	if (cam != nullptr) {
		for (auto i = zSorting.rbegin(); i != zSorting.rend(); ++i) {
			i->second->Draw(cam);
		}
	}
	else {
		for (auto i = zSorting.rbegin(); i != zSorting.rend(); ++i) {
			i->second->Draw(currentCamera);
		}
	}

	RecurseUIDraw(rootSceneGO, mode);
}