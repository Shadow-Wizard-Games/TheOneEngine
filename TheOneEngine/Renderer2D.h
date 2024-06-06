#pragma once

#include "Defs.h"
#include "Resources.h"
#include "Font.h"

class Batch;
class RenderTarget;

enum BatchType
{
	WORLD,
	UI,
	EDITOR,
	MAX
};

class Renderer2D
{
public:
	static void Init();
	static void Update(BatchType type, RenderTarget target);
	static void UpdateIndexed(BatchType type, RenderTarget target);
	static void Shutdown();
	static void ResetBatches();

	// Primitives
	static void DrawQuad(BatchType type, const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
	static void DrawQuad(BatchType type, const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
	static void DrawQuad(BatchType type, const glm::vec2& position, const glm::vec2& size, ResourceId imageID, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);
	static void DrawQuad(BatchType type, const glm::vec3& position, const glm::vec2& size, ResourceId imageID, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);

	// Quad With Texture Coordinates
	struct TexCoordsSection
	{
		glm::vec2 leftBottom;
		glm::vec2 rightBottom;
		glm::vec2 rightTop;
		glm::vec2 leftTop;
	};
	static void DrawQuad(BatchType type, const glm::vec2& position, const glm::vec2& size, ResourceId imageID, const TexCoordsSection& texCoords, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);
	static void DrawQuad(BatchType type, const glm::vec3& position, const glm::vec2& size, ResourceId imageID, const TexCoordsSection& texCoords, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);

	static void DrawQuad(BatchType type, const glm::mat4& transform, const glm::vec4& color);
	static void DrawQuad(BatchType type, const glm::mat4& transform, ResourceId imageID, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);
	static void DrawQuad(BatchType type, const glm::mat4& transform, ResourceId imageID, const TexCoordsSection& texCoords, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);

	static void DrawRotatedQuad(BatchType type, const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
	static void DrawRotatedQuad(BatchType type, const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
	static void DrawRotatedQuad(BatchType type, const glm::vec2& position, const glm::vec2& size, float rotation, ResourceId imageID, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);
	static void DrawRotatedQuad(BatchType type, const glm::vec3& position, const glm::vec2& size, float rotation, ResourceId imageID, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);

	static void DrawCircle(BatchType type, const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f);

	static void DrawLine(BatchType type, const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color);

	static void DrawRect(BatchType type, const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
	static void DrawRect(BatchType type, const glm::mat4& transform, const glm::vec4& color);

	struct TextParams
	{
		glm::vec4 Color{ 1.0f };
		float Kerning = 0.0f;
		float LineSpacing = 0.0f;
	};
	static void DrawString(BatchType type, const std::string& string, Font* font, const glm::vec2& position, const glm::vec2& size, const TextParams& textParams);
	static void DrawString(BatchType type, const std::string& string, Font* font, const glm::mat4& transform, const TextParams& textParams);

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

	static void InitParticleShader();
	static ResourceId GetParticleMaterialID();

private:
	static void NextQuadBatch(Batch& batch);

	static void DrawQuadBatch(const Batch& batch, BatchType type, RenderTarget target);
	static void DrawQuadIndexedBatch(const Batch& batch);
	static void DrawCircleBatch(const Batch& batch);
	static void DrawLineBatch(const Batch& batch);
	static void DrawTextBatch(const Batch& batch);

	static void ResetQuadBatch(Batch& batch);
	static void ResetCircleBatch(Batch& batch);
	static void ResetLineBatch(Batch& batch);
	static void ResetTextBatch(Batch& batch);

};
