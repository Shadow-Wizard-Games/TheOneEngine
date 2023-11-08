#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Defs.h"

#include <memory>

class GameObject;

enum class ComponentType
{
	Transform,
	Mesh,
	MeshRenderer,
	Camera,
	Unknown
};

class Component
{
public:

    Component(std::shared_ptr<GameObject> containerGO, ComponentType type);
    virtual ~Component();
    virtual void Enable();
    virtual void Disable();
    virtual void Update() {};

    ComponentType GetType() const;
    std::weak_ptr<GameObject> GetContainerGameObject() const;

    bool IsEnabled() const;

    //virtual void CreateInspectorNode() = 0;
    //virtual void CreateInspectorNode();
    virtual void DrawComponent() {};

protected:
    std::weak_ptr<GameObject> containerGO;
    ComponentType type;
    bool enabled;
};

#endif // !__COMPONENT_H__