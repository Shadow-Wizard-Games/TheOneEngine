#include "Collider2D.h"
#include "Defs.h"

#include "GameObject.h"
#include "Transform.h"

#include "EngineCore.h"

#include "SDL2/SDL.h"
#include "GL/gl.h"

Collider2D::Collider2D(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Collider2D)
{
    colliderType = ColliderType::Circle;
    this->collisionType = CollisionType::Player;

    w = containerGO.get()->GetAABBox().sizes().x;
    h = containerGO.get()->GetAABBox().sizes().z;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;

    //TODO: CHANGE INTO REAL CALCULATED RADIUS
    radius = 0.5;
    //push the game object that has colliders into the collidergo list
    engine->collisionSolver->goWithCollision.push_back(containerGO.get());
}

Collider2D::Collider2D(std::shared_ptr<GameObject> containerGO, Collider2D* ref) : Component(containerGO, ComponentType::Collider2D)
{
    this->colliderType = ref->colliderType;
    this->collisionType = ref->collisionType;
    this->h = ref->h;
    this->w = ref->w;
    //TODO: CHANGE INTO REAL CALCULATED RADIUS
    this->radius = ref->radius;
    //push the game object that has colliders into the collidergo list
    engine->collisionSolver->goWithCollision.push_back(containerGO.get());
}

Collider2D::Collider2D(std::shared_ptr<GameObject> containerGO, ColliderType colliderType) :
    Component(containerGO, ComponentType::Collider2D),
    colliderType(colliderType)
{
    this->collisionType = CollisionType::Player;
    radius = 0.0;
    //TODO: CHANGE INTO REAL CALCULATED SIZES
    h = 1;
    w = 1;
    //push the game object that has colliders into the collidergo list
    engine->collisionSolver->goWithCollision.push_back(containerGO.get());
}

Collider2D::~Collider2D() {}

json Collider2D::SaveComponent()
{
    json colliderJSON;

    colliderJSON["UID"] = UID;
    colliderJSON["Name"] = name;
    colliderJSON["Type"] = type;
    colliderJSON["CollisionType"] = collisionType;
    colliderJSON["ColliderType"] = colliderType;
    colliderJSON["Width"] = w;
    colliderJSON["Height"] = h;
    colliderJSON["Radius"] = radius;
    colliderJSON["OffsetX"] = offset.x;
    colliderJSON["OffsetY"] = offset.y;
    colliderJSON["OffsetZ"] = offset.z;

    if (auto pGO = containerGO.lock())
        colliderJSON["ParentUID"] = pGO.get()->GetUID();


    return colliderJSON;
}

void Collider2D::LoadComponent(const json& colliderJSON)
{
    // Load basic properties
    if (colliderJSON.contains("UID")) UID = colliderJSON["UID"];
    if (colliderJSON.contains("Name")) name = colliderJSON["Name"];
    if (colliderJSON.contains("Type")) type = colliderJSON["Type"];
    if (colliderJSON.contains("CollisionType")) collisionType = colliderJSON["CollisionType"];
    if (colliderJSON.contains("ColliderType")) colliderType = colliderJSON["ColliderType"];
    if (colliderJSON.contains("Width")) w = colliderJSON["Width"];
    if (colliderJSON.contains("Height")) h = colliderJSON["Height"];
    if (colliderJSON.contains("Radius")) radius = colliderJSON["Radius"];
    if (colliderJSON.contains("OffsetX")) offset.x = colliderJSON["OffsetX"];
    if (colliderJSON.contains("OffsetY")) offset.y = colliderJSON["OffsetY"];
    if (colliderJSON.contains("OffsetZ")) offset.z = colliderJSON["OffsetZ"];

    if (colliderJSON.contains("UID")) UID = colliderJSON["UID"];

    // Load parent UID and set parent
    /*if (colliderJSON.contains("ParentUID"))
    {
        uint32_t parentUID = colliderJSON["ParentUID"];
        if (auto parentGameObject = SceneManager::GetInstance().FindGOByUID(parentUID))
        {
            containerGO = parentGameObject;
        }
    }*/
}