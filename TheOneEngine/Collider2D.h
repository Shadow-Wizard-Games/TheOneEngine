#ifndef __COLLIDER_2D_H__
#define __COLLIDER_2D_H__
#pragma once

#include "Defs.h"
#include "GameObject.h"
#include "Log.h"

#include "..\TheOneEditor\Input.h"


enum class ColliderType
{
    Circle,
    Rect
};

enum class CollisionType
{
    Player,
    Enemy,
    Wall,
    Bullet,
};

enum class ObjectOrientation
{
    Front = 0,
    Right,
    Back,
    Left
};

class Collider2D : public Component
{
public:

    Collider2D(std::shared_ptr<GameObject> containerGO);
    Collider2D(std::shared_ptr<GameObject> containerGO, Collider2D* ref);
    Collider2D(std::shared_ptr<GameObject> containerGO, ColliderType colliderType);
    ~Collider2D();

    json SaveComponent();
    void LoadComponent(const json& meshJSON);

public:

    ColliderType colliderType;
    CollisionType collisionType;
    double radius;
    double w, h;
    vec2 offset;
    //true:   collision will be build with pivot as corner
    //false:  collision will be build with pivot as center
    bool cornerPivot;
    ObjectOrientation objectOrientation = ObjectOrientation::Front;
};
#endif // !__COLLIDER_2D_H__
