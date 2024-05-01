#ifndef __PANEL_SCENE_H__
#define __PANEL_SCENE_H__
#pragma once

#include "Panel.h"

#include "TheOneEngine/Transform.h"
#include "TheOneEngine/Framebuffer.h"

#include<vector>

class Ray;
class Camera;
class Scene;
class Easing;

class PanelScene : public Panel
{
public:
	PanelScene(PanelType type, std::string name);
	~PanelScene();
	void Start();

	bool Draw();

	Ray GetScreenRay(int x, int y, Camera* camera, int width, int height);
	void CameraInput(GameObject* cam);
	void MoveCamera();

	HandleSpace GetHandleSpace() const { return handleSpace; }
	HandlePosition GetHandlePosition() const { return handlePosition; }

public:
	bool cameraControl;
	bool isHovered;
	bool isFocused;
	std::vector<Ray> rays;

private:
	HandleSpace handleSpace;
	HandlePosition handlePosition;
	int gizmoType;
	int gizmoMode;

	const char* spaces[2] = { "Local","Global" };
	const char* positions[2] = { "Pivot","Center" };

	Scene* current;
	std::shared_ptr<GameObject> sceneCamera;
	std::shared_ptr<GameObject> cameraParent;
	std::unique_ptr<Easing> cameraIn;
	std::unique_ptr<Easing> cameraOut;
	std::shared_ptr<FrameBuffer> frameBuffer;

	// snapping
	bool snapping;
	int snapAmount;

	bool easing;
	float camTargetSpeed;
	double camSpeed;
	double camSpeedOut;
	char camKeyOut;

	bool drawMesh;
	bool drawWireframe;
	bool drawNormalsVerts;
	bool drawNormalsFaces;
	bool drawAABB;
	bool drawOBB;
	bool drawRaycasting;
	bool drawChecker;

	glm::vec2 viewportSize = { 0.0f, 0.0f };

};

#endif // !__PANEL_SCENE_H__