#include "CollisionSolver.h"
#include "Defs.h"
#include "Collider2D.h"
#include "Transform.h"
#include "EngineCore.h"
#include "N_SceneManager.h"
#include "MonoManager.h"

#include "SDL2/SDL.h"
#include "GL/gl.h"

CollisionSolver::CollisionSolver() {}

CollisionSolver::~CollisionSolver() {}

bool CollisionSolver::PreUpdate()
{
    bool ret = true;

    return ret;
}

bool CollisionSolver::Update(double dt)
{
    bool ret = true;
    //first, lets see the collider component still exists
    for (auto it = goWithCollision.begin(); it != goWithCollision.end(); )
    {
        bool remItem = true;
        for (auto& item2 : (*it)->GetAllComponents())
        {
            if (item2->GetType() == ComponentType::Collider2D) remItem = false;
        }
        if (remItem)
        {
            it = goWithCollision.erase(it);
        }
        else
        {
            ++it;
        }
    }

    //now lets check and solve collisions
    for (auto& item : goWithCollision)
    {
        // Collision solving
        switch (item->GetComponent<Collider2D>()->collisionType)
        {
        case CollisionType::Player:
            for (auto& item2 : goWithCollision)
            {
                if (item != item2)
                {
                    switch (item2->GetComponent<Collider2D>()->collisionType)
                    {
                    case CollisionType::Player:
                        //there is no player-player collision since we only have 1 player
                        break;
                    case CollisionType::Enemy:
                        //implement any low life to player
                        break;
                    case CollisionType::Wall:
                        //if they collide
                        if (CheckCollision(item, item2))
                        {
                            //we push player out of wall
                            SolveCollision(item, item2);
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
            break;
        case CollisionType::Enemy:
            for (auto& item2 : goWithCollision)
            {
                if (item != item2)
                {
                    switch (item2->GetComponent<Collider2D>()->collisionType)
                    {
                    case CollisionType::Player:
                        //implement any low life to player
                        if (CheckCollision(item, item2))
                        {
                            MonoManager::CallScriptFunction(item->GetComponent<Script>()->monoBehaviourInstance, "CheckJump");
                        }
                        break;
                    case CollisionType::Enemy:
                        //if they collide
                        if (CheckCollision(item, item2))
                        {
                            //we push player out of other enemy
                            SolveCollision(item, item2);
                        }
                        break;
                    case CollisionType::Wall:
                        //if they collide
                        if (CheckCollision(item, item2))
                        {
                            //we push enemy out of wall
                            SolveCollision(item, item2);
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
            break;
        case CollisionType::Wall:
            // do nothing at all
            break;
        case CollisionType::Bullet:
            for (auto& item2 : goWithCollision)
            {
                if (item != item2)
                {
                    switch (item2->GetComponent<Collider2D>()->collisionType)
                    {
                    case CollisionType::Player:
                        if (CheckCollision(item, item2))
                        {
                            MonoManager::CallScriptFunction(item2->GetComponent<Script>()->monoBehaviourInstance, "ReduceLife");
                            item->AddToDelete(engine->N_sceneManager->objectsToDelete);
                        }
                        break;
                    case CollisionType::Enemy:
                        //if they collide
                        if (CheckCollision(item, item2))
                        {
                            MonoManager::CallScriptFunction(item2->GetComponent<Script>()->monoBehaviourInstance, "ReduceLife");
                            item->AddToDelete(engine->N_sceneManager->objectsToDelete);
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
            break;
        default:
            break;
        }
    }

    return ret;
}

vec2 CollisionSolver::Clamp(vec2 origin, vec2 min, vec2 max)
{
    origin.x <= min.x ? origin.x = min.x : origin.x = origin.x;
    origin.y <= min.y ? origin.y = min.y : origin.y = origin.y;

    origin.x >= max.x ? origin.x = max.x : origin.x = origin.x;
    origin.y >= max.y ? origin.y = max.y : origin.y = origin.y;
    return origin;
}

void CollisionSolver::DrawCollisions() {
    std::vector<GameObject*> temp = SceneRootToCollisionVector(engine->N_sceneManager->currentScene->GetRootSceneGO());

    for (const auto& collision : temp) {
        Transform* transform = collision->GetComponent<Transform>();
        Collider2D* collider = collision->GetComponent<Collider2D>();

        if (!transform || !collider) {
            LOG(LogType::LOG_ERROR, "Missing required component for collision object");
            continue;
        }

        glm::vec3 position = glm::vec3(transform->GetPosition()) + glm::vec3(collider->offset.x, 0.0f, collider->offset.y);
        glm::vec4 color(0.0f, 0.8f, 0.0f, 1.0f);

        if (collider->colliderType == ColliderType::Rect) {
            glm::vec2 size(collider->w, collider->h);

            if (collider->cornerPivot) {
                position -= glm::vec3(size.x / 2, 0.0f, size.y / 2);
            }

            glm::mat4 rotation = glm::mat4(1.0f);
            /*switch (collider->objectOrientation) {
            case ObjectOrientation::Right:
                rotation = glm::rotate(rotation, glm::radians(90.0f), glm::vec3(1, 0, 0));
                break;
            case ObjectOrientation::Back:
                rotation = glm::rotate(rotation, glm::radians(180.0f), glm::vec3(1, 0, 0));
                break;
            case ObjectOrientation::Left:
                rotation = glm::rotate(rotation, glm::radians(270.0f), glm::vec3(1, 0, 0));
                break;
            default:
                break;
            }*/

            glm::mat4 model = glm::translate(glm::mat4(1.0f), position) 
                * rotation 
                * glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));
            Renderer2D::DrawRect(model, color);
        }
        else if (collider->colliderType == ColliderType::Circle) {
            float radius = collider->radius;
            glm::mat4 circleTransform = glm::translate(glm::mat4(1.0f), position)
                * glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(1, 0, 0))
                * glm::scale(glm::mat4(1.0f), glm::vec3(radius, radius, radius));
            Renderer2D::DrawCircle(circleTransform, color, 0.1f);
        }
    }
}

double DistanceXZ(const vec3 posA, const vec3 posB)
{
    vec2 posA_XZ = vec2(posA.x, posA.z);
    vec2 posB_XZ = vec2(posB.x, posB.z);

    return glm::distance(posA_XZ, posB_XZ);
}

bool CollisionSolver::CheckCollision(GameObject* objA, GameObject* objB)
{
    const Transform* transformA = objA->GetComponent<Transform>();
    const Collider2D* colliderA = objA->GetComponent<Collider2D>();

    const Transform* transformB = objB->GetComponent<Transform>();
    const Collider2D* colliderB = objB->GetComponent<Collider2D>();

    if (colliderA->colliderType == ColliderType::Circle && colliderB->colliderType == ColliderType::Circle)
    {
        // Check collision between two circles
        double distance = DistanceXZ(transformA->GetPosition() + vec3(colliderA->offset.x, 0, colliderA->offset.y), transformB->GetPosition() + vec3(colliderB->offset.x, 0, colliderB->offset.y));
        double radiusSum = colliderA->radius + colliderB->radius;
        return (distance <= radiusSum);
    }
    else if (colliderA->colliderType == ColliderType::Rect && colliderB->colliderType == ColliderType::Rect)
    {
        //right now there is no usage of a rect to rect collision
        

        //if (colliderA->cornerPivot)
        //{
        //    switch (colliderA->objectOrientation)
        //    {
        //    case ObjectOrientation::Front:
        //        break;
        //    case ObjectOrientation::Right:
        //        break;
        //    case ObjectOrientation::Back:
        //        break;
        //    case ObjectOrientation::Left:
        //        break;
        //    default:
        //        break;
        //    }
        //}
        //else
        //{
        //    // Check collision between two rectangles
        //    double xDist = abs(transformA->GetPosition().x + colliderA->offset.x - transformB->GetPosition().x + colliderB->offset.x);
        //    double yDist = abs(transformA->GetPosition().z + colliderA->offset.y - transformB->GetPosition().z + colliderA->offset.y);
        //    double xOverlap = (colliderA->w + colliderB->w) / 2 - xDist;
        //    double yOverlap = (colliderA->h + colliderB->h) / 2 - yDist;
        //    return (xOverlap > 0 && yOverlap > 0);
        //}
    }
    else
    {
        // Swap so that objA is the circle
        GameObject* circle;
        GameObject* rectangle;
        if (colliderA->colliderType == ColliderType::Circle) {
            circle = objA;
            rectangle = objB;
        }
        else {
            circle = objB;
            rectangle = objA;
        }
        
        const Transform* circleT = circle->GetComponent<Transform>();
        const Collider2D* circleC = circle->GetComponent<Collider2D>();

        const Transform* rectangleT = rectangle->GetComponent<Transform>();
        const Collider2D* rectangleC = rectangle->GetComponent<Collider2D>();

        if (rectangleC->cornerPivot)
        {
            double circleDistanceX;
            double circleDistanceY;
            double cornerDistanceSq;
            bool ret;
            switch (rectangleC->objectOrientation)
            {
            case ObjectOrientation::Front:
                // Calculate distance between circle center and rectangle center
                circleDistanceX = std::abs(circleT->GetPosition().x + circleC->offset.x - (rectangleT->GetPosition().x + rectangleC->w / 2 + rectangleC->offset.x));
                circleDistanceY = std::abs(circleT->GetPosition().z + circleC->offset.y - (rectangleT->GetPosition().z + rectangleC->h / 2 + rectangleC->offset.y));

                // Check if circle is too far away from rectangle to collide
                if (circleDistanceX > (rectangleC->w / 2.0 + circleC->radius)) { return false; }
                if (circleDistanceY > (rectangleC->h / 2.0 + circleC->radius)) { return false; }

                // Check if circle is close enough to collide with rectangle
                if (circleDistanceX <= (rectangleC->w / 2.0)) { return true; }
                if (circleDistanceY <= (rectangleC->h / 2.0)) { return true; }

                // Check if circle collides with corner of rectangle
                cornerDistanceSq = pow(circleDistanceX - rectangleC->w / 2.0, 2) + pow(circleDistanceY - rectangleC->h / 2.0, 2);
                ret = (cornerDistanceSq <= pow(circleC->radius, 2));
                return ret;
                break;
            case ObjectOrientation::Right:
                circleDistanceX = std::abs(circleT->GetPosition().x + circleC->offset.x - (rectangleT->GetPosition().x - rectangleC->w / 2 + rectangleC->offset.x));
                circleDistanceY = std::abs(circleT->GetPosition().z + circleC->offset.y - (rectangleT->GetPosition().z + rectangleC->h / 2 + rectangleC->offset.y));

                // Check if circle is too far away from rectangle to collide
                if (circleDistanceX > (rectangleC->w / 2.0 + circleC->radius)) { return false; }
                if (circleDistanceY > (rectangleC->h / 2.0 + circleC->radius)) { return false; }

                // Check if circle is close enough to collide with rectangle
                if (circleDistanceX <= (rectangleC->w / 2.0)) { return true; }
                if (circleDistanceY <= (rectangleC->h / 2.0)) { return true; }

                // Check if circle collides with corner of rectangle
                cornerDistanceSq = pow(circleDistanceX - rectangleC->w / 2.0, 2) + pow(circleDistanceY - rectangleC->h / 2.0, 2);
                ret = (cornerDistanceSq <= pow(circleC->radius, 2));
                return ret;
                break;
            case ObjectOrientation::Back:
                circleDistanceX = std::abs(circleT->GetPosition().x + circleC->offset.x - (rectangleT->GetPosition().x - rectangleC->w / 2 + rectangleC->offset.x));
                circleDistanceY = std::abs(circleT->GetPosition().z + circleC->offset.y - (rectangleT->GetPosition().z - rectangleC->h / 2 + rectangleC->offset.y));

                // Check if circle is too far away from rectangle to collide
                if (circleDistanceX > (rectangleC->w / 2.0 + circleC->radius)) { return false; }
                if (circleDistanceY > (rectangleC->h / 2.0 + circleC->radius)) { return false; }

                // Check if circle is close enough to collide with rectangle
                if (circleDistanceX <= (rectangleC->w / 2.0)) { return true; }
                if (circleDistanceY <= (rectangleC->h / 2.0)) { return true; }

                // Check if circle collides with corner of rectangle
                cornerDistanceSq = pow(circleDistanceX - rectangleC->w / 2.0, 2) + pow(circleDistanceY - rectangleC->h / 2.0, 2);
                ret = (cornerDistanceSq <= pow(circleC->radius, 2));
                return ret;
                break;
            case ObjectOrientation::Left:
                circleDistanceX = std::abs(circleT->GetPosition().x + circleC->offset.x - (rectangleT->GetPosition().x + rectangleC->w / 2 + rectangleC->offset.x));
                circleDistanceY = std::abs(circleT->GetPosition().z + circleC->offset.y - (rectangleT->GetPosition().z - rectangleC->h / 2 + rectangleC->offset.y));

                // Check if circle is too far away from rectangle to collide
                if (circleDistanceX > (rectangleC->w / 2.0 + circleC->radius)) { return false; }
                if (circleDistanceY > (rectangleC->h / 2.0 + circleC->radius)) { return false; }

                // Check if circle is close enough to collide with rectangle
                if (circleDistanceX <= (rectangleC->w / 2.0)) { return true; }
                if (circleDistanceY <= (rectangleC->h / 2.0)) { return true; }

                // Check if circle collides with corner of rectangle
                cornerDistanceSq = pow(circleDistanceX - rectangleC->w / 2.0, 2) + pow(circleDistanceY - rectangleC->h / 2.0, 2);
                ret = (cornerDistanceSq <= pow(circleC->radius, 2));
                return ret;
                break;
            default:
                break;
            }
        }
        else
        {
            // Calculate distance between circle center and rectangle center
            double circleDistanceX = std::abs(circleT->GetPosition().x + circleC->offset.x - (rectangleT->GetPosition().x + rectangleC->offset.x));
            double circleDistanceY = std::abs(circleT->GetPosition().z + circleC->offset.y - (rectangleT->GetPosition().z + rectangleC->offset.y));

            // Check if circle is too far away from rectangle to collide
            if (circleDistanceX > (rectangleC->w / 2.0 + circleC->radius)) { return false; }
            if (circleDistanceY > (rectangleC->h / 2.0 + circleC->radius)) { return false; }

            // Check if circle is close enough to collide with rectangle
            if (circleDistanceX <= (rectangleC->w / 2.0)) { return true; }
            if (circleDistanceY <= (rectangleC->h / 2.0)) { return true; }

            // Check if circle collides with corner of rectangle
            double cornerDistanceSq = pow(circleDistanceX - rectangleC->w / 2.0, 2) + pow(circleDistanceY - rectangleC->h / 2.0, 2);
            bool ret = (cornerDistanceSq <= pow(circleC->radius, 2));
            return ret;
        }
    }
}

void CollisionSolver::LoadCollisions(std::shared_ptr<GameObject> go)
{
    if (go->GetComponent<Collider2D>())
    {
        goWithCollision.push_back(go.get());
    }
    for (auto& item : go->children)
    {
        if (item->IsEnabled())
        {
            LoadCollisions(item);
        }
    }
}

void CollisionSolver::ClearCollisions()
{
    goWithCollision.clear();
}

std::vector<GameObject*> CollisionSolver::SceneRootToCollisionVector(std::shared_ptr<GameObject> go)
{
    std::vector<GameObject*> ret;
    for (auto& item : go->children)
    {
        std::vector<GameObject*> it;
        if (item->IsEnabled())
        {
            if (item->GetComponent<Collider2D>())
            {
                ret.push_back(item.get());
            }
            if (!item->children.empty())
            {
                it = SceneRootToCollisionVector(item);
                for (auto& item2 : it)
                {
                    ret.push_back(item2);
                }
            }
        }
    }
    return ret;
}

void CollisionSolver::SolveCollision(GameObject* objA, GameObject* objB)
{
    switch (objA->GetComponent<Collider2D>()->colliderType)
    {
    case ColliderType::Circle:
        switch (objB->GetComponent<Collider2D>()->colliderType)
        {
        case ColliderType::Circle:
            CirCirCollision(objA, objB);
            break;
        case ColliderType::Rect:
            CirRectCollision(objA, objB);
            break;
        default:
            break;
        }
        break;
    case ColliderType::Rect:
        switch (objB->GetComponent<Collider2D>()->colliderType)
        {
        case ColliderType::Circle:
            //rect circle function not implemented
            break;
        case ColliderType::Rect:
            RectRectCollision(objA, objB);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void CollisionSolver::RectRectCollision(GameObject* objA, GameObject* objB)
{
    // Currently not needed
}

void CollisionSolver::CirCirCollision(GameObject* objA, GameObject* objB)
{
    Transform* transformA = objA->GetComponent<Transform>();
    Collider2D* colliderA = objA->GetComponent<Collider2D>();

    Transform* transformB = objB->GetComponent<Transform>();
    Collider2D* colliderB = objB->GetComponent<Collider2D>();


    // Vector from player circle center to NPC circle center
    vec2 collisionVector = { 
        transformA->GetPosition().x + colliderA->offset.x - transformB->GetPosition().x + colliderB->offset.x, 
        transformA->GetPosition().z + colliderA->offset.y - transformB->GetPosition().z + colliderB->offset.y 
    };

    // Distance between the centers of the two circles
    vec2 pos2dA = { transformA->GetPosition().x + colliderA->offset.x, transformA->GetPosition().z + colliderA->offset.y };
    vec2 pos2dB = { transformB->GetPosition().x + colliderB->offset.x, transformB->GetPosition().z + colliderB->offset.y };
    double distance = glm::distance(pos2dA, pos2dB);

    // Sum of the radii of the two circles
    double totalRadius = colliderA->radius + colliderB->radius;

    // Calculate mtv if the is overlap
    if (distance <= totalRadius)
    {
        vec2 mtv = glm::normalize(collisionVector);
        mtv.x *= totalRadius - distance;
        mtv.y *= totalRadius - distance;

        transformA->SetPosition({ transformA->GetPosition().x + mtv.x, transformA->GetPosition().y, transformA->GetPosition().z + mtv.y });
    }
}

void CollisionSolver::CirRectCollision(GameObject* objA, GameObject* objB)
{
    Transform* transformA = objA->GetComponent<Transform>();
    Collider2D* colliderA = objA->GetComponent<Collider2D>();

    Transform* transformB = objB->GetComponent<Transform>();
    Collider2D* colliderB = objB->GetComponent<Collider2D>();

    // Closest point on the rectangle to the circle center
    vec2 topLeft = { transformB->GetPosition().x - colliderB->w / 2 + colliderB->offset.x, transformB->GetPosition().z - colliderB->h / 2 + colliderB->offset.y };
    vec2 botRight = { transformB->GetPosition().x + colliderB->w / 2 + colliderB->offset.x, transformB->GetPosition().z + colliderB->h / 2 + colliderB->offset.y };

    if (colliderB->cornerPivot)
    {
        switch (colliderB->objectOrientation)
        {
        case ObjectOrientation::Front:
            topLeft = { 
                transformB->GetPosition().x - colliderB->w / 2 + colliderB->offset.x + colliderB->w / 2,
                transformB->GetPosition().z - colliderB->h / 2 + colliderB->offset.y + colliderB->h / 2
            };
            botRight = { 
                transformB->GetPosition().x + colliderB->w / 2 + colliderB->offset.x + colliderB->w / 2,
                transformB->GetPosition().z + colliderB->h / 2 + colliderB->offset.y + colliderB->h / 2
            };
            break;
        case ObjectOrientation::Right:
            topLeft = {
                transformB->GetPosition().x - colliderB->w / 2 + colliderB->offset.x - colliderB->w / 2,
                transformB->GetPosition().z - colliderB->h / 2 + colliderB->offset.y + colliderB->h / 2
            };
            botRight = {
                transformB->GetPosition().x + colliderB->w / 2 + colliderB->offset.x - colliderB->w / 2,
                transformB->GetPosition().z + colliderB->h / 2 + colliderB->offset.y + colliderB->h / 2
            };
            break;
        case ObjectOrientation::Back:
            topLeft = {
                transformB->GetPosition().x - colliderB->w / 2 + colliderB->offset.x - colliderB->w / 2,
                transformB->GetPosition().z - colliderB->h / 2 + colliderB->offset.y - colliderB->h / 2
            };
            botRight = {
                transformB->GetPosition().x + colliderB->w / 2 + colliderB->offset.x - colliderB->w / 2,
                transformB->GetPosition().z + colliderB->h / 2 + colliderB->offset.y - colliderB->h / 2
            };
            break;
        case ObjectOrientation::Left:
            topLeft = {
                transformB->GetPosition().x - colliderB->w / 2 + colliderB->offset.x + colliderB->w / 2,
                transformB->GetPosition().z - colliderB->h / 2 + colliderB->offset.y - colliderB->h / 2
            };
            botRight = {
                transformB->GetPosition().x + colliderB->w / 2 + colliderB->offset.x + colliderB->w / 2,
                transformB->GetPosition().z + colliderB->h / 2 + colliderB->offset.y - colliderB->h / 2
            };
            break;
        default:
            break;
        }
    }

    vec2 temp = { transformA->GetPosition().x + colliderA->offset.x, transformA->GetPosition().z + colliderA->offset.y };
    vec2 closestPoint = Clamp(temp, topLeft, botRight);

    // Vector from the closest point on the rectangle to the circle center
    vec2 rectToCircle = temp - closestPoint;

    // Distance between the closest point and the circle center
    double distance = glm::distance(temp, closestPoint);

    // Overlap distance between the player and the obstacle
    double overlapDistance = colliderA->radius - distance;

    // Calculate mtv if there is overlap
    if (overlapDistance > 0.0)
    {
        vec2 mtv = glm::normalize(rectToCircle);
        mtv.x *= overlapDistance;
        mtv.y *= overlapDistance;

        transformA->SetPosition({ transformA->GetPosition().x + mtv.x, transformA->GetPosition().y, transformA->GetPosition().z + mtv.y });
    }
}