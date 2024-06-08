#ifndef __COLLIDER_2D_H__
#define __COLLIDER_2D_H__
#pragma once

#include "Defs.h"
#include "GameObject.h"
#include "Log.h"


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
    Grenade,
    Explosion,
    Melee,
    Dot
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
    Collider2D(std::shared_ptr<GameObject> containerGO, ColliderType colliderType, CollisionType collisionType);
    Collider2D(std::shared_ptr<GameObject> containerGO, ColliderType colliderType, Collider2D* ref);
    ~Collider2D();

    json SaveComponent();
    void LoadComponent(const json& meshJSON);

public:
    bool activeCollision;

    ColliderType colliderType;
    CollisionType collisionType;
    double radius;
    double w, h;
    vec2 offset;
    bool cornerPivot;
    ObjectOrientation objectOrientation = ObjectOrientation::Front;
};
#endif // !__COLLIDER_2D_H__
