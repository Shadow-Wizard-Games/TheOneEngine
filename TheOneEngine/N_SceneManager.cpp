#include "N_SceneManager.h"
#include "EngineCore.h"
#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "Camera.h"
#include "Mesh.h"
#include "Texture.h"
#include "Script.h"
#include "Collider2D.h"
#include "Listener.h"
#include "AudioSource.h"
#include "Canvas.h"
#include "ParticleSystem.h"
#include "ImageUI.h"
#include "Renderer.h"
#include "Renderer2D.h"
#include "Renderer3D.h"

#include "TheOneAudio/AudioCore.h"

#include <fstream>
#include <filesystem>
#include "ImageUI.h"
#include "TextUI.h"
#include <unordered_set>

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
	if (sceneChange && !sceneIsPlaying)
	{
		// Kiko - Here add the transition managing
		engine->collisionSolver->ClearCollisions();

		objectsToDelete.clear();

		if (previousFrameIsPlaying)
			LoadSceneFromJSON(currentScene->GetPath(), sceneChangeKeepGOs);
		else
			LoadSceneFromJSON(currentScene->GetPath());

		engine->collisionSolver->LoadCollisions(currentScene->GetRootSceneGO());

		FindCameraInScene();

		currentScene->SetIsDirty(true);
		sceneChangeKeepGOs = false;
		if (!previousFrameIsPlaying)
			sceneChange = false;
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
		RecursiveScriptInit(currentScene->GetRootSceneGO(), !sceneChange);
	}

	previousFrameIsPlaying = sceneIsPlaying;
	sceneIsPlaying = isPlaying;

	if (sceneIsPlaying && !sceneChange)
		currentScene->UpdateGOs(dt);
	else if (previousFrameIsPlaying && sceneIsPlaying && sceneChange)
		sceneChange = false;

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

void N_SceneManager::LoadSave(std::string sceneName, std::string scenePath)
{
	this->currentScene->SetPath(scenePath + sceneName + ".toe");

	if (!fs::exists(this->currentScene->GetPath()))
	{
		this->currentScene->SetPath("Assets/Scenes/" + sceneName + ".toe");
	}

	this->sceneChange = true;
	sceneIsPlaying = false;
	sceneChangeKeepGOs = true;
}

void N_SceneManager::SaveScene(std::string directories, std::string sceneName)
{
	//Change to save the Scene Class

	std::string fileNameExt = sceneName + ".toe";

	fs::path filename = fs::path(directories) / fileNameExt;
	//string filename = "Assets/Scenes/";
	fs::path folderName = fs::path(directories);
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
	LOG(LogType::LOG_OK, (currentScene->GetSceneName() + " SAVE SUCCESFUL at " + filename.string()).c_str());

	currentScene->SetIsDirty(false);
}

void N_SceneManager::LoadSceneFromJSON(const std::string& filename, bool keepGO)
{
	//Change to load the Scene Class

	// Check if the scene file exists
	if (!fs::exists(filename))
	{
		LOG(LogType::LOG_ERROR, "Scene file does not exist: %s", filename.data());
		return;
	}

	// Read the scene JSON from the file
	std::ifstream file(filename);
	if (!file.is_open())
	{
		LOG(LogType::LOG_ERROR, "Failed to open scene file: %s", filename.data());
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
		LOG(LogType::LOG_ERROR, "Failed to parse scene JSON: %s", e.what());
		return;
	}

	// Close the file
	file.close();

	//clear lights
	Renderer3D::CleanLights();

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
				if (gameObjectJSON["Keeped"] == true && keepGO)
					continue;
			}

			if (gameObjectJSON.contains("PrefabID") && gameObjectJSON["PrefabID"] != 0)
			{
				if (gameObjectJSON.contains("Components"))
				{
					mat4 transformationMatrix;
					for (auto& componentJSON : gameObjectJSON["Components"])
					{
						if (componentJSON.contains("Name") && componentJSON["Name"] == "Transform")
						{
							int it = 0;
							for (int i = 0; i < 4; i++) {
								for (int j = 0; j < 4; j++) {
									if (componentJSON.contains("Transformation Matrix"))
									{
										transformationMatrix[i][j] = componentJSON["Transformation Matrix"][it];
										it++;
									}
								}
							}
						}
					}

					CreatePrefabWithName(gameObjectJSON["PrefabName"], transformationMatrix);

					LOG(LogType::LOG_INFO, "Loaded prefab from prefab file instead of scene file");
				}
				else
				{
					LOG(LogType::LOG_ERROR, "Prefab does not contain Components in its file. File might be damaged.");
				}
			}
			else
			{
				// Create a new game object
				auto newGameObject = CreateEmptyGO();
				newGameObject.get()->SetName(currentScene->GetSceneName());
				// Load the game object from JSON
				newGameObject->LoadGameObject(gameObjectJSON);
			}


			LOG(LogType::LOG_OK, "LOAD SUCCESSFUL");
		}
	}
	else
	{
		LOG(LogType::LOG_ERROR, "Scene file does not contain GameObjects information");
	}
}

void N_SceneManager::RemoveFile(std::string pathToDelete)
{
	fs::path pathObj(pathToDelete);

	try {
		if (fs::exists(pathObj)) {
			if (fs::is_regular_file(pathObj)) {
				fs::remove(pathObj);
			}
			else if (fs::is_directory(pathObj)) {
				fs::remove_all(pathObj);
			}
			LOG(LogType::LOG_OK, "Successfully deleted: %s", pathToDelete.data());
		}
		else
			LOG(LogType::LOG_WARNING, "Path does not exist: %s", pathToDelete.data());
	}
	catch (const fs::filesystem_error& e) {
		LOG(LogType::LOG_ERROR, "Error deleting %s", e.what());
	}
}

void* N_SceneManager::AccessFileDataRead(std::string filepath, DataType dataType, std::vector<std::string> dataPath, std::string dataName)
{
	if (!fs::exists(filepath))
	{
		LOG(LogType::LOG_ERROR, "File does not exist: %s", filepath.data());
		return nullptr;
	}

	std::ifstream file(filepath);
	if (!file.is_open())
	{
		LOG(LogType::LOG_ERROR, "Failed to open file: %s", filepath.data());
		return nullptr;
	}

	json jsonData;
	try
	{
		file >> jsonData;
	}
	catch (const std::exception& e)
	{
		LOG(LogType::LOG_ERROR, "Failed to parse JSON: %s", e.what());
		return nullptr;
	}

	file.close();

	for (const auto& path : dataPath)
	{
		if (!jsonData[path].is_object())
		{
			LOG(LogType::LOG_WARNING, "Object path not found in json: %s", path.data());
			return nullptr;
		}
		jsonData = jsonData[path];
	}

	if (!jsonData.contains(dataName))
	{
		LOG(LogType::LOG_WARNING, "Data not found in json object: %s", dataName.data());
		return nullptr;
	}

	switch (dataType)
	{
	case DATA_BOOL:
		return new bool(jsonData[dataName]);
		break;
	case DATA_STRING:
		return new std::string(jsonData[dataName]);
		break;
	case DATA_INT:
		return new int(jsonData[dataName]);
		break;
	case DATA_FLOAT:
		return new float(jsonData[dataName]);
		break;
	default:
		break;
	}

	return nullptr;
}

void N_SceneManager::AccessFileDataWrite(std::string filepath, DataType dataType, std::vector<std::string> dataPath, std::string dataName, void* data)
{
	if (!fs::exists(filepath))
	{
		LOG(LogType::LOG_INFO, "File does not exist, created file: %s", filepath.data());

		fs::path directory = fs::path(filepath).parent_path();
		if (!fs::exists(directory))
		{
			// Create directories if they don't exist
			fs::create_directories(directory);
		}

		std::ofstream file(filepath);
		if (!file.is_open())
		{
			LOG(LogType::LOG_ERROR, "Failed to open file: %s", filepath.data());
			return;
		}

		json jsonData;

		json* currentObject = &jsonData;
		for (const auto& path : dataPath)
		{
			(*currentObject)[path] = json::object();
			currentObject = &(*currentObject)[path];
		}

		switch (dataType)
		{
		case DATA_BOOL:
			(*currentObject)[dataName] = *reinterpret_cast<bool*>(data);
			break;
		case DATA_STRING:
			(*currentObject)[dataName] = *static_cast<std::string*>(data);
			break;
		case DATA_INT:
			(*currentObject)[dataName] = *reinterpret_cast<int*>(data);
			break;
		case DATA_FLOAT:
			(*currentObject)[dataName] = *reinterpret_cast<float*>(data);
			break;
		default:
			break;
		}

		file << std::setw(4) << jsonData;
		file.close();
	}
	else
	{
		std::ifstream file(filepath);
		if (!file.is_open())
		{
			LOG(LogType::LOG_ERROR, "Failed to open file: %s", filepath.data());
			return;
		}

		json fileJSON;

		try
		{
			file >> fileJSON;
		}
		catch (const std::exception& e)
		{
			LOG(LogType::LOG_ERROR, "Failed to parse json file: %s",e.what());
			file.close();
			return;
		}

		file.close();

		file.open(filepath);
		if (!file.is_open())
		{
			LOG(LogType::LOG_ERROR, "Failed to reopen file: %s", filepath.data());
			return;
		}

		json* currentObject = &fileJSON;
		for (const auto& path : dataPath)
		{
			if (!currentObject->contains(path))
				(*currentObject)[path] = json::object();

			currentObject = &(*currentObject)[path];
		}

		switch (dataType)
		{
		case DATA_BOOL:
			(*currentObject)[dataName] = *reinterpret_cast<bool*>(data);
			break;
		case DATA_STRING:
			(*currentObject)[dataName] = *static_cast<std::string*>(data);
			break;
		case DATA_INT:
			(*currentObject)[dataName] = *reinterpret_cast<int*>(data);
			break;
		case DATA_FLOAT:
			(*currentObject)[dataName] = *reinterpret_cast<float*>(data);
			break;
		default:
			break;
		}

		std::ofstream outFile(filepath);
		if (!outFile.is_open())
		{
			LOG(LogType::LOG_ERROR, "Failed to open file to save the data: %s", filepath.data());
			return;
		}

		outFile << std::setw(4) << fileJSON;
		outFile.close();
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

std::string N_SceneManager::GenerateUniqueName(const std::string& baseName, const GameObject* go)
{
	std::unordered_set<std::string> existingNames;
	std::string newName = baseName;
	std::vector<std::shared_ptr<GameObject>> children;
	children = go ? go->parent.lock()->children : currentScene->GetRootSceneGO()->children;

	for (const auto& child : children)
		existingNames.insert(child->GetName());

	if (existingNames.count(newName) > 0)
	{
		int count = 1;
		while (existingNames.count(newName) > 0)
		{
			// Check if the name already ends with a number in parentheses
			size_t pos = newName.find_last_of('(');
			if (pos != std::string::npos && newName.back() == ')' && newName[pos] == '(' && pos > 0)
			{
				// Extract the number, increment it, and update the newName
				int num = std::stoi(newName.substr(pos + 1, newName.size() - pos - 2));
				newName = newName.substr(0, pos - 1) + " (" + std::to_string(++num) + ")";
			}
			else
			{
				newName = baseName + " (" + std::to_string(count++) + ")";
			}
		}
	}

	return newName;
}

std::shared_ptr<GameObject> N_SceneManager::DuplicateGO(std::shared_ptr<GameObject> originalGO, bool recursive)
{
	GameObject* ref = originalGO.get();

	std::shared_ptr<GameObject> duplicatedGO = std::make_shared<GameObject>(recursive ? originalGO.get()->GetName() : GenerateUniqueName(originalGO.get()->GetName(), ref));
	//meshGO.get()->GetComponent<Mesh>()->mesh = mesh;
	//meshGO.get()->GetComponent<Mesh>()->mesh.texture = textures[mesh.materialIndex];
	duplicatedGO->hasTransparency = ref->hasTransparency;

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
		case ComponentType::Light:
			duplicatedGO.get()->AddCopiedComponent<Light>((Light*)item);
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
	return currentScene->GetRootSceneGO().get()->children.emplace_back(cameraGO);
}

std::shared_ptr<GameObject> N_SceneManager::CreateCanvasGO(std::string name)
{
	std::shared_ptr<GameObject> canvasGO = std::make_shared<GameObject>(name);
	canvasGO.get()->AddComponent<Transform>();
	canvasGO.get()->AddComponent<Canvas>();

	// Debug Img
	//canvasGO.get()->GetComponent<Canvas>()->AddItemUI<ImageUI>();

	canvasGO.get()->GetComponent<Canvas>()->AddItemUI<TextUI>("Assets/Fonts/ComicSansMS.ttf");

	canvasGO.get()->parent = currentScene->GetRootSceneGO().get()->weak_from_this();
	currentScene->GetRootSceneGO().get()->children.emplace_back(canvasGO);
	return canvasGO;
}

std::shared_ptr<GameObject> N_SceneManager::CreateLightGO(LightType type)
{
	std::string name;

	switch (type)
	{
		case Directional: name = "Directional Light"; break;
		case Point: name = "Point Light"; break;
		case Spot: name = "Spot Light"; break;
		case Area: name = "Area Light"; break;
		default: name = "error_type"; break;
	}

	std::shared_ptr<GameObject> lightGO = std::make_shared<GameObject>(name);
	lightGO.get()->AddComponent<Transform>();
	lightGO.get()->AddComponent<Light>(type);

	lightGO.get()->parent = currentScene->GetRootSceneGO().get()->weak_from_this();
	
	return currentScene->GetRootSceneGO().get()->children.emplace_back(lightGO);
}

void N_SceneManager::CreateMeshGO(std::string path)
{
	switch (FBXIMPORTER::FBXtype(path))
	{
	case MeshType::DEFAULT:
	{
		std::vector<ResourceId> meshesID = Resources::LoadMultiple<Model>(path);
		if (meshesID.empty())
			return;

		std::string name = path.substr(path.find_last_of("\\/") + 1, path.find_last_of('.') - path.find_last_of("\\/") - 1);
		name = GenerateUniqueName(name);

		bool isSingleMesh = meshesID.size() > 1 ? false : true;
		std::shared_ptr<GameObject> emptyParent = isSingleMesh ? nullptr : CreateEmptyGO();
		if (!isSingleMesh) emptyParent.get()->SetName(name);

		for (auto& meshID : meshesID)
			CreateDefaultMeshGO(meshID, emptyParent, isSingleMesh);
	}
		break;
	case MeshType::SKELETAL:
		CreateSkeletalMeshGO(Resources::Load<SkeletalModel>(path), nullptr, true);
		break;
	default:
		break;
	}

	if (!sceneIsPlaying) AddPendingGOs();
}

void N_SceneManager::CreateExistingMeshGO(std::string path)
{
	std::string fbxName = path.substr(path.find_last_of("\\/") + 1, path.find_last_of('.') - path.find_last_of("\\/") - 1);
	std::string folderName = Resources::PathToLibrary<Model>(fbxName);

	std::vector<std::string> fileNames = Resources::GetAllFilesFromFolder(folderName, ".mesh", ".animator");
	if (fileNames.empty())
		return;

	std::string name = fbxName;
	name = GenerateUniqueName(name);

	// Create emptyGO parent if meshes >1
	bool isSingleMesh = fileNames.size() > 1 ? false : true;
	std::shared_ptr<GameObject> emptyParent = isSingleMesh ? nullptr : CreateEmptyGO();
	if (!isSingleMesh) emptyParent.get()->SetName(name);

	MeshType type = FBXIMPORTER::FBXtype(path);
	for (const auto& file : fileNames)
	{
		switch (type)
		{
		case MeshType::DEFAULT:
			CreateDefaultMeshGO(Resources::LoadFromLibrary<Model>(file), emptyParent, isSingleMesh);
			break;
		case MeshType::SKELETAL:
			CreateSkeletalMeshGO(Resources::LoadFromLibrary<SkeletalModel>(file), nullptr, true);
			if (!sceneIsPlaying) AddPendingGOs();
			return;
		default:
			break;
		}
	}

	if (!sceneIsPlaying) AddPendingGOs();
}

void N_SceneManager::CreateDefaultMeshGO(ResourceId meshID, std::shared_ptr<GameObject> emptyParent, bool isSingle)
{
	Model* mesh = Resources::GetResourceById<Model>(meshID);

	std::shared_ptr<GameObject> meshGO = std::make_shared<GameObject>(mesh->GetMeshName());
	meshGO.get()->AddComponent<Transform>();
	meshGO.get()->GetComponent<Transform>()->SetTransform(mesh->GetMeshTransform());
	meshGO.get()->SetAABBox(mesh->GetMeshAABB());
	meshGO.get()->AddComponent<Mesh>();

	meshGO.get()->GetComponent<Mesh>()->meshID = meshID;
	meshGO.get()->GetComponent<Mesh>()->materialID = Resources::LoadFromLibrary<Material>(mesh->GetMaterialPath());
	meshGO.get()->GetComponent<Mesh>()->meshType = MeshType::DEFAULT;

	Renderer3D::AddMesh(mesh->GetMeshID(), meshGO.get()->GetComponent<Mesh>()->materialID);

	if (isSingle)
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

void N_SceneManager::CreateSkeletalMeshGO(ResourceId meshID, std::shared_ptr<GameObject> emptyParent, bool isSingle)
{
	SkeletalModel* mesh = Resources::GetResourceById<SkeletalModel>(meshID);

	std::shared_ptr<GameObject> meshGO = std::make_shared<GameObject>(mesh->GetMeshName());
	meshGO.get()->AddComponent<Transform>();
	meshGO.get()->GetComponent<Transform>()->SetTransform(mesh->GetMeshTransform());
	meshGO.get()->AddComponent<Mesh>();

	meshGO.get()->GetComponent<Mesh>()->meshID = meshID;
	meshGO.get()->GetComponent<Mesh>()->materialID = Resources::LoadFromLibrary<Material>(mesh->GetMaterialPath());
	meshGO.get()->GetComponent<Mesh>()->meshType = MeshType::SKELETAL;

	if (isSingle)
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
		LOG(LogType::LOG_ERROR, "Failed to open prefab file: %s", filename);
		return;
	}

	json prefabJSON;
	try
	{
		file >> prefabJSON;
	}
	catch (const json::parse_error& e)
	{
		LOG(LogType::LOG_ERROR, "Failed to parse prefab JSON: %s", e.what());
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
				if (goToModify->GetComponent<Collider2D>() == nullptr)
					goToModify->AddComponent<Collider2D>();

				auto collider = goToModify->GetComponent<Collider2D>();
				collider->LoadComponent(componentJSON);
				break;
			}
			case ComponentType::Camera:
			{
				if (goToModify->GetComponent<Camera>() == nullptr) 
					goToModify->AddComponent<Camera>();

				auto camera = goToModify->GetComponent<Camera>();
				camera->LoadComponent(componentJSON);
				break;
			}
			case ComponentType::Canvas:
			{
				if (goToModify->GetComponent<Canvas>() == nullptr)
					goToModify->AddComponent<Canvas>();

				auto canvas = goToModify->GetComponent<Canvas>();
				canvas->LoadComponent(componentJSON);
				break;
			}
			case ComponentType::AudioSource:
			{
				if (goToModify->GetComponent<AudioSource>() == nullptr)
					goToModify->AddComponent<AudioSource>();

				auto audioSource = goToModify->GetComponent<AudioSource>();
				audioSource->LoadComponent(componentJSON);
				break;
			}
			case ComponentType::Listener:
			{
				if (goToModify->GetComponent<Listener>() == nullptr)
					goToModify->AddComponent<Listener>();

				auto listener = goToModify->GetComponent<Listener>();
				listener->LoadComponent(componentJSON);
				break;
			}
			case ComponentType::ParticleSystem:
			{
				if (goToModify->GetComponent<ParticleSystem>() == nullptr)
					goToModify->AddComponent<ParticleSystem>();

				auto particleSystem = goToModify->GetComponent<ParticleSystem>();
				particleSystem->LoadComponent(componentJSON);
				break;
			}
			case ComponentType::Light:
			{
				if (goToModify->GetComponent<Light>() == nullptr)
					goToModify->AddComponent<Light>();

				auto light = goToModify->GetComponent<Light>();
				light->LoadComponent(componentJSON);
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

void N_SceneManager::CreatePrefabWithName(std::string prefabName, const vec3f& position, vec3f& rotation)
{
	auto newGameObject = CreateEmptyGO();
	newGameObject.get()->SetName(currentScene->GetSceneName());

	// Load the game object from 
	fs::path filename = ASSETS_PATH;
	filename += "Prefabs\\" + prefabName + ".prefab";

	std::ifstream file(filename);
	if (!file.is_open())
	{
		LOG(LogType::LOG_ERROR, "Failed to open prefab file: %s", filename);
		return;
	}

	json prefabJSON;
	try
	{
		file >> prefabJSON;
	}
	catch (const json::parse_error& e)
	{
		LOG(LogType::LOG_ERROR, "Failed to parse prefab JSON: %s", e.what());
		return;
	}

	// Close the file
	file.close();

	newGameObject->LoadGameObject(prefabJSON);

	Transform* goTransform = newGameObject->GetComponent<Transform>();

	goTransform->SetPosition(position);
	goTransform->SetRotation(rotation);
}

void N_SceneManager::CreatePrefabWithName(std::string prefabName, const mat4& transform)
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


	// Check name

	if (newGameObject.get()->GetName() != "SK_MainCharacter" && newGameObject.get()->GetName() != "UI_Manager")
	{
		newGameObject.get()->SetName(GenerateUniqueName(newGameObject.get()->GetName()));
	}


	newGameObject->GetComponent<Transform>()->SetTransform(transform);
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
	Camera* camera = cam ? cam : currentCamera;

	zSorting.clear();
	RecurseSceneDraw(rootSceneGO, camera);
	
	for (auto i = zSorting.rbegin(); i != zSorting.rend(); ++i)
		i->second->Draw(camera);

	if (mode == DrawMode::EDITOR)
		return;

	RecurseUIDraw(rootSceneGO, mode);
	if (engine->N_sceneManager->GetSceneIsChanging())
		engine->N_sceneManager->loadingScreen->DrawUI(cam, DrawMode::GAME);
}

inline void Scene::RecurseSceneDraw(std::shared_ptr<GameObject> parentGO, Camera* camera)
{
	for (const auto& gameObject : parentGO.get()->children)
	{
		if (!gameObject->hasTransparency) {
			gameObject->Draw(camera);
		}
		else
		{
			float distance = glm::length(
				(vec3)gameObject->GetComponent<Transform>()->GetGlobalTransform()[3] -
				camera->GetContainerGO()->GetComponent<Transform>()->GetPosition());

			zSorting.insert(std::pair<float, GameObject*>(distance, gameObject.get()));
		}

		RecurseSceneDraw(gameObject, camera);
	}
}