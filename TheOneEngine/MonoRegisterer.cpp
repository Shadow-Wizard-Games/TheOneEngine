#include "MonoRegisterer.h"
#include "MonoManager.h"
#include "Defs.h"

#include "EngineCore.h"
#include "Transform.h"
#include "Canvas.h"
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
	return go;
}

static void DestroyGameObject(GameObject* objectToDestroy)
{
	objectToDestroy->AddToDelete(engine->N_sceneManager->objectsToDelete);
}

static GameObject* FindGameObject(MonoString* monoString)
{
	std::string name = MonoRegisterer::MonoStringToUTF8(monoString);

	for (auto go : engine->N_sceneManager->currentScene->GetRootSceneGO()->children)
	{
		if (go->GetName() == name)
		{
			return go.get();
		}
	}
	return nullptr;
}

//Scene Management
static void LoadScene(MonoString* sceneName)
{
	std::string name = MonoRegisterer::MonoStringToUTF8(sceneName);

	engine->N_sceneManager->LoadScene(name);
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

static int GetSelectiedButton(GameObject* containerGO)
{
	std::vector<ItemUI*> uiElements = containerGO->GetComponent<Canvas>()->GetUiElements();
	int ret = -1;
	for (size_t i = 0; i < uiElements.size(); i++)
	{
		if (uiElements[i]->GetType() == UiType::BUTTONIMAGE)
		{
			ret++;
			if (uiElements[i]->GetState() == UiState::HOVERED)
				return ret;
		}
	}
	return ret;
}

static void MoveSelectedButton(GameObject* containerGO, int direction)
{
	std::vector<ItemUI*> uiElements = containerGO->GetComponent<Canvas>()->GetUiElements();

	for (size_t i = 0; i < uiElements.size(); i++)
	{
		if (uiElements[i]->GetType() == UiType::BUTTONIMAGE && uiElements[i]->GetState() == UiState::HOVERED)
		{
			for (int j = i + direction; j != i; j += direction)
			{
				if (j < 0)
					j = uiElements.size() - 1;
				else if (j >= uiElements.size())
					j = 0;

				if (uiElements[j]->GetType() == UiType::BUTTONIMAGE)
				{
					uiElements[i]->SetState(UiState::IDLE);
					uiElements[j]->SetState(UiState::HOVERED);
					break;
				}
			}
			break;
		}
	}
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

// Particle System
static void PlayPS(GameObject* GOptr)
{
	GOptr->GetComponent<ParticleSystem>()->Play();
}

static void StopPS(GameObject* GOptr)
{
	GOptr->GetComponent<ParticleSystem>()->Stop();
}

static void ReplayPS(GameObject* GOptr)
{
	GOptr->GetComponent<ParticleSystem>()->Replay();
}

static void StopSource(GameObject* GOptr, uint audio) {
	AkUInt32 myAkUInt32 = static_cast<AkUInt32>(audio);

	audioManager->StopAudio(GOptr->GetComponent<Source>(), audio);
}

// Audio
static void PlaySource(GameObject* GOptr, uint audio) {
	AkUInt32 myAkUInt32 = static_cast<AkUInt32>(audio);

	audioManager->PlayAudio(GOptr->GetComponent<Source>(), audio);
}

void MonoRegisterer::RegisterFunctions()
{
	mono_add_internal_call("InternalCalls::GetGameObjectPtr", GetGameObjectPtr);

	mono_add_internal_call("InternalCalls::GetKeyboardButton", GetKeyboardButton);
	mono_add_internal_call("InternalCalls::GetControllerButton", GetControllerButton);
	mono_add_internal_call("InternalCalls::GetControllerJoystick", GetControllerJoystick);

	mono_add_internal_call("InternalCalls::TransformCheck", TransformCheck);
	mono_add_internal_call("InternalCalls::GetPosition", GetPosition);
	mono_add_internal_call("InternalCalls::SetPosition", SetPosition);
	mono_add_internal_call("InternalCalls::GetRotation", GetRotation);
	mono_add_internal_call("InternalCalls::SetRotation", SetRotation);
	mono_add_internal_call("InternalCalls::Translate", Translate);
	mono_add_internal_call("InternalCalls::Rotate", Rotate);
	mono_add_internal_call("InternalCalls::GetTransformForward", GetTransformForward);
	mono_add_internal_call("InternalCalls::GetTransformRight", GetTransformRight);

	mono_add_internal_call("InternalCalls::InstantiateBullet", InstantiateBullet);
	mono_add_internal_call("InternalCalls::DestroyGameObject", DestroyGameObject);
	mono_add_internal_call("InternalCalls::FindGameObject", FindGameObject);

	mono_add_internal_call("InternalCalls::LoadScene", LoadScene);

	mono_add_internal_call("InternalCalls::CanvasEnableToggle", CanvasEnableToggle);
	mono_add_internal_call("InternalCalls::GetSelectiedButton", GetSelectiedButton);
	mono_add_internal_call("InternalCalls::MoveSelectedButton", MoveSelectedButton);

	mono_add_internal_call("InternalCalls::GetAppDeltaTime", GetAppDeltaTime);
	mono_add_internal_call("InternalCalls::ExitApplication", ExitApplication);

	mono_add_internal_call("InternalCalls::ScriptingLog", ScriptingLog);
	mono_add_internal_call("InternalCalls::DrawWireCircle", DrawWireCircle);
	mono_add_internal_call("InternalCalls::DrawWireSphere", DrawWireSphere);

	mono_add_internal_call("InternalCalls::PlayPS", PlayPS);
	mono_add_internal_call("InternalCalls::StopPS", StopPS);
	mono_add_internal_call("InternalCalls::ReplayPS", ReplayPS);

	mono_add_internal_call("InternalCalls::PlaySource", PlaySource);
	mono_add_internal_call("InternalCalls::StopSource", StopSource);
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
