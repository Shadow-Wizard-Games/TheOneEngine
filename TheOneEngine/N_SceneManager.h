#ifndef __N_SCENE_MANAGER_H__
#define __N_SCENE_MANAGER_H__
#pragma once

#include "Defs.h"
#include "GameObject.h"
#include "Camera.h"
#include "Canvas.h"

#include <string>
#include <memory>


class Scene;
class Transform;
class MeshLoader;

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
	void LoadScene(std::string sceneName);

	std::string GenerateUniqueName(const std::string& baseName);

	// Create GameObjects functions
	std::shared_ptr<GameObject> DuplicateGO(std::shared_ptr<GameObject> originalGO, bool recursive = false);
	std::shared_ptr<GameObject> CreateEmptyGO(std::string name = "Empty GameObject", bool isRoot = true);
	void ReparentGO(std::shared_ptr<GameObject> go, std::shared_ptr<GameObject> newParentGO);
	std::shared_ptr<GameObject> CreateCameraGO(std::string name);
	std::shared_ptr<GameObject> CreateCanvasGO(std::string name);

	// Try to mix this two (CreateExistingMeshGO should be erased and CreateMeshGO has to do)
	std::shared_ptr<GameObject> CreateMeshGO(std::string path);
	std::shared_ptr<GameObject> CreateExistingMeshGO(std::string fbxName);

	std::shared_ptr<GameObject> CreateCube();
	std::shared_ptr<GameObject> CreateSphere();
	std::shared_ptr<GameObject> CreateMF();
	std::shared_ptr<GameObject> CreateTeapot();

	void AddPendingGOs();
	void DeletePendingGOs();

	// Get/Set
	uint GetNumberGO() const;
	std::vector<std::shared_ptr<GameObject>>GetGameObjects();
	const bool GetSceneIsPlaying() { return sceneIsPlaying; }
	const bool GetSceneIsChanging() { return sceneChange; }

	// SelectedGo (Editor Only???)
	std::shared_ptr<GameObject> GetSelectedGO() const;
	void SetSelectedGO(std::shared_ptr<GameObject> gameObj);

	void FindCameraInScene();

	/*SCENE SERIALIZATION*/
	void SaveScene();
	void LoadSceneFromJSON(const std::string& filename);

public:
	Scene* currentScene = nullptr; //Convert to smart ptr
	std::vector<std::shared_ptr<GameObject>> objectsToAdd;
	std::vector<GameObject*> objectsToDelete;
	std::shared_ptr<GameObject> loadingScreen;

private:
	std::shared_ptr<GameObject> selectedGameObject = nullptr;
	MeshLoader* meshLoader = nullptr;
	bool sceneIsPlaying = false;
	bool previousFrameIsPlaying = false;
	//Kikofp02: This will check if the engine has to change scene
	bool sceneChange = false;
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

	~Scene() {}

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
	
	void Draw(DrawMode mode = DrawMode::GAME);

	void FindCameraInScene();

	void ChangePrimaryCamera(GameObject* newPrimaryCam);

private:
	inline void RecurseSceneDraw(std::shared_ptr<GameObject> parentGO);
	inline void RecurseUIDraw(std::shared_ptr<GameObject> parentGO, DrawMode mode = DrawMode::GAME);

private:
	uint index;
	std::string sceneName;
	std::shared_ptr<GameObject> rootSceneGO;

	//Historn: This is to remember to save the scene if any change is made
	bool isDirty;

	std::string path;

public:
	Camera* currentCamera = nullptr;
	//int listenerAudioGOID = -1;
};

#endif // !__N_SCENE_MANAGER_H__