#include "MonoRegisterer.h"
#include "MonoManager.h"
#include "Defs.h"

#include "EngineCore.h"
#include "Component.h"
#include "Transform.h"
#include "Canvas.h"
#include "ImageUI.h"
#include "ButtonImageUI.h"
#include "CheckerUI.h"
#include "SliderUI.h"
#include "TextUI.h"
#include "Collider2D.h"
#include "ParticleSystem.h"
#include "N_SceneManager.h"

#include <glm/vec3.hpp>

//Constructors
static GameObject* GetGameObjectPtr()
{
	return MonoManager::GetCurrentGameObjectPtr();
}

//Input
static bool GetKeyboardButton(int id)
{
	return engine->inputManager->GetKey(id) == KEY_REPEAT;
}

static bool GetControllerButton(int controllerButton, int gamePad)
{
	auto inputToPass = (SDL_GameControllerButton)controllerButton;

	auto result = engine->inputManager->GetGamepadButton(gamePad, inputToPass);

	return result == InputManagerNamespace::KEY_DOWN;
}
static void GetControllerJoystick(int joystick, vec2f* joyResult, int gamePad)
{
	if (joystick) //value is 1, so it means right
	{
		joyResult->x = engine->inputManager->pads[gamePad].right_x;
		joyResult->y = engine->inputManager->pads[gamePad].right_y;
	}
	else
	{
		joyResult->x = engine->inputManager->pads[gamePad].left_x;
		joyResult->y = engine->inputManager->pads[gamePad].left_y;
	}
}

//Transform
static bool TransformCheck(GameObject* GOptr)
{
	bool ret = false;
	if (GOptr->GetComponent<Transform>())
		ret = true;

	return ret;
}

static vec3f GetPosition(GameObject* GOptr)
{
	return (vec3f)GOptr->GetComponent<Transform>()->GetPosition();
}

static void SetPosition(GameObject* GOptr, vec3f* position)
{
	GOptr->GetComponent<Transform>()->SetPosition((vec3)*position);
}

static vec3f GetRotation(GameObject* GOptr)
{
	return (vec3f)GOptr->GetComponent<Transform>()->GetRotationEuler();
}
static void SetRotation(GameObject* GOptr, vec3f* rotation)
{
	GOptr->GetComponent<Transform>()->SetRotation((vec3)*rotation);
}

static vec3f GetScale(GameObject* GOptr)
{
	return (vec3f)GOptr->GetComponent<Transform>()->GetScale();
}
static void SetScale(GameObject* GOptr, vec3f* scale)
{
	GOptr->GetComponent<Transform>()->SetScale((vec3)*scale);
}

static void Translate(GameObject* GOptr, vec3f* finalPos)
{
	//This implementation is temporary, engine Transform.Translate is not working properly.

	GOptr->GetComponent<Transform>()->SetPosition((vec3)*finalPos, HandleSpace::GLOBAL);
}
static void Rotate(GameObject* GOptr, vec3f* increment)
{
	GOptr->GetComponent<Transform>()->Rotate((vec3)*increment, HandleSpace::LOCAL);
}

static vec3f GetTransformForward(GameObject* GOptr)
{
	return (vec3f)GOptr->GetComponent<Transform>()->GetForward();
}
static vec3f GetTransformRight(GameObject* GOptr)
{
	return (vec3f)GOptr->GetComponent<Transform>()->GetRight();
}

//GameObject
static GameObject* InstantiateBullet(vec3f* initialPosition, vec3f* direction)
{
	engine->N_sceneManager->CreateMeshGO("Assets/Meshes/SM_Cube.fbx");
	GameObject* go = engine->N_sceneManager->objectsToAdd.back().get();

	SetPosition(go, initialPosition);
	SetRotation(go, direction);

	go->AddScript("Bullet");
	go->AddComponent<Collider2D>();
	go->GetComponent<Collider2D>()->colliderType = ColliderType::Circle;
	go->GetComponent<Collider2D>()->collisionType = CollisionType::Bullet;
	go->GetComponent<Collider2D>()->radius = 0.4f;
	engine->collisionSolver->LoadCollisions(engine->N_sceneManager->objectsToAdd.back());
	return go;
}

static GameObject* InstantiateXenomorph(vec3f* initialPosition, vec3f* direction, vec3f* scale)
{
	engine->N_sceneManager->CreateMeshGO("Assets/Meshes/SK_Facehugger.fbx");
	GameObject* go = engine->N_sceneManager->objectsToAdd.back().get();

	SetPosition(go, initialPosition);
	SetRotation(go, direction);
	SetScale(go, scale);

	go->AddComponent<Collider2D>();
	go->GetComponent<Collider2D>()->colliderType = ColliderType::Circle;
	go->GetComponent<Collider2D>()->collisionType = CollisionType::Enemy;
	go->GetComponent<Collider2D>()->radius = 30.0f;

	go->AddComponent<AudioSource>();
	
	go->AddScript("FaceHuggerBehaviour");

	return go;
}

static MonoString* GetGameObjectName(GameObject* GOptr)
{
	return mono_string_new(MonoManager::GetAppDomain(), GOptr->GetName().c_str());
}

static void DestroyGameObject(GameObject* objectToDestroy)
{
	objectToDestroy->AddToDelete(engine->N_sceneManager->objectsToDelete);
}

static GameObject* RecursiveFindGO(std::string _name, GameObject* refGO)
{
	GameObject* returnedGO = nullptr;

	for (auto go : refGO->children)
	{
		if (go->GetName() == _name)
		{
			return go.get();
		}
		returnedGO = RecursiveFindGO(_name, go.get());

		if (returnedGO != nullptr)
			break;
	}
	return returnedGO;
}

static GameObject* FindGameObject(MonoString* monoString)
{
	std::string name = MonoRegisterer::MonoStringToUTF8(monoString);

	return RecursiveFindGO(name, engine->N_sceneManager->currentScene->GetRootSceneGO().get());
}

static GameObject* FindGameObjectInChildren(GameObject* refGO, MonoString* monoString)
{
	std::string name = MonoRegisterer::MonoStringToUTF8(monoString);

	return RecursiveFindGO(name, refGO);
}

static void* ComponentCheck(GameObject* GOptr, int componentType, MonoString* scriptName = nullptr)
{
	ComponentType type = (ComponentType)componentType;

	if (type != ComponentType::Script)
	{
		for (auto comp : GOptr->GetAllComponents())
		{
			if (comp->GetType() == type)
			{
				return &comp->enabled;
			}
		}
	}
	else if (scriptName != nullptr)
	{
		std::string scriptToFind = MonoRegisterer::MonoStringToUTF8(scriptName);
		
		for (auto comp: GOptr->GetAllComponents())
		{
			if (comp->GetType() == ComponentType::Script)
			{
				Script* scriptToAccess = (Script*)comp;
				if (scriptToAccess->scriptName == scriptToFind)
				{
					return scriptToAccess;
				}
			}
		}
	}
	return nullptr;
}

static void* GetScript(GameObject* GOptr, MonoString* scriptName)
{
	std::string scriptToFind = MonoRegisterer::MonoStringToUTF8(scriptName);

	for (auto comp : GOptr->GetAllComponents())
	{
		if (comp->GetType() == ComponentType::Script)
		{
			Script* scriptToAccess = (Script*)comp;
			if (scriptToAccess->scriptName == scriptToFind)
			{
				return MonoManager::CallScriptFunction(scriptToAccess->monoBehaviourInstance, "GetClassInstance");
			}
		}
	}
}

static void Disable(GameObject* GOtoDisable)
{
	GOtoDisable->Disable();
}

static void Enable(GameObject* GOtoEnable)
{
	GOtoEnable->Enable();
}

//Scene Management
static void LoadScene(MonoString* sceneName, bool keep)
{
	std::string name = MonoRegisterer::MonoStringToUTF8(sceneName);

	engine->N_sceneManager->LoadScene(name, keep);
}

static MonoString* GetCurrentSceneName()
{
	return mono_string_new(MonoManager::GetAppDomain(), engine->N_sceneManager->currentScene->GetSceneName().c_str());
}

static void CreatePrefab(MonoString* prefabName, vec3f* position)
{
	std::string MprefabName = MonoRegisterer::MonoStringToUTF8(prefabName);

	engine->N_sceneManager->CreatePrefabWithName(MprefabName, *position);
}

//User Interface
static void CanvasEnableToggle(GameObject* containerGO)
{
	if (containerGO->GetComponent<Canvas>()->IsEnabled() == true)
	{
		containerGO->GetComponent<Canvas>()->Disable();
	}
	else
	{
		containerGO->GetComponent<Canvas>()->Enable();
	}
}

static void ToggleChecker(GameObject* containerGO, bool value, MonoString* nameM)
{
	std::string name = MonoRegisterer::MonoStringToUTF8(nameM);

	std::vector<ItemUI*> uiElements = containerGO->GetComponent<Canvas>()->GetUiElements();
	for (size_t i = 0; i < uiElements.size(); i++)
	{
		if (uiElements[i]->GetType() == UiType::CHECKER && uiElements[i]->GetName() == name)
		{
			containerGO->GetComponent<Canvas>()->GetItemUI<CheckerUI>(uiElements[i]->GetID())->SetChecker(value);
		}
	}
}

static void PrintItemUI(GameObject* containerGO, bool value, MonoString* nameM)
{
	std::string name = MonoRegisterer::MonoStringToUTF8(nameM);

	std::vector<ItemUI*> uiElements = containerGO->GetComponent<Canvas>()->GetUiElements();
	for (size_t i = 0; i < uiElements.size(); i++)
	{
		if (uiElements[i]->GetName() == name)
		{
			containerGO->GetComponent<Canvas>()->GetItemUI<ItemUI>(uiElements[i]->GetID())->SetPrint(value);
		}
	}
}

static int GetSelectedButton(GameObject* containerGO)
{
	std::vector<ItemUI*> uiElements = containerGO->GetComponent<Canvas>()->GetUiElements();
	int ret = -1;
	for (size_t i = 0; i < uiElements.size(); i++)
	{
		ButtonImageUI* tempButton = (ButtonImageUI*)uiElements[i];
		if (uiElements[i]->GetType() == UiType::BUTTONIMAGE && tempButton->IsRealButton())
		{
			ret++;
			if (uiElements[i]->GetState() == UiState::HOVERED)
				return ret;
		}
	}
	return ret;
}

static int GetSelected(GameObject* containerGO)
{
	std::vector<ItemUI*> uiElements = containerGO->GetComponent<Canvas>()->GetUiElements();
	int ret = -1;
	for (size_t i = 0; i < uiElements.size(); i++)
	{
		if (uiElements[i]->GetType() != UiType::IMAGE && uiElements[i]->GetType() != UiType::TEXT)
		{
			ret++;
			if (uiElements[i]->GetState() == UiState::HOVERED)
			{
				if (uiElements[i]->GetType() == UiType::BUTTONIMAGE)
				{
					ButtonImageUI* tempButton = (ButtonImageUI*)uiElements[i];
					if (tempButton->IsRealButton())
					{
						return ret;
					}
				}
				else
				{
					return ret;
				}
			}

		}
	}
	return ret;
}

static void SetUiItemState(GameObject* containerGO, int state, MonoString* name)
{
	std::string itemName = MonoRegisterer::MonoStringToUTF8(name);
	std::vector<ItemUI*> uiElements = containerGO->GetComponent<Canvas>()->GetUiElements();
	for (size_t i = 0; i < uiElements.size(); i++)
	{
		if ((uiElements[i]->GetType() != UiType::IMAGE && uiElements[i]->GetType() != UiType::TEXT) && uiElements[i]->GetName() == itemName)
		{
			(uiElements[i]->SetState((UiState)state));
		}
	}
}

static void MoveSelectedButton(GameObject* containerGO, int direction)
{
	if (direction != 0)
	{
		std::vector<ItemUI*> uiElements = containerGO->GetComponent<Canvas>()->GetUiElements();

		for (size_t i = 0; i < uiElements.size(); i++)
		{
			ButtonImageUI* tempButton = (ButtonImageUI*)uiElements[i];
			if (uiElements[i]->GetType() == UiType::BUTTONIMAGE && tempButton->IsRealButton() && uiElements[i]->GetState() == UiState::HOVERED)
			{
				int val = 1;
				if (direction < 0)
					val *= -1;

				for (int j = i + val; j != i; j += val)
				{
					if (j < 0)
						j = uiElements.size() - 1;
					else if (j >= uiElements.size())
						j = 0;

					if (uiElements[j]->GetType() == UiType::BUTTONIMAGE)
					{
						if (direction != 0)
							direction += (val * -1);

						if (direction == 0)
						{
							uiElements[i]->SetState(UiState::IDLE);
							uiElements[j]->SetState(UiState::HOVERED);
							break;
						}
					}
				}
				break;
			}
		}
	}
}

static void MoveSelection(GameObject* containerGO, int direction)
{
	if (direction != 0)
	{
		std::vector<ItemUI*> uiElements = containerGO->GetComponent<Canvas>()->GetUiElements();

		for (size_t i = 0; i < uiElements.size(); i++)
		{
			if ((uiElements[i]->GetType() != UiType::IMAGE && uiElements[i]->GetType() != UiType::TEXT) && uiElements[i]->GetState() == UiState::HOVERED)
			{
				if (uiElements[i]->GetType() == UiType::BUTTONIMAGE)
				{
					ButtonImageUI* tempButton = (ButtonImageUI*)uiElements[i];
					if (!tempButton->IsRealButton()) continue;
				}
				int val = 1;
				if (direction < 0)
					val *= -1;

				for (int j = i + val; j != i; j += val)
				{
					if (j < 0)
						j = uiElements.size() - 1;
					else if (j >= uiElements.size())
						j = 0;

					if (uiElements[j]->GetType() != UiType::IMAGE)
					{
						if (uiElements[j]->GetType() == UiType::BUTTONIMAGE)
						{
							ButtonImageUI* tempButton = (ButtonImageUI*)uiElements[j];
							if (!tempButton->IsRealButton()) continue;
						}
						if (direction != 0)
							direction += (val * -1);

						if (direction == 0)
						{
							uiElements[i]->SetState(UiState::IDLE);
							uiElements[j]->SetState(UiState::HOVERED);
							break;
						}
					}
				}
				break;
			}
		}
	}
}

static void ChangeSectImg(GameObject* containerGO, MonoString* name, int x, int y, int w, int h)
{
	std::string itemName = MonoRegisterer::MonoStringToUTF8(name);
	std::vector<ItemUI*> uiElements = containerGO->GetComponent<Canvas>()->GetUiElements();
	for (size_t i = 0; i < uiElements.size(); i++)
	{
		if (uiElements[i]->GetType() == UiType::IMAGE && uiElements[i]->GetName() == itemName)
		{
			ImageUI* ui = containerGO->GetComponent<Canvas>()->GetItemUI<ImageUI>(uiElements[i]->GetID());
			ui->SetSectSize(x, y, w, h);
		}
	}
}

static int GetSliderValue(GameObject* containerGO, MonoString* name)
{
	std::string itemName = MonoRegisterer::MonoStringToUTF8(name);
	std::vector<ItemUI*> uiElements = containerGO->GetComponent<Canvas>()->GetUiElements();
	for (size_t i = 0; i < uiElements.size(); i++)
	{
		if (uiElements[i]->GetType() == UiType::SLIDER && uiElements[i]->GetName() == itemName)
		{
			SliderUI* ui = containerGO->GetComponent<Canvas>()->GetItemUI<SliderUI>(uiElements[i]->GetID());
			return ui->GetValue();
		}
	}

	return -1;
}

static int GetSliderMaxValue(GameObject* containerGO, MonoString* name)
{
	std::string itemName = MonoRegisterer::MonoStringToUTF8(name);
	std::vector<ItemUI*> uiElements = containerGO->GetComponent<Canvas>()->GetUiElements();
	for (size_t i = 0; i < uiElements.size(); i++)
	{
		if (uiElements[i]->GetType() == UiType::SLIDER && uiElements[i]->GetName() == itemName)
		{
			SliderUI* ui = containerGO->GetComponent<Canvas>()->GetItemUI<SliderUI>(uiElements[i]->GetID());
			return ui->GetMaxValue();
		}
	}

	return -1;
}

static void SetSliderValue(GameObject* containerGO, int value, MonoString* name)
{
	std::string itemName = MonoRegisterer::MonoStringToUTF8(name);
	std::vector<ItemUI*> uiElements = containerGO->GetComponent<Canvas>()->GetUiElements();
	for (size_t i = 0; i < uiElements.size(); i++)
	{
		if (uiElements[i]->GetType() == UiType::SLIDER && uiElements[i]->GetName() == itemName)
		{
			SliderUI* ui = containerGO->GetComponent<Canvas>()->GetItemUI<SliderUI>(uiElements[i]->GetID());
			ui->SetValue(value);
			break;
		}
	}
}

static void SetTextString(GameObject* containerGO, MonoString* text, MonoString* name)
{
	std::string itemName = MonoRegisterer::MonoStringToUTF8(name);
	std::vector<ItemUI*> uiElements = containerGO->GetComponent<Canvas>()->GetUiElements();
	for (size_t i = 0; i < uiElements.size(); i++)
	{
		if (uiElements[i]->GetType() == UiType::TEXT && uiElements[i]->GetName() == itemName)
		{
			TextUI* ui = containerGO->GetComponent<Canvas>()->GetItemUI<TextUI>(uiElements[i]->GetID());
			ui->SetText(MonoRegisterer::MonoStringToUTF8(text));
			break;
		}
	}
}

static string GetTextString(GameObject* containerGO, MonoString* name)
{
	std::string itemName = MonoRegisterer::MonoStringToUTF8(name);
	std::vector<ItemUI*> uiElements = containerGO->GetComponent<Canvas>()->GetUiElements();
	for (size_t i = 0; i < uiElements.size(); i++)
	{
		if (uiElements[i]->GetType() == UiType::TEXT && uiElements[i]->GetName() == itemName)
		{
			TextUI* ui = containerGO->GetComponent<Canvas>()->GetItemUI<TextUI>(uiElements[i]->GetID());
			return ui->GetText();
		}
	}

	return "TextUI element not found";
}

//Helpers
static float GetAppDeltaTime()
{
	return (float)engine->dt;
}
static void ExitApplication()
{
	engine->inputManager->shutDownEngine = true;
}

//Debug
static void ScriptingLog(MonoString* monoString, LogType logType)
{
	std::string message = MonoRegisterer::MonoStringToUTF8(monoString);
	LOG(logType, message.c_str());
}
static void DrawWireCircle(vec3f position, float radius, vec3f colorNormalized)
{
	DebugShape shapeToAdd;

	shapeToAdd.center = position;
	shapeToAdd.color = colorNormalized;

	//Set points which will define the line
	const int segments = 30;
	for (int i = 0; i < segments; ++i) {
		float angle = 2.0f * 3.14159f * float(i) / float(segments);
		float x = radius * cosf(angle);
		float y = radius * sinf(angle);
		shapeToAdd.points.push_back(vec3f(x, 0, y));
	}

	engine->monoManager->debugShapesQueue.push_back(shapeToAdd);
}
static void DrawWireSquare()
{

}
static void DrawWireSphere(vec3f position, float radius, vec3f colorNormalized)
{
	DebugShape shapeToAdd;

	shapeToAdd.center = position;
	shapeToAdd.color = colorNormalized;

	//Set points which will define the line
	const int segments = 40;
	for (int i = 0; i < segments; ++i) {
		float angle = 2.0f * 3.14159f * float(i) / float(segments);
		float x = radius * cosf(angle);
		float y = radius * sinf(angle);
		shapeToAdd.points.push_back(vec3f(x, 0, y));
	}
	for (int i = 0; i < segments; ++i) {
		float angle = 2.0f * 3.14159f * float(i) / float(segments);
		float x = radius * cosf(angle);
		float y = radius * sinf(angle);
		shapeToAdd.points.push_back(vec3f(x, y, 0));
	}
	
	//Go to starting spot for last circle
	for (int i = 0; i < segments / 4; ++i) {
		float angle = 2.0f * 3.14159f * float(i) / float(segments);
		float x = radius * cosf(angle);
		float y = radius * sinf(angle);
		shapeToAdd.points.push_back(vec3f(x, 0, y));
	}

	for (int i = 0; i < segments; ++i) {
		float angle = 2.0f * 3.14159f * float(i) / float(segments);
		float x = radius * cosf(angle);
		float y = radius * sinf(angle);
		shapeToAdd.points.push_back(vec3f(0, y, x));
	}

	//Go back to starting spot for entire shape
	for (int i = segments / 4; i > 0; --i) {
		float angle = 2.0f * 3.14159f * float(i) / float(segments);
		float x = radius * cosf(angle);
		float y = radius * sinf(angle);
		shapeToAdd.points.push_back(vec3f(x, 0, y));
	}

	engine->monoManager->debugShapesQueue.push_back(shapeToAdd);
}
static void DrawWireCube()
{

}

static void ToggleCollidersDraw()
{
	engine->drawCollisions = !engine->drawCollisions;
}

static void ToggleGridDraw()
{
	engine->drawGrid = !engine->drawGrid;
}

// Particle System
static void PlayPS(GameObject* GOptr)
{
	GOptr->GetComponent<ParticleSystem>()->Play();
}

static void PausePS(GameObject* GOptr)
{
	GOptr->GetComponent<ParticleSystem>()->Pause();
}

static void ReplayPS(GameObject* GOptr)
{
	GOptr->GetComponent<ParticleSystem>()->Replay();
}

static void StopPS(GameObject* GOptr)
{
	GOptr->GetComponent<ParticleSystem>()->Stop();
}

// Audio Manager
static void PlayAudioSource(GameObject* GOptr, uint audio) {
	AkUInt32 myAkUInt32 = static_cast<AkUInt32>(audio);

	audioManager->PlayAudio(GOptr->GetComponent<AudioSource>(), audio);
}

static void StopAudioSource(GameObject* GOptr, uint audio) {
	AkUInt32 myAkUInt32 = static_cast<AkUInt32>(audio);

	audioManager->StopAudio(GOptr->GetComponent<AudioSource>(), audio);
}

// Collider2D
static float GetColliderRadius(GameObject* GOptr)
{
	return (float)GOptr->GetComponent<Collider2D>()->radius;
}
static void SetColliderRadius(GameObject* GOptr, float radiusToSet)
{
	GOptr->GetComponent<Collider2D>()->radius = (double)radiusToSet;
}
static vec2f GetColliderBoxSize(GameObject* GOptr)
{
	return vec2f((float)GOptr->GetComponent<Collider2D>()->w, (float)GOptr->GetComponent<Collider2D>()->h);
}
static void SetColliderBoxSize(GameObject* GOptr, vec2f sizeToSet)
{
	GOptr->GetComponent<Collider2D>()->w = (double)sizeToSet.x;
	GOptr->GetComponent<Collider2D>()->h = (double)sizeToSet.y;
}

// Camera
static double GetFov(GameObject* GOptr)
{
	return (double)GOptr->GetComponent<Camera>()->fov;
}

static void SetFov(GameObject* GOptr, double* fov)
{
	GOptr->GetComponent<Camera>()->fov = (double)*fov;
}

static double GetAspect(GameObject* GOptr)
{
	return (double)GOptr->GetComponent<Camera>()->aspect;
}

static void SetAspect(GameObject* GOptr, double* aspect)
{
	GOptr->GetComponent<Camera>()->aspect = (double)*aspect;
}

static float GetYaw(GameObject* GOptr)
{
	return (float)GOptr->GetComponent<Camera>()->yaw;
}

static void SetYaw(GameObject* GOptr, float* yaw)
{
	GOptr->GetComponent<Camera>()->yaw = (float)*yaw;
}

static float GetPitch(GameObject* GOptr)
{
	return (float)GOptr->GetComponent<Camera>()->pitch;
}

static void SetPitch(GameObject* GOptr, float* pitch)
{
	GOptr->GetComponent<Camera>()->pitch = (float)*pitch;
}

static uint GetCameraType(GameObject* GOptr)
{
	return static_cast<uint>(GOptr->GetComponent<Camera>()->cameraType);
}

static void SetCameraType(GameObject* GOptr, uint* cameraType)
{
	GOptr->GetComponent<Camera>()->cameraType = static_cast<CameraType>((uint)*cameraType);
}

static bool GetPrimaryCam(GameObject* GOptr)
{
	return (bool)GOptr->GetComponent<Camera>()->primaryCam;
}

static void SetPrimaryCam(GameObject* GOptr, bool* primaryCam)
{
	GOptr->GetComponent<Camera>()->primaryCam = (bool)*primaryCam;
}

// Animator
static void PlayAnimation(GameObject* GOptr, MonoString* name) {
	std::string aName = MonoRegisterer::MonoStringToUTF8(name);
	Model* m = Resources::GetResourceById<Model>(GOptr->GetComponent<Mesh>()->meshID);
	if (m->HasAnimation(aName)) {
		m->PlayAnimation(aName);
	}
}

static void StopAnimation(GameObject* GOptr) {
	Model* m = Resources::GetResourceById<Model>(GOptr->GetComponent<Mesh>()->meshID);
	if (m->isAnimated()) {
		m->StopAnimation();
	}
}

static bool GetTransitionBlend(GameObject* GOptr){
	Model* m = Resources::GetResourceById<Model>(GOptr->GetComponent<Mesh>()->meshID);
	if (m->isAnimated()) {
		return m->getBlendOnTransition();
	}
	return false;
}

static void SetTransitionBlend(GameObject* GOptr, bool* blend) {
	Model* m = Resources::GetResourceById<Model>(GOptr->GetComponent<Mesh>()->meshID);
	if (m->isAnimated()) {
		m->setBlendOnTransition((bool)*blend);
	}
}

static float GetTransitionTime(GameObject* GOptr) {
	Model* m = Resources::GetResourceById<Model>(GOptr->GetComponent<Mesh>()->meshID);
	if (m->isAnimated()) {
		return m->getTransitionTime();
	}
	return -1.0f;
}

static void SetTransitionTime(GameObject* GOptr, float* time) {
	Model* m = Resources::GetResourceById<Model>(GOptr->GetComponent<Mesh>()->meshID);
	if (m->isAnimated()) {
		m->setTransitionTime((float)*time);
	}
}

static void UpdateAnimation(GameObject* GOptr, float* dt) {
	Model* m = Resources::GetResourceById<Model>(GOptr->GetComponent<Mesh>()->meshID);
	if (m->isAnimated()) {
		m->UpdateAnim((float)*dt);
	}
}

void MonoRegisterer::RegisterFunctions()
{
	//GameObject
	mono_add_internal_call("InternalCalls::GetGameObjectPtr", GetGameObjectPtr);
	mono_add_internal_call("InternalCalls::InstantiateBullet", InstantiateBullet);
	mono_add_internal_call("InternalCalls::InstantiateXenomorph", InstantiateXenomorph);
	mono_add_internal_call("InternalCalls::GetGameObjectName", GetGameObjectName);
	mono_add_internal_call("InternalCalls::DestroyGameObject", DestroyGameObject);
	mono_add_internal_call("InternalCalls::FindGameObject", FindGameObject);
	mono_add_internal_call("InternalCalls::FindGameObjectInChildren", FindGameObjectInChildren);
	mono_add_internal_call("InternalCalls::ComponentCheck", ComponentCheck);
	mono_add_internal_call("InternalCalls::GetScript", GetScript);
	mono_add_internal_call("InternalCalls::Disable", Disable);
	mono_add_internal_call("InternalCalls::Enable", Enable);

	//Input
	mono_add_internal_call("InternalCalls::GetKeyboardButton", GetKeyboardButton);
	mono_add_internal_call("InternalCalls::GetControllerButton", GetControllerButton);
	mono_add_internal_call("InternalCalls::GetControllerJoystick", GetControllerJoystick);

	//Transform
	mono_add_internal_call("InternalCalls::TransformCheck", TransformCheck);
	mono_add_internal_call("InternalCalls::GetPosition", GetPosition);
	mono_add_internal_call("InternalCalls::SetPosition", SetPosition);
	mono_add_internal_call("InternalCalls::GetRotation", GetRotation);
	mono_add_internal_call("InternalCalls::SetRotation", SetRotation);
	mono_add_internal_call("InternalCalls::GetScale", GetScale);
	mono_add_internal_call("InternalCalls::SetScale", SetScale);
	mono_add_internal_call("InternalCalls::Translate", Translate);
	mono_add_internal_call("InternalCalls::Rotate", Rotate);
	mono_add_internal_call("InternalCalls::GetTransformForward", GetTransformForward);
	mono_add_internal_call("InternalCalls::GetTransformRight", GetTransformRight);

	//Scene Manager
	mono_add_internal_call("InternalCalls::LoadScene", LoadScene);
	mono_add_internal_call("InternalCalls::GetCurrentSceneName", GetCurrentSceneName);
	mono_add_internal_call("InternalCalls::CreatePrefab", CreatePrefab);

	//User Interfaces
	mono_add_internal_call("InternalCalls::CanvasEnableToggle", CanvasEnableToggle);
	mono_add_internal_call("InternalCalls::ToggleChecker", ToggleChecker);
	mono_add_internal_call("InternalCalls::PrintItemUI", PrintItemUI);
	mono_add_internal_call("InternalCalls::GetSelectedButton", GetSelectedButton);
	mono_add_internal_call("InternalCalls::GetSelected", GetSelected);
	mono_add_internal_call("InternalCalls::MoveSelectedButton", MoveSelectedButton);
	mono_add_internal_call("InternalCalls::SetUiItemState", SetUiItemState);
	mono_add_internal_call("InternalCalls::MoveSelection", MoveSelection);
	mono_add_internal_call("InternalCalls::ChangeSectImg", ChangeSectImg);
	mono_add_internal_call("InternalCalls::GetSliderValue", GetSliderValue);
	mono_add_internal_call("InternalCalls::SetSliderValue", SetSliderValue);
	mono_add_internal_call("InternalCalls::GetSliderMaxValue", GetSliderMaxValue);
	mono_add_internal_call("InternalCalls::SetTextString", SetTextString);
	mono_add_internal_call("InternalCalls::GetTextString", GetTextString);

	//Helpers
	mono_add_internal_call("InternalCalls::GetAppDeltaTime", GetAppDeltaTime);
	mono_add_internal_call("InternalCalls::ExitApplication", ExitApplication);

	//Debug
	mono_add_internal_call("InternalCalls::ScriptingLog", ScriptingLog);
	mono_add_internal_call("InternalCalls::DrawWireCircle", DrawWireCircle);
	mono_add_internal_call("InternalCalls::DrawWireSphere", DrawWireSphere);
	mono_add_internal_call("InternalCalls::ToggleCollidersDraw", ToggleCollidersDraw);
	mono_add_internal_call("InternalCalls::ToggleGridDraw", ToggleGridDraw);

	//Particle Systems
	mono_add_internal_call("InternalCalls::PlayPS", PlayPS);
	mono_add_internal_call("InternalCalls::PausePS", PausePS);
	mono_add_internal_call("InternalCalls::ReplayPS", ReplayPS);
	mono_add_internal_call("InternalCalls::StopPS", StopPS);

	//Audio
	mono_add_internal_call("InternalCalls::PlaySource", PlayAudioSource);
	mono_add_internal_call("InternalCalls::StopSource", StopAudioSource);

	//Collider2D
	mono_add_internal_call("InternalCalls::GetColliderRadius", GetColliderRadius);
	mono_add_internal_call("InternalCalls::SetColliderRadius", SetColliderRadius);
	mono_add_internal_call("InternalCalls::GetColliderBoxSize", GetColliderBoxSize);
	mono_add_internal_call("InternalCalls::SetColliderBoxSize", SetColliderBoxSize);

	//Camera
	mono_add_internal_call("InternalCalls::GetFov", GetFov);
	mono_add_internal_call("InternalCalls::SetFov", SetFov);
	mono_add_internal_call("InternalCalls::GetAspect", GetAspect);
	mono_add_internal_call("InternalCalls::SetAspect", SetAspect);
	mono_add_internal_call("InternalCalls::GetYaw", GetYaw);
	mono_add_internal_call("InternalCalls::SetYaw", SetYaw);
	mono_add_internal_call("InternalCalls::GetPitch", GetPitch);
	mono_add_internal_call("InternalCalls::SetPitch", SetPitch);
	mono_add_internal_call("InternalCalls::GetCameraType", GetCameraType);
	mono_add_internal_call("InternalCalls::SetCameraType", SetCameraType);
	mono_add_internal_call("InternalCalls::GetPrimaryCam", GetPrimaryCam);
	mono_add_internal_call("InternalCalls::SetPrimaryCam", SetPrimaryCam);

	//Animation
	mono_add_internal_call("InternalCalls::PlayAnimation", PlayAnimation);
	mono_add_internal_call("InternalCalls::StopAnimation", StopAnimation);
	mono_add_internal_call("InternalCalls::SetTransitionBlend", GetTransitionBlend);
	mono_add_internal_call("InternalCalls::SetTransitionBlend", SetTransitionBlend);
	mono_add_internal_call("InternalCalls::SetTransitionTime", GetTransitionTime);
	mono_add_internal_call("InternalCalls::SetTransitionTime", SetTransitionTime);
	mono_add_internal_call("InternalCalls::UpdateAnimation", UpdateAnimation);
}

bool MonoRegisterer::CheckMonoError(MonoError& error)
{
	bool hasError = !mono_error_ok(&error);
	if (hasError)
	{
		unsigned short errorCode = mono_error_get_error_code(&error);
		const char* errorMessage = mono_error_get_message(&error);
		std::cout << "Mono Error!" << std::endl;
		std::cout << "Error Code: " << errorCode << std::endl;
		std::cout << "Error Message: " << errorMessage << std::endl;
		mono_error_cleanup(&error);
	}
	return hasError;
}

std::string MonoRegisterer::MonoStringToUTF8(MonoString* monoString)
{
	if (monoString == nullptr || mono_string_length(monoString) == 0)
		return "";

	MonoError error;
	char* utf8 = mono_string_to_utf8_checked(monoString, &error);
	if (CheckMonoError(error))
		return "";
	std::string result(utf8);
	mono_free(utf8);
	return result;
}
