#ifndef __SCENE_MANAGER_H__
#define __SCENE_MANAGER_H__
#pragma once

#include "Module.h"
#include "..\TheOneEngine\N_SceneManager.h"

#include <vector>
#include <string>
#include <memory>
#include <algorithm>

#define NOT_SPECIFIED ""

class SceneManager : public Module
{
public:
	SceneManager(App* app);
	virtual ~SceneManager();

	bool Awake();
	bool Start();

	bool PreUpdate();
	bool Update(double dt);
	bool PostUpdate();

	bool CleanUp();

private:
	// Private constructor to enforce singleton pattern
	SceneManager() {}
	int listenerID = -1;
};

#endif // !__SCENE_MANAGER_H__