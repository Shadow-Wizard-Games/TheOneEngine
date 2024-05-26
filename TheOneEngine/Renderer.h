#pragma once

#include "Defs.h"
#include "GameObject.h"
#include "Camera.h"
#include "FrameBuffer.h"

#include <vector>
#include <string>

class RenderTarget;
class Ray;

class Renderer
{
public:
	static void Init();
	static void Update();
	static void Shutdown();

	static unsigned int AddRenderTarget(DrawMode mode, Camera* camera, glm::vec2 viewportSize, std::vector<std::vector<Attachment>> frameBuffers);
	static std::vector<FrameBuffer>* GetFrameBuffers(unsigned int targetID);
	static FrameBuffer* GetFrameBuffer(unsigned int targetID, std::string name);

	static void SetRenderEnvironment();
	static void SetUniformBufferCamera(const glm::mat4& cam);

	static void AddRay(Ray ray);
	static void ClearRays();

	// Draw State
	static bool GetDrawGrid();
	static void SetDrawGrid(bool draw);

	static bool GetDrawAxis();
	static void SetDrawAxis(bool draw);

	static bool GetDrawCollisions();
	static void SetDrawCollisions(bool draw);

	static bool GetDrawScriptShapes();
	static void SetDrawScriptShapes(bool draw);

	static bool GetDrawRaycasting();
	static void SetDrawRaycasting(bool draw);

private:

	// Draw Utils
	static void DebugDraw(bool override = false);
	static void DrawAxis();
	static void DrawGrid(int grid_size, int grid_step);
	static void DrawFrustum(const Frustum& frustum);
	static void DrawRay(const Ray& ray);
};