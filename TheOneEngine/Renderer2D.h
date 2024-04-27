#pragma once

#include "Defs.h"
#include "Resources.h"

class Renderer2D
{
public:
	static void Init();
	static void Shutdown();

	static void StartBatch();
	static void Flush();

	// Primitives
	static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
	static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
	static void DrawQuad(const glm::vec2& position, const glm::vec2& size, ResourceId spriteID, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
	static void DrawQuad(const glm::vec3& position, const glm::vec2& size, ResourceId spriteID, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

	static void DrawQuad(const glm::mat4& transform, const glm::vec4& color);
	static void DrawQuad(const glm::mat4& transform, ResourceId spriteID, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

	static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
	static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
	static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, ResourceId spriteID, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
	static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, ResourceId spriteID, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

	static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f);

	static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color);

	static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
	static void DrawRect(const glm::mat4& transform, const glm::vec4& color);

	//TODO: Draw text

	static float GetLineWidth();
	static void SetLineWidth(float width);

	// Stats
	struct Statistics
	{
		uint32_t DrawCalls = 0;
		uint32_t QuadCount = 0;

		uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
		uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
	};
	static void ResetStats();
	static Statistics GetStats();

private:
	static void NextBatch();
};
