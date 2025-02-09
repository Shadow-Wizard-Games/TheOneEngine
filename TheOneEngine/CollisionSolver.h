#ifndef __COLLISION_SOLVER_H__
#define __COLLISION_SOLVER_H__
#pragma once
#include "Defs.h"


class GameObject;
enum class ColliderType;

class CollisionSolver
{
public:
	CollisionSolver();
	~CollisionSolver();

	void CleanUp();

	bool PreUpdate();
	bool Update(double dt);

	bool CheckCollision(GameObject* objA, GameObject* objB);

	void LoadCollisions(std::shared_ptr<GameObject> go);
	void ClearCollisions();

	std::vector<GameObject*> SceneRootToCollisionVector(std::shared_ptr<GameObject> go);

	void SolveCollision(GameObject* objA, GameObject* objB);

	void RectRectCollision(GameObject* objA, GameObject* objB);
	void CirCirCollision(GameObject* objA, GameObject* objB);
	void CirRectCollision(GameObject* objA, GameObject* objB);

	vec2 Clamp(vec2 origin, vec2 min, vec2 max);

	void DrawCollisions();

	std::vector<GameObject*> goWithCollision;

private:

};

#endif // !__COLLISION_SOLVER_H__