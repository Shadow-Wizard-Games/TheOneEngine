#ifndef __RENDERER_3D_H__
#define __RENDERER_3D_H__
#pragma once

#include "Module.h"

#include <memory>

class EngineCore;
class GameObject;
class SceneManager;

class Renderer3D : public Module
{
public:
	Renderer3D(App* app);

	virtual ~Renderer3D();

	bool Awake();
	bool Start();

	bool PreUpdate();
	bool Update(double dt);
	bool PostUpdate();

	bool CleanUp();

	void CreateRay();
};

#endif // !__RENDERER_3D_H__