#ifndef __BUILDER_SCENE_MANAGER_H__
#define __BUILDER_SCENE_MANAGER_H__
#pragma once

#include "BuilderModule.h"

#include "TheOneEngine\N_SceneManager.h"

#include <memory>

class GameObject;

class BuilderSceneManager : public BuilderModule
{
public:

	BuilderSceneManager(BuilderApp* app);
	~BuilderSceneManager();

	bool Awake();
	bool Start();

	bool PreUpdate();
	bool Update(double dt);
	bool PostUpdate();

	bool CleanUp();

private:

	// Render Target
	glm::vec2 viewportSize;
	unsigned int renderTarget;
	shared_ptr<GameObject> cameraGO;
};

#endif // !__BUILDER_SCENE_MANAGER_H__