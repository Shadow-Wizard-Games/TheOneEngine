#ifndef __PANEL_SCENE_H__
#define __PANEL_SCENE_H__
#pragma once

#include "Panel.h"

#include "../TheOneEngine/Transform.h"
#include "../TheOneEngine/Framebuffer.h"

#include<vector>

class Ray;
class Camera;
class Scene;

class PanelScene : public Panel
{
public:
	PanelScene(PanelType type, std::string name);
	~PanelScene();
	void Start();

	bool Draw();

	Ray GetScreenRay(int x, int y, Camera* camera, int width, int height);

	HandleSpace GetHandleSpace() const { return handleSpace; }
	HandlePosition GetHandlePosition() const { return handlePosition; }

public:
	bool isHovered;
	bool isFocused;
	std::vector<Ray> rays;

	bool drawMesh;
	bool drawWireframe;
	bool drawNormalsVerts;
	bool drawNormalsFaces;
	bool drawAABB;
	bool drawOBB;
	bool drawRaycasting;
	bool drawChecker;

private:
	HandleSpace handleSpace;
	HandlePosition handlePosition;
	int gizmoType;
	int gizmoMode;

	const char* spaces[2] =
	{
		"Local",
		"Global",
	};

	const char* positions[2] =
	{
		"Pivot",
		"Center",
	};

	Scene* current;
	std::shared_ptr<GameObject> sceneCamera;
	std::shared_ptr<GameObject> cameraParent;
	std::shared_ptr<FrameBuffer> frameBuffer;

	// snapping
	bool snappingFixed = false;
	float snapAmount = 10.0f;

	glm::vec2 viewportSize = { 0.0f, 0.0f };
};

#endif // !__PANEL_SCENE_H__