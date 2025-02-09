#ifndef __N_SCENE_MANAGER_H__
#define __N_SCENE_MANAGER_H__
#pragma once

#include "Defs.h"
#include "GameObject.h"
#include "Camera.h"
#include "Canvas.h"
#include "Light.h"

#include <string>
#include <memory>


class Scene;
class Transform;

enum DataType
{
	DATA_BOOL,
	DATA_STRING,
	DATA_INT,
	DATA_FLOAT
};

class N_SceneManager
{
public:
	N_SceneManager();
	~N_SceneManager();

	bool Awake();
	bool Start();

	bool PreUpdate();
	bool Update(double dt, bool isPlaying);
	bool PostUpdate();

	bool CleanUp();

public:

	void CreateNewScene(uint _index, std::string name);

	// Change between scenes
	void LoadScene(uint index);
	void LoadScene(std::string sceneName, bool keep = false);
	void LoadSave(std::string sceneName, std::string scenePath = "GameData/");

	std::string GenerateUniqueName(const std::string& baseName, const GameObject* parent = nullptr);

	// Create GameObjects functions
	std::shared_ptr<GameObject> DuplicateGO(std::shared_ptr<GameObject> originalGO, bool recursive = false);
	std::shared_ptr<GameObject> CreateEmptyGO(std::string name = "Empty GameObject", bool isRoot = true);
	void ReparentGO(std::shared_ptr<GameObject> go, std::shared_ptr<GameObject> newParentGO);

	std::shared_ptr<GameObject> CreateCameraGO(std::string name);
	std::shared_ptr<GameObject> CreateCanvasGO(std::string name);
	std::shared_ptr<GameObject> CreateLightGO(LightType type);

	void CreateMeshGO(const std::string& path);
	void CreateExistingMeshGO(const std::string& fbxName);
	void CreateLibMeshGO(const std::string& path);
	void CreateDefaultMeshGO(ResourceId meshID, std::shared_ptr<GameObject> emptyParent, bool isSingle);
	void CreateSkeletalMeshGO(ResourceId meshID, std::shared_ptr<GameObject> emptyParent, bool isSingle);

	std::shared_ptr<GameObject> CreateCube();
	std::shared_ptr<GameObject> CreateSphere();
	void CreateMF();
	void CreateTeapot();

	void AddPendingGOs();
	void DeletePendingGOs();

	void OverrideScenePrefabs(uint32_t prefabID);
	void OverridePrefabsRecursive(std::shared_ptr<GameObject> parent, uint32_t prefabID);
	void OverrideGameobjectFromPrefab(std::shared_ptr<GameObject> goToModify);
	void CreatePrefabWithName(std::string prefabName, const vec3f& position, vec3f& rotation);
	void CreatePrefabWithName(std::string prefabName, const mat4& transform);
	std::string ReturnLastGOName(std::string toCompare, std::shared_ptr<GameObject> parent);
	void CreatePrefabFromPath(std::string prefabPath, const vec3f& position);

	// Get/Set
	uint GetNumberGO() const;
	std::vector<std::shared_ptr<GameObject>>GetGameObjects();

	// hekbas: Either rename to IsSceneXXX
	// or use enum > GetSceneState
	const bool GetSceneIsPlaying() { return sceneIsPlaying; }
	const bool GetSceneIsChanging() { return sceneChange; }

	// SelectedGo
	std::shared_ptr<GameObject> GetSelectedGO() const;
	void SetSelectedGO(std::shared_ptr<GameObject> gameObj);

	void FindCameraInScene();

	/*SCENE SERIALIZATION*/
	void SaveScene(std::string directories, std::string sceneName);
	void LoadSceneFromJSON(const std::string& filename, bool keepGO = false);
	void RemoveFile(std::string pathToDelete);
	void* AccessFileDataRead(std::string filepath, DataType dataType, std::vector<std::string> dataPath, std::string dataName);
	void AccessFileDataWrite(std::string filepath, DataType dataType, std::vector<std::string> dataPath, std::string dataName, void* data);

private:
	void UpdateTransforms(std::shared_ptr<GameObject> go);
	void RecursiveScriptInit(std::shared_ptr<GameObject> go, bool firstInit = false);

public:
	Scene* currentScene = nullptr; //Convert to smart ptr
	std::vector<std::shared_ptr<GameObject>> objectsToAdd;
	std::vector<GameObject*> objectsToDelete;
	std::shared_ptr<GameObject> loadingScreen;

private:
	std::shared_ptr<GameObject> selectedGameObject = nullptr;
	bool sceneIsPlaying = false;
	bool previousFrameIsPlaying = false;
	//Kikofp02: This will check if the engine has to change scene
	bool sceneChange = false;
	bool sceneChangeKeepGOs = false;
};

class Scene
{
public:
	Scene() : sceneName("Scene"), index(0), isDirty(true)
	{
		rootSceneGO = std::make_shared<GameObject>(sceneName);
		rootSceneGO.get()->AddComponent<Transform>();
	}

	inline Scene(uint _index, std::string name) : sceneName(name), index(_index), isDirty(true)
	{
		rootSceneGO = std::make_shared<GameObject>(name);
		rootSceneGO.get()->AddComponent<Transform>();
	}

	~Scene() 
	{ 
		lights.clear();
		delete currentCamera;
		currentCamera = nullptr;
	}

	inline std::string GetSceneName() const { return sceneName; }
	inline void SetSceneName(std::string name) { sceneName = name; }

	inline uint GetIndex() const { return index; }
	inline void SetIndex(uint _index) { index = _index; }

	inline bool IsDirty() const { return isDirty; }
	inline void SetIsDirty(bool state) { isDirty = state; }

	inline std::string GetPath() const { return path; }
	inline void SetPath(std::string _path) { path = _path; }

	inline std::shared_ptr<GameObject> GetRootSceneGO() const { return rootSceneGO; }

	inline void UpdateGOs(double dt);
	
	void Draw(DrawMode mode = DrawMode::GAME, Camera* cam = nullptr);

	void FindCameraInScene();

	void ChangePrimaryCamera(GameObject* newPrimaryCam);

private:
	inline void RecurseSceneDraw(std::shared_ptr<GameObject> parentGO, Camera* cam = nullptr);
	inline void RecurseUIDraw(std::shared_ptr<GameObject> parentGO, DrawMode mode = DrawMode::GAME);
	//void Set2DCamera();

private:
	uint index;
	std::string sceneName;
	std::shared_ptr<GameObject> rootSceneGO;

	//Historn: This is to remember to save the scene if any change is made
	bool isDirty;

	std::string path;

public:
	Camera* currentCamera = nullptr;

	std::multimap<float, GameObject*> zSorting;

	std::vector<Light*> lights;

	//int listenerAudioGOID = -1;
};

#endif // !__N_SCENE_MANAGER_H__