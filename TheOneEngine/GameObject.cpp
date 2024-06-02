#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "Mesh.h"
#include "Texture.h"
#include "Script.h"
#include "Collider2D.h"
#include "Listener.h"
#include "AudioSource.h"
#include "Canvas.h"
#include "ParticleSystem.h"
#include "UIDGen.h"
#include "BBox.hpp"
#include "Camera.h"
#include "Light.h"
#include "N_SceneManager.h"

#include "Math.h"
#include "EngineCore.h"
#include "Renderer.h"

GameObject::GameObject(std::string name) :
	name(name),
	parent(),
	children(),
	components(),
	enabled(true),
	isStatic(false),
	isKeeped(false),
	hasTransparency(false),
	aabb()
{
	// hekbas - shared_from_this() must NOT be called in the constructor!!!
	// uncomenting the following line causes undefined behaviour
	//AddComponent(ComponentType::Transform);

	Enable();
}


GameObject::~GameObject() {}

void GameObject::Update(double dt)
{
	// Update enabled Components
	for (const auto& component : components)
	{
		if (component && component->IsEnabled())
			component->Update();
	}

	// Update childs
	for (const auto& child : children)
		child->Update(dt);

	// Recalculate AABBs
	aabb = CalculateAABB();

	// Prefab deletion check to make GO not prefab
	// Not works because only goes through GO update in PLAY state
	/*if (this->IsPrefab())
	{
		std::stringstream prefabPath(ASSETS_PATH);
		prefabPath << "Prefabs\\" << this->name;

		std::ifstream prefabFile(prefabPath.str());

		if (!prefabFile.good())
		{
			prefabID = 0;
			SetPrefab(false);

		}
	}
	return false;*/
}

void GameObject::Draw(Camera* camera)
{
	for (const auto& component : components)
	{
		if (component && component->IsEnabled() && component->GetType() != ComponentType::Canvas)
			component->DrawComponent(camera);
	}

	if (Renderer::GetDrawAABB())
		DrawAABB();
}

void GameObject::DrawUI(Camera* camera, const DrawMode mode)
{
	auto canvas = this->GetComponent<Canvas>();

	if (canvas && canvas->IsEnabled())
		if (mode == DrawMode::GAME || canvas->debugDraw || engine->N_sceneManager->GetSceneIsPlaying())
			canvas->DrawComponent(camera);
}

// Component ----------------------------------------
void GameObject::RemoveComponent(ComponentType type)
{
	for (auto it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->GetType() == type)
		{
			if ((*it)->GetType() == ComponentType::Light)
			{
				//hekbas LIGHT
				//this->GetComponent<Light>()->RemoveLight();
			}
			it = components.erase(it);
			break;
		}
	}
}


// AABB -------------------------------------
AABBox GameObject::CalculateAABB()
{
	AABBox aabb = this->aabb;

	for (const auto& child : children)
	{
		AABBox childAABB = (child.get()->GetComponent<Transform>()->GetTransform() * child.get()->CalculateAABB()).AABB();
		aabb.min = (glm::min)(aabb.min, childAABB.min);
		aabb.max = (glm::max)(aabb.max, childAABB.max);
	}

	return aabb;
}

AABBox GameObject::CalculateAABBWithChildren()
{
	AABBox aabb = CalculateAABB();
	// Transform the AABB to the coordinate system of the parent objects
	mat4 parentTransform = GetComponent<Transform>()->CalculateWorldTransform();
	OBBox obb = parentTransform * aabb;
	return obb.AABB();
}

static inline void glVec3(const vec3& v) { glVertex3dv(&v.x); }

void GameObject::DrawAABB()
{
	Renderer2D::DrawLine(BT::WORLD, aabb.a(), aabb.b(), { 1.0f, 1.0f, 1.0f, 0.8 });
	Renderer2D::DrawLine(BT::WORLD, aabb.b(), aabb.c(), { 1.0f, 1.0f, 1.0f, 0.8 });
	Renderer2D::DrawLine(BT::WORLD, aabb.c(), aabb.d(), { 1.0f, 1.0f, 1.0f, 0.8 });
	Renderer2D::DrawLine(BT::WORLD, aabb.d(), aabb.a(), { 1.0f, 1.0f, 1.0f, 0.8 });

	Renderer2D::DrawLine(BT::WORLD, aabb.e(), aabb.f(), { 1.0f, 1.0f, 1.0f, 0.8 });
	Renderer2D::DrawLine(BT::WORLD, aabb.f(), aabb.g(), { 1.0f, 1.0f, 1.0f, 0.8 });
	Renderer2D::DrawLine(BT::WORLD, aabb.g(), aabb.h(), { 1.0f, 1.0f, 1.0f, 0.8 });
	Renderer2D::DrawLine(BT::WORLD, aabb.h(), aabb.e(), { 1.0f, 1.0f, 1.0f, 0.8 });

	Renderer2D::DrawLine(BT::WORLD, aabb.h(), aabb.d(), { 1.0f, 1.0f, 1.0f, 0.8 });
	Renderer2D::DrawLine(BT::WORLD, aabb.f(), aabb.b(), { 1.0f, 1.0f, 1.0f, 0.8 });
	Renderer2D::DrawLine(BT::WORLD, aabb.g(), aabb.c(), { 1.0f, 1.0f, 1.0f, 0.8 });
}

void GameObject::DrawOBB()
{
	/*glColor3f(1, 0, 1);
	glLineWidth(2);
	vec3f* obb_points = nullptr;*/
	//obb.GetCornerPoints(obb_points);
	//
	//glBegin(GL_LINES);
	//
	//glVertex3f(obb.CornerPoint(0).x, obb.CornerPoint(0).y, obb.CornerPoint(0).z);
	//glVertex3f(obb.CornerPoint(1).x, obb.CornerPoint(1).y, obb.CornerPoint(1).z);
	//
	//glVertex3f(obb.CornerPoint(0).x, obb.CornerPoint(0).y, obb.CornerPoint(0).z);
	//glVertex3f(obb.CornerPoint(4).x, obb.CornerPoint(4).y, obb.CornerPoint(4).z);
	//
	//glVertex3f(obb.CornerPoint(0).x, obb.CornerPoint(0).y, obb.CornerPoint(0).z);
	//glVertex3f(obb.CornerPoint(2).x, obb.CornerPoint(2).y, obb.CornerPoint(2).z);
	//
	//glVertex3f(obb.CornerPoint(2).x, obb.CornerPoint(2).y, obb.CornerPoint(2).z);
	//glVertex3f(obb.CornerPoint(3).x, obb.CornerPoint(3).y, obb.CornerPoint(3).z);
	//
	//glVertex3f(obb.CornerPoint(1).x, obb.CornerPoint(1).y, obb.CornerPoint(1).z);
	//glVertex3f(obb.CornerPoint(5).x, obb.CornerPoint(5).y, obb.CornerPoint(5).z);
	//
	//glVertex3f(obb.CornerPoint(1).x, obb.CornerPoint(1).y, obb.CornerPoint(1).z);
	//glVertex3f(obb.CornerPoint(3).x, obb.CornerPoint(3).y, obb.CornerPoint(3).z);
	//
	//glVertex3f(obb.CornerPoint(4).x, obb.CornerPoint(4).y, obb.CornerPoint(4).z);
	//glVertex3f(obb.CornerPoint(5).x, obb.CornerPoint(5).y, obb.CornerPoint(5).z);
	//
	//glVertex3f(obb.CornerPoint(4).x, obb.CornerPoint(4).y, obb.CornerPoint(4).z);
	//glVertex3f(obb.CornerPoint(6).x, obb.CornerPoint(6).y, obb.CornerPoint(6).z);
	//
	//glVertex3f(obb.CornerPoint(2).x, obb.CornerPoint(2).y, obb.CornerPoint(2).z);
	//glVertex3f(obb.CornerPoint(6).x, obb.CornerPoint(6).y, obb.CornerPoint(6).z);
	//
	//glVertex3f(obb.CornerPoint(5).x, obb.CornerPoint(5).y, obb.CornerPoint(5).z);
	//glVertex3f(obb.CornerPoint(7).x, obb.CornerPoint(7).y, obb.CornerPoint(7).z);
	//
	//glVertex3f(obb.CornerPoint(6).x, obb.CornerPoint(6).y, obb.CornerPoint(6).z);
	//glVertex3f(obb.CornerPoint(7).x, obb.CornerPoint(7).y, obb.CornerPoint(7).z);
	//
	//glVertex3f(obb.CornerPoint(3).x, obb.CornerPoint(3).y, obb.CornerPoint(3).z);
	//glVertex3f(obb.CornerPoint(7).x, obb.CornerPoint(7).y, obb.CornerPoint(7).z);
	//
	//glLineWidth(1);
	//glEnd();
}


// Get/Set ------------------------------------------
bool GameObject::IsEnabled() const
{
	return enabled;
}

void GameObject::Enable()
{
	enabled = true;

	for (const auto& component : components)
		component->Enable();

	for (const auto& child : children)
		child->Enable();
}

void GameObject::Disable()
{
	enabled = false;

	for (const auto& component : components)
		component->Disable();

	for (const auto& child : children)
		child->Disable();
}

void GameObject::Delete()
{
	auto it = std::find(parent.lock()->children.begin(), parent.lock()->children.end(), shared_from_this());

	if (it != parent.lock()->children.end())
  		parent.lock()->children.erase(it);	
}

void GameObject::AddToDelete(std::vector<GameObject*>& objectsToDelete)
{
	for (auto go : objectsToDelete)
		if (go == this) return;

	objectsToDelete.push_back(this);
}

std::vector<Component*> GameObject::GetAllComponents()
{
	std::vector<Component*> tempComponents;
	for (const auto& item : components)
	{
		tempComponents.push_back(item.get());
	}
	return tempComponents;
}

std::string GameObject::GetName() const
{
	return name;
}

void GameObject::SetName(const std::string& newName)
{
	name = newName;
}

bool GameObject::IsStatic() const
{
	return isStatic;
}

void GameObject::SetStatic(bool staticFlag)
{
	isStatic = staticFlag;
}

bool GameObject::HasCameraComponent()
{
	return this->GetComponent<Camera>();
}

void GameObject::CreateUID()
{
	UID = UIDGen::GenerateUID();
}


// Save/Load ------------------------------------------
json GameObject::SaveGameObject()
{
	json gameObjectJSON;

	if (auto pGO = parent.lock())
	{
		gameObjectJSON["ParentUID"] = pGO.get()->UID;
	}

	gameObjectJSON["UID"] = UID;
	gameObjectJSON["Name"] = name;
	gameObjectJSON["Static"] = isStatic;
	gameObjectJSON["Keeped"] = isKeeped;
	gameObjectJSON["Enabled"] = enabled;
	gameObjectJSON["HasTransparency"] = hasTransparency;
	
	//Save prefab variables
	if (prefabID != 0)
	{
		gameObjectJSON["PrefabID"] = prefabID;
		gameObjectJSON["EditablePrefab"] = editablePrefab;
		gameObjectJSON["PrefabDirty"] = isPrefabDirty;
		gameObjectJSON["PrefabName"] = prefabName;
	}

	if (!components.empty())
	{
		json componentsJSON;

		for (const auto& component : components)
		{
			componentsJSON.push_back(component.get()->SaveComponent());
		}
		gameObjectJSON["Components"] = componentsJSON;
	}

	if (!children.empty())
	{
		json childrenGOJSON;

		for (const auto& go : children)
		{
			childrenGOJSON.push_back(go.get()->SaveGameObject());
		}
		gameObjectJSON["GameObjects"] = childrenGOJSON;
	}

	return gameObjectJSON;
}

void GameObject::LoadGameObject(const json& gameObjectJSON)
{
	// Load basic properties
	if (gameObjectJSON.contains("UID"))
	{
		UID = gameObjectJSON["UID"];
	}

	if (gameObjectJSON.contains("Name"))
	{
		name = gameObjectJSON["Name"];
	}

	if (gameObjectJSON.contains("Static"))
	{
		isStatic = gameObjectJSON["Static"];
	}

	if (gameObjectJSON.contains("Keeped"))
	{
		isKeeped = gameObjectJSON["Keeped"];
	}

	if (gameObjectJSON.contains("Enabled"))
	{
		enabled = gameObjectJSON["Enabled"];
	}

	if (gameObjectJSON.contains("HasTransparency"))
	{
		hasTransparency = gameObjectJSON["HasTransparency"];
	}

	if (gameObjectJSON.contains("PrefabID"))
	{
		prefabID = gameObjectJSON["PrefabID"];

		if (gameObjectJSON.contains("EditablePrefab"))
		{
			editablePrefab = gameObjectJSON["EditablePrefab"];
		}
		if (gameObjectJSON.contains("PrefabDirty"))
		{
			isPrefabDirty = gameObjectJSON["PrefabDirty"];
		}
		if (gameObjectJSON.contains("PrefabName"))
		{
			prefabName = gameObjectJSON["PrefabName"];
		}
	}

	// Load components
	if (gameObjectJSON.contains("Components"))
	{
		const json& componentsJSON = gameObjectJSON["Components"];

		for (const auto& componentJSON : componentsJSON)
		{
			if (componentJSON["Type"] == (int)ComponentType::Transform)
			{
				this->AddComponent<Transform>();
				this->GetComponent<Transform>()->LoadComponent(componentJSON);
			}
			else if (componentJSON["Type"] == (int)ComponentType::Camera)
			{
				this->AddComponent<Camera>();
				this->GetComponent<Camera>()->LoadComponent(componentJSON);
			}
			else if (componentJSON["Type"] == (int)ComponentType::Mesh)
			{
				this->AddComponent<Mesh>();
				this->GetComponent<Mesh>()->LoadComponent(componentJSON);
			}
			else if (componentJSON["Type"] == (int)ComponentType::Script)
			{
				this->AddComponent<Script>(componentJSON["ScriptName"]);
				this->GetComponent<Script>()->LoadComponent(componentJSON);
			}
			else if (componentJSON["Type"] == (int)ComponentType::Listener)
			{
				this->AddComponent<Listener>();
				this->GetComponent<Listener>()->LoadComponent(componentJSON);

				// Check if the gameobject has an Audio Source
				if (this->GetComponent<AudioSource>() == nullptr)
					this->GetComponent<Listener>()->goID = audioManager->audio->RegisterGameObject(this->GetName());
				else
					this->GetComponent<Listener>()->goID = this->GetComponent<AudioSource>()->goID;
				audioManager->AddAudioObject((std::shared_ptr<AudioComponent>)this->GetComponent<Listener>());
				audioManager->audio->SetDefaultListener(this->GetComponent<Listener>()->goID);
			}
			else if (componentJSON["Type"] == (int)ComponentType::AudioSource)
			{
				this->AddComponent<AudioSource>();
				this->GetComponent<AudioSource>()->LoadComponent(componentJSON);

				// Check if the gameobject has a Listener
				if (this->GetComponent<Listener>() == nullptr)
					this->GetComponent<AudioSource>()->goID = audioManager->audio->RegisterGameObject(this->GetName());
				else
					this->GetComponent<AudioSource>()->goID = this->GetComponent<Listener>()->goID;

				audioManager->AddAudioObject((std::shared_ptr<AudioComponent>)this->GetComponent<AudioSource>());
			}
			else if (componentJSON["Type"] == (int)ComponentType::Collider2D)
			{
				this->AddComponent<Collider2D>();
				this->GetComponent<Collider2D>()->LoadComponent(componentJSON);
			}
			else if (componentJSON["Type"] == (int)ComponentType::Canvas)
			{
				this->AddComponent<Canvas>();
				this->GetComponent<Canvas>()->LoadComponent(componentJSON);
			}
			else if (componentJSON["Type"] == (int)ComponentType::ParticleSystem)
			{
				this->AddComponent<ParticleSystem>();
				this->GetComponent<ParticleSystem>()->LoadComponent(componentJSON);
			}
			else if (componentJSON["Type"] == (int)ComponentType::Light)
			{
				this->AddComponent<Light>((LightType)componentJSON["LightType"]);
				this->GetComponent<Light>()->LoadComponent(componentJSON);
			}
		}
	}

	// Load child game objects
	if (gameObjectJSON.contains("GameObjects"))
	{
		const json& childrenGOJSON = gameObjectJSON["GameObjects"];

		for (const auto& childJSON : childrenGOJSON)
		{
			auto childGameObject = std::make_shared<GameObject>("Empty GO");
			childGameObject->LoadGameObject(childJSON);

			// Add the loaded child game object to the current game object
			childGameObject.get()->parent = this->weak_from_this().lock();
			this->children.emplace_back(childGameObject);
		}
	}
}

void GameObject::SetPrefab(const uint32_t& pID, const std::string fileName)
{
	prefabID = pID;
	prefabName = fileName;
}

void GameObject::UnpackPrefab()
{
	if (IsPrefab())
	{
		SetPrefab(0, "");
		editablePrefab = true; 
		isPrefabDirty = false;
	}
		
}

void GameObject::SetEditablePrefab(bool editable)
{
	if (IsPrefab())
	{
		editablePrefab = editable;
		for (auto item = children.begin(); item != children.end(); ++item) {
			if (*item != nullptr) {
				(*item).get()->SetEditablePrefab(editable);
			}
		}
	}
}

void GameObject::SetPrefabDirty(bool changed)
{
	if (IsPrefab())
	{
		isPrefabDirty = changed;
		if (parent.lock() != engine->N_sceneManager->currentScene->GetRootSceneGO() && parent.lock().get()->GetPrefabID() == this->prefabID)
		{
			parent.lock().get()->SetPrefabDirty(changed);
		}
		for (auto item = children.begin(); item != children.end(); ++item) {
			if (*item != nullptr) {
				(*item).get()->SetPrefabDirty(changed);
			}
		}
	}
}
