#include "CollisionSolver.h"
#include "Defs.h"
#include "Collider2D.h"
#include "Transform.h"
#include "EngineCore.h"
#include "N_SceneManager.h"
#include "MonoManager.h"

#include "SDL2/SDL.h"
#include "GL/gl.h"

CollisionSolver::CollisionSolver()
{
    drawCollisions = true;
}

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
                            LOG(LogType::LOG_WARNING, "Player Hit");
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

void CollisionSolver::DrawCollisions()
{
    //just in case lets make sure all items in the list have collision component
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

    // Iterar sobre las colisiones y dibujarlas
    for (const auto& collision : goWithCollision) {
        glPushMatrix();

        // Obtener la posición del objeto con colisión
        auto transform = collision->GetComponent<Transform>();
        if (transform == nullptr) {
            LOG(LogType::LOG_ERROR, "Transform component not found for collision object");
            continue;
        }

        auto collider = collision->GetComponent<Collider2D>();

        //Calculate position of collider
        float x_, y_, z_ = 0;
        x_ = transform->GetPosition().x + collider->offset.x;
        y_ = 1 + collider->offset.y;
        z_ = transform->GetPosition().z + collider->offset.z;

        glTranslatef(x_, y_, z_);

        // Dibujar la colisión según su tipo y configuración
        switch (collider->collisionType) {
        case CollisionType::Player:
            glColor3f(0.0f, 1.0f, 0.0f); // Verde para jugador
            break;
        case CollisionType::Enemy:
            glColor3f(1.0f, 0.0f, 0.0f); // Rojo para enemigo
            break;
        case CollisionType::Wall:
            glColor3f(0.0f, 0.0f, 1.0f); // Azul para muro
            break;
        case CollisionType::Bullet:
            glColor3f(1.0f, 0.7f, 0.0f); // Naranja para bala
            break;
        default:
            glColor3f(1.0f, 1.0f, 1.0f); // Blanco para otros tipos
            break;
        }

        glBegin(GL_LINE_LOOP);
        if (collision->GetComponent<Collider2D>()->colliderType == ColliderType::Rect) {
            // Dibujar rectángulo
            if (collider->collisionType == CollisionType::Wall)
            {
                // Modular kit
                float width_ = collision->GetComponent<Collider2D>()->w;
                float height_ = collision->GetComponent<Collider2D>()->h;
                float angle = transform->GetRotationEuler().y;

                // Calculate vertex values when it rotates
                float cosAngle = cosf(angle);
                float sinAngle = sinf(angle);

                float rotatedX1 = -width_ * cosAngle;
                float rotatedZ1 = width_ * sinAngle;
                float rotatedX2 = -width_ * cosAngle + height_ * sinAngle;
                float rotatedZ2 = width_ * sinAngle + height_ * cosAngle;
                float rotatedX3 = height_ * sinAngle;
                float rotatedZ3 = height_ * cosAngle;

                // Draw rectangle
                glVertex3f(rotatedX1, 0.0f, rotatedZ1);
                glVertex3f(rotatedX2, 0.0f, rotatedZ2);
                glVertex3f(rotatedX3, 0.0f, rotatedZ3);
                glVertex3f(0.0f, 0.0f, 0.0f);
            }
            else
            {
                // Pivot middle
                float halfW = collision->GetComponent<Collider2D>()->w / 2.0f;
                float halfH = collision->GetComponent<Collider2D>()->h / 2.0f;
                glVertex3f(-halfW, 0.0f, -halfH);
                glVertex3f(halfW, 0.0f, -halfH);
                glVertex3f(halfW, 0.0f, halfH);
                glVertex3f(-halfW, 0.0f, halfH);
            }
        }
        else if (collision->GetComponent<Collider2D>()->colliderType == ColliderType::Circle) {
            // Dibujar círculo
            const int segments = 30;
            for (int i = 0; i < segments; ++i) {
                float angle = 2.0f * 3.14159f * float(i) / float(segments);
                float x = collision->GetComponent<Collider2D>()->radius * cosf(angle);
                float y = collision->GetComponent<Collider2D>()->radius * sinf(angle);
                glVertex3f(x, 0.0f, y); // Usar y como la altura en perspectiva
            }
        }
        glEnd();

        glPopMatrix();
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
        double distance = DistanceXZ(transformA->GetPosition(), transformB->GetPosition());
        double radiusSum = colliderA->radius + colliderB->radius;
        return (distance <= radiusSum);
    }
    else if (colliderA->colliderType == ColliderType::Rect && colliderB->colliderType == ColliderType::Rect)
    {
        // Check collision between two rectangles
        double xDist = abs(transformA->GetPosition().x - transformB->GetPosition().x);
        double yDist = abs(transformA->GetPosition().y - transformB->GetPosition().y);
        double xOverlap = (colliderA->w + colliderB->w) / 2 - xDist;
        double yOverlap = (colliderA->h + colliderB->h) / 2 - yDist;
        return (xOverlap > 0 && yOverlap > 0);
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

        // Calculate distance between circle center and rectangle center
        double circleDistanceX = std::abs(circleT->GetPosition().x - rectangleT->GetPosition().x);
        double circleDistanceY = std::abs(circleT->GetPosition().z - rectangleT->GetPosition().z);

        // Check if circle is too far away from rectangle to collide
        if (circleDistanceX > (rectangleC->w / 2.0 + circleC->radius)) { return false; }
        if (circleDistanceY > (rectangleC->h / 2.0 + circleC->radius)) { return false; }

        // Check if circle is close enough to collide with rectangle
        if (circleDistanceX <= (rectangleC->w / 2.0)) { return true; }
        if (circleDistanceY <= (rectangleC->h / 2.0)) { return true; }

        // Check if circle collides with corner of rectangle
        double cornerDistanceSq = pow(circleDistanceX - rectangleC->w / 2.0, 2) + pow(circleDistanceY - rectangleC->h / 2.0, 2);
        return (cornerDistanceSq <= pow(circleC->radius, 2));
    }
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
    vec2 collisionVector = { transformA->GetPosition().x - transformB->GetPosition().x, transformA->GetPosition().z - transformB->GetPosition().z };

    // Distance between the centers of the two circles
    vec2 pos2dA = { transformA->GetPosition().x, transformA->GetPosition().z };
    vec2 pos2dB = { transformB->GetPosition().x, transformB->GetPosition().z };
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
    vec2 topLeft = { transformB->GetPosition().x - colliderB->w / 2, transformB->GetPosition().z - colliderB->h / 2 };
    vec2 botRight = { transformB->GetPosition().x + colliderB->w / 2, transformB->GetPosition().z + colliderB->h / 2 };
    vec2 temp = { transformA->GetPosition().x, transformA->GetPosition().z };
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