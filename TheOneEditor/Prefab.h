#ifndef __PREFAB_H__
#define __PREFAB_H__
#pragma once

#include "..\TheOneEngine\GameObject.h"
#include <string>

class Prefab
{
	Prefab() {}
	Prefab(uint pId, std::string pName);
	~Prefab();

	GameObject* ConvertToGameObject(vec3f local_position, GameObject* parent = nullptr);

	void SerializePrefab(std::string path);

private:
	uint id = 0;
	std::string name;
};


#endif // !__PREFAB_H__