#ifndef __PANEL_SCENE_H__
#define __PANEL_SCENE_H__
#pragma once

#include "Panel.h"

#include "TheOneEngine/Defs.h"
#include "TheOneEngine/Transform.h"
#include "TheOneEngine/Framebuffer.h"

#include <vector>
#include <stack>

class Ray;
class Camera;
class Scene;
class Easing;

enum class CamControlMode
{
	ENABLED,
	PANNING,
	ORBIT,
	DISABLED
};

class PanelScene : public Panel
{
public:
	PanelScene(PanelType type, std::string name);
	~PanelScene();
	void Start();

	bool Draw();

	Ray GetScreenRay(int x, int y, Camera* camera, int width, int height);
	void CameraMode();
	void CameraMovement(GameObject* cam);
	void HandleInput(SDL_Scancode key);
	void SetTargetSpeed();
	void InfiniteScroll(bool global = false);

	HandleSpace GetHandleSpace() const { return handleSpace; }
	HandlePosition GetHandlePosition() const { return handlePosition; }

public:
	bool isHovered;
	bool isFocused;
	std::vector<Ray> rays;

private:
	Scene* current;

	std::shared_ptr<GameObject> sceneCamera;
	std::shared_ptr<GameObject> cameraParent;
	CamControlMode camControlMode;
	std::stack<SDL_Scancode> inputStack;
	uint camEaseInX;
	uint camEaseInY;
	uint camEaseOutX;
	uint camEaseOutY;

	vec2 camInitSpeed = { 0, 0 };
	vec2 camTargetSpeed = { 0, 0 };
	vec2 camCurrentSpeed = { 0, 0 };

	std::shared_ptr<FrameBuffer> frameBuffer;
	glm::vec2 viewportSize;

	int gizmoType;
	int gizmoMode;

	// Menu Bar Settings ---
	HandleSpace handleSpace;
	HandlePosition handlePosition;
	const char* spaces[2] = { "Local", "Global" };
	const char* positions[2] = { "Pivot", "Center" };

	bool easing;
	float camSpeedMult;

	bool drawMesh;
	bool drawWireframe;
	bool drawNormalsVerts;
	bool drawNormalsFaces;
	bool drawAABB;
	bool drawOBB;
	bool drawRaycasting;
	bool drawChecker;

	bool snapping;
	int snapAmount;
};

#endif // !__PANEL_SCENE_H__