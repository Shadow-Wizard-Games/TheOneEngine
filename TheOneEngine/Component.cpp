#include "Component.h"
#include "GameObject.h"


Component::Component(std::shared_ptr<GameObject> containerGO, ComponentType type) :
	type(type),
	enabled(true)
{
	std::weak_ptr<GameObject> weakPtr = containerGO;
	this->containerGO = weakPtr;

	CreateUID();
	CreateNameFromType(type);
}

Component::Component(std::shared_ptr<GameObject> containerGO, Component* ref, ComponentType type) :
	type(type),
	enabled(ref->enabled)
{
	std::weak_ptr<GameObject> weakPtr = containerGO;
	this->containerGO = weakPtr;

	CreateUID();
	CreateNameFromType(type);
}

Component::~Component()
{
	Disable();
}

void Component::Enable()
{
	enabled = true;
}

void Component::Disable()
{
	enabled = false;
}

bool Component::IsEnabled() const
{
	return enabled;
}

std::string Component::GetName()
{
	return name;
}

std::string* Component::GetNameToEdit()
{
	return &name;
}

void Component::CreateNameFromType(ComponentType type)
{
	switch (type)
	{
		case ComponentType::Transform: name = "Transform"; break;
		case ComponentType::Camera: name = "Camera"; break;
		case ComponentType::Mesh: name = "Mesh"; break;
		case ComponentType::Texture: name = "Texture"; break;
		case ComponentType::Script: name = "Script"; break;
		case ComponentType::Listener: name = "Listener"; break;
		case ComponentType::Source: name = "Source"; break;
		case ComponentType::Canvas: name = "Canvas"; break;
		case ComponentType::ParticleSystem: name = "ParticleSystem"; break;
		case ComponentType::Unknown: name = "Unknown"; break;
		default: name = "Specified Type was not found"; break;
	}
}

ComponentType Component::GetType() const
{
	return type;
}

std::shared_ptr<GameObject> Component::GetContainerGO() const
{
	return containerGO.lock();
}