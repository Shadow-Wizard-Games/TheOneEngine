#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__
#pragma once

#include "Defs.h"
#include "Component.h"
#include "BBox.hpp"
#include "Log.h"

#include <string>
#include <vector>
#include <list>
#include <memory>
#include <utility>

class Camera;

enum class DrawMode
{
    EDITOR,
    GAME,
    BUILD
};

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:

    GameObject(std::string name = "gameObject");
    ~GameObject();

    void Update(double dt);
    void Draw(Camera* camera);
    void DrawUI(Camera* camera, const DrawMode mode);


    // Components
    template <typename TComponent>
    TComponent* GetComponent()
    {
        for (const auto& component : components)
        {
            if (dynamic_cast<TComponent*>(component.get()))
                return static_cast<TComponent*>(component.get());
        }
        return nullptr;
    }

    std::vector<Component*> GetAllComponents();

    template <typename TComponent>
    std::vector<TComponent*> GetComponents() const
    {
        std::vector<TComponent*> matchingComponents;
        for (const auto& component : components)
        {
            if (TComponent* castedComponent = dynamic_cast<TComponent*>(component.get())) {
                matchingComponents.push_back(castedComponent);
            }
        }
        return matchingComponents;
    }

    template <typename TComponent, typename... Args>
    bool AddComponent(Args&&... args)
    {
        Component* component = this->GetComponent<TComponent>();

        // Check for already existing Component
        if (component && component->GetType() != ComponentType::Script)
        {
            LOG(LogType::LOG_WARNING, "Component already applied");
            LOG(LogType::LOG_INFO, "-GameObject [Name: %s] ", name.data());
            LOG(LogType::LOG_INFO, "-Component  [Type: %s] ", component->GetName().data());

            return false;
        }

        std::unique_ptr<Component> newComponent = std::make_unique<TComponent>(shared_from_this(), std::forward<Args>(args)...);
        newComponent->Enable(); // hekbas: Enable the component if necessary?
        components.push_back(std::move(newComponent));

        return true;
    }

    template <typename TComponent>
    bool AddCopiedComponent(TComponent* ref)
    {
        Component* component = this->GetComponent<TComponent>();

        // Check for already existing Component
        if (component != nullptr)
        {
            LOG(LogType::LOG_WARNING, "Component already applied");
            LOG(LogType::LOG_INFO, "-GameObject [Name: %s] ", name.data());
            LOG(LogType::LOG_INFO, "-Component  [Type: %s] ", component->GetName().data());

            return false;
        }

        std::unique_ptr<Component> newComponent = std::make_unique<TComponent>(shared_from_this(), ref);
        newComponent->Enable(); // hekbas: Enable the component if necessary?
        components.push_back(std::move(newComponent));

        return true;
    }

    void RemoveComponent(ComponentType type);

    template <typename TComponent>
    void RemoveComponent(TComponent* compToRemove)
    {
        auto it = std::find_if(components.begin(), components.end(),
            [compToRemove](const std::unique_ptr<Component>& comp) {
                return comp.get() == compToRemove;
            });

        if (it != components.end())
            components.erase(it);
    }


    // AABB
    AABBox CalculateAABB();
    AABBox CalculateAABBWithChildren();
    void DrawAABB();
    void DrawOBB();


    // Get/Set
    bool IsEnabled() const;
    void Enable();
    void Disable();

    // Remove go from parent's children vector
    void Delete();
    void AddToDelete(std::vector<GameObject*>& objectsToDelete);

    std::string GetName() const;
    void SetName(const std::string& name);

    bool IsStatic() const;
    void SetStatic(bool isStatic);
    bool HasCameraComponent();

    void CreateUID();
    uint32 GetUID() { return UID; }

    AABBox GetAABBox() { return aabb; }
    void SetAABBox(AABBox newAABB) { aabb = newAABB; }

    json SaveGameObject();
    void LoadGameObject(const json& gameObjectJSON);

    //Prefab State Methods
    void SetPrefab(const uint32_t& pId, const std::string fileName);
    const uint32_t GetPrefabID() const { return prefabID; }
    void UnpackPrefab();
    bool IsPrefab() const { return prefabID != 0; }
    void SetEditablePrefab(bool editable);
    bool IsEditablePrefab() const { return editablePrefab; }
    void SetPrefabDirty(bool changed);
    bool IsPrefabDirty() const { return isPrefabDirty; }
    std::string GetPrefabName() const { return prefabName; }

public:
    std::weak_ptr<GameObject> parent;
    std::vector<std::shared_ptr<GameObject>> children;
    bool isStatic;
    bool isKeeped;

    // activate this bool if the mesh is transparent so it's rendered okay
    bool hasTransparency;

private:
    std::string name;
    std::vector<std::unique_ptr<Component>> components;
    uint32_t UID;
    bool enabled;
    AABBox aabb;

    //Prefab Vars
    uint32_t prefabID = 0; //Init at 0 for GO that are not Prefab
    bool editablePrefab = true;
    bool isPrefabDirty = false;
    std::string prefabName;
};

#endif // !__GAME_OBJECT_H__