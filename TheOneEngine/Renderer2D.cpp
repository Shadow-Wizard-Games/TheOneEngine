#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "UniformBuffer.h"
#include "Texture.h"
#include "Resources.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



struct QuadVertex
{
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec2 TexCoord;
	float TexIndex;
	float TilingFactor;
};

struct CircleVertex
{
	glm::vec3 WorldPosition;
	glm::vec3 LocalPosition;
	glm::vec4 Color;
	float Thickness;
	float Fade;
};

struct LineVertex
{
	glm::vec3 Position;
	glm::vec4 Color;
};

struct Renderer2DData
{
	static const uint32_t MaxQuads = 20000;
	static const uint32_t MaxVertices = MaxQuads * 4;
	static const uint32_t MaxIndices = MaxQuads * 6;
	static const uint32_t MaxTextureSlots = 32;

	std::shared_ptr<VertexArray> QuadVertexArray;
	std::shared_ptr<VertexBuffer> QuadVertexBuffer;
	std::shared_ptr<Shader> QuadShader;
	std::shared_ptr<Texture> WhiteTexture;

	std::shared_ptr<VertexArray> CircleVertexArray;
	std::shared_ptr<VertexBuffer> CircleVertexBuffer;
	std::shared_ptr<Shader> CircleShader;

	std::shared_ptr<VertexArray> LineVertexArray;
	std::shared_ptr<VertexBuffer> LineVertexBuffer;
	std::shared_ptr<Shader> LineShader;

	uint32_t QuadIndexCount = 0;
	QuadVertex* QuadVertexBufferBase = nullptr;
	QuadVertex* QuadVertexBufferPtr = nullptr;

	uint32_t CircleIndexCount = 0;
	CircleVertex* CircleVertexBufferBase = nullptr;
	CircleVertex* CircleVertexBufferPtr = nullptr;

	uint32_t LineVertexCount = 0;
	LineVertex* LineVertexBufferBase = nullptr;
	LineVertex* LineVertexBufferPtr = nullptr;

	float LineWidth = 2.0f;

	std::array<Texture*, MaxTextureSlots> TextureSlots;
	uint32_t TextureSlotIndex = 1; // 0 = white texture

	glm::vec4 QuadVertexPositions[4];

	Renderer2D::Statistics Stats;
};

static Renderer2DData renderer2D;



static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount)
{
	vertexArray->Bind();
	uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
	GLCALL(glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr));
}

static void DrawLines(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount)
{
	vertexArray->Bind();
	GLCALL(glDrawArrays(GL_LINES, 0, vertexCount));
}

static void SetLineWidth(float width)
{
	GLCALL(glLineWidth(width));
}

void Renderer2D::Init()
{
	renderer2D.QuadVertexArray = std::make_shared<VertexArray>();

	renderer2D.QuadVertexBuffer = std::make_shared<VertexBuffer>(renderer2D.MaxVertices * sizeof(QuadVertex));
	renderer2D.QuadVertexBuffer->SetLayout({
		{ ShaderDataType::Float3, "a_Position"     },
		{ ShaderDataType::Float4, "a_Color"        },
		{ ShaderDataType::Float2, "a_TexCoord"     },
		{ ShaderDataType::Float,  "a_TexIndex"     },
		{ ShaderDataType::Float,  "a_TilingFactor" }
		});
	renderer2D.QuadVertexArray->AddVertexBuffer(renderer2D.QuadVertexBuffer);

	renderer2D.QuadVertexBufferBase = new QuadVertex[renderer2D.MaxVertices];

	uint32_t* quadIndices = new uint32_t[renderer2D.MaxIndices];

	uint32_t offset = 0;
	for (uint32_t i = 0; i < renderer2D.MaxIndices; i += 6)
	{
		quadIndices[i + 0] = offset + 0;
		quadIndices[i + 1] = offset + 1;
		quadIndices[i + 2] = offset + 2;

		quadIndices[i + 3] = offset + 2;
		quadIndices[i + 4] = offset + 3;
		quadIndices[i + 5] = offset + 0;

		offset += 4;
	}

	std::shared_ptr<IndexBuffer> quadIB = std::make_shared<IndexBuffer>(quadIndices, renderer2D.MaxIndices);
	renderer2D.QuadVertexArray->SetIndexBuffer(quadIB);
	delete[] quadIndices;

	// Circles
	renderer2D.CircleVertexArray = std::make_shared<VertexArray>();

	renderer2D.CircleVertexBuffer = std::make_shared<VertexBuffer>(renderer2D.MaxVertices * sizeof(CircleVertex));
	renderer2D.CircleVertexBuffer->SetLayout({
		{ ShaderDataType::Float3, "a_WorldPosition" },
		{ ShaderDataType::Float3, "a_LocalPosition" },
		{ ShaderDataType::Float4, "a_Color"         },
		{ ShaderDataType::Float,  "a_Thickness"     },
		{ ShaderDataType::Float,  "a_Fade"          }
		});
	renderer2D.CircleVertexArray->AddVertexBuffer(renderer2D.CircleVertexBuffer);
	renderer2D.CircleVertexArray->SetIndexBuffer(quadIB); // Use quad IB
	renderer2D.CircleVertexBufferBase = new CircleVertex[renderer2D.MaxVertices];

	// Lines
	renderer2D.LineVertexArray = std::make_shared<VertexArray>();

	renderer2D.LineVertexBuffer = std::make_shared<VertexBuffer>(renderer2D.MaxVertices * sizeof(LineVertex));
	renderer2D.LineVertexBuffer->SetLayout({
		{ ShaderDataType::Float3, "a_Position" },
		{ ShaderDataType::Float4, "a_Color"    }
		});
	renderer2D.LineVertexArray->AddVertexBuffer(renderer2D.LineVertexBuffer);
	renderer2D.LineVertexBufferBase = new LineVertex[renderer2D.MaxVertices];

	renderer2D.WhiteTexture = std::make_shared<Texture>();
	uint32_t whiteTextureData = 0xffffffff;
	renderer2D.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

	int32_t samplers[renderer2D.MaxTextureSlots];
	for (uint32_t i = 0; i < renderer2D.MaxTextureSlots; i++)
		samplers[i] = i;

	renderer2D.QuadShader = std::make_shared<Shader>("Assets/Shaders/Renderer2DQuad");
	renderer2D.CircleShader = std::make_shared<Shader>("Assets/Shaders/Renderer2DCircle");
	renderer2D.LineShader = std::make_shared<Shader>("Assets/Shaders/Renderer2DLine");

	// Set first texture slot to 0
	renderer2D.TextureSlots[0] = renderer2D.WhiteTexture.get();

	renderer2D.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
	renderer2D.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
	renderer2D.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
	renderer2D.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
}

void Renderer2D::Shutdown()
{
	renderer2D.WhiteTexture->Delete();
	renderer2D.QuadVertexArray->Delete();
	renderer2D.CircleVertexArray->Delete();
	renderer2D.LineVertexArray->Delete();
	delete[] renderer2D.QuadVertexBufferBase;
}

void Renderer2D::StartBatch()
{
	renderer2D.QuadIndexCount = 0;
	renderer2D.QuadVertexBufferPtr = renderer2D.QuadVertexBufferBase;

	renderer2D.CircleIndexCount = 0;
	renderer2D.CircleVertexBufferPtr = renderer2D.CircleVertexBufferBase;

	renderer2D.LineVertexCount = 0;
	renderer2D.LineVertexBufferPtr = renderer2D.LineVertexBufferBase;

	renderer2D.TextureSlotIndex = 1;
}

void Renderer2D::Flush()
{
	if (renderer2D.QuadIndexCount)
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)renderer2D.QuadVertexBufferPtr - (uint8_t*)renderer2D.QuadVertexBufferBase);
		renderer2D.QuadVertexBuffer->SetData(renderer2D.QuadVertexBufferBase, dataSize);

		// Bind textures
		for (uint32_t i = 0; i < renderer2D.TextureSlotIndex; i++)
			renderer2D.TextureSlots[i]->Bind(i);

		renderer2D.QuadShader->Bind();
		DrawIndexed(renderer2D.QuadVertexArray, renderer2D.QuadIndexCount);
		renderer2D.Stats.DrawCalls++;
	}

	if (renderer2D.CircleIndexCount)
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)renderer2D.CircleVertexBufferPtr - (uint8_t*)renderer2D.CircleVertexBufferBase);
		renderer2D.CircleVertexBuffer->SetData(renderer2D.CircleVertexBufferBase, dataSize);

		renderer2D.CircleShader->Bind();
		DrawIndexed(renderer2D.CircleVertexArray, renderer2D.CircleIndexCount);
		renderer2D.Stats.DrawCalls++;
	}

	if (renderer2D.LineVertexCount)
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)renderer2D.LineVertexBufferPtr - (uint8_t*)renderer2D.LineVertexBufferBase);
		renderer2D.LineVertexBuffer->SetData(renderer2D.LineVertexBufferBase, dataSize);

		renderer2D.LineShader->Bind();
		SetLineWidth(renderer2D.LineWidth);
		DrawLines(renderer2D.LineVertexArray, renderer2D.LineVertexCount);
		renderer2D.Stats.DrawCalls++;
	}
}

void Renderer2D::NextBatch()
{
	Flush();
	StartBatch();
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
{
	DrawQuad({ position.x, position.y, 0.0f }, size, color);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	DrawQuad(transform, color);
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, ResourceId spriteID, float tilingFactor, const glm::vec4& tintColor)
{
	DrawQuad({ position.x, position.y, 0.0f }, size, spriteID, tilingFactor, tintColor);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, ResourceId spriteID, float tilingFactor, const glm::vec4& tintColor)
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	DrawQuad(transform, spriteID, tilingFactor, tintColor);
}

void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
{
	constexpr size_t quadVertexCount = 4;
	const float textureIndex = 0.0f; // White Texture
	constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
	const float tilingFactor = 1.0f;

	if (renderer2D.QuadIndexCount >= Renderer2DData::MaxIndices)
		NextBatch();

	for (size_t i = 0; i < quadVertexCount; i++)
	{
		renderer2D.QuadVertexBufferPtr->Position = transform * renderer2D.QuadVertexPositions[i];
		renderer2D.QuadVertexBufferPtr->Color = color;
		renderer2D.QuadVertexBufferPtr->TexCoord = textureCoords[i];
		renderer2D.QuadVertexBufferPtr->TexIndex = textureIndex;
		renderer2D.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		renderer2D.QuadVertexBufferPtr++;
	}

	renderer2D.QuadIndexCount += 6;

	renderer2D.Stats.QuadCount++;
}

void Renderer2D::DrawQuad(const glm::mat4& transform, ResourceId spriteID, float tilingFactor, const glm::vec4& tintColor)
{
	constexpr size_t quadVertexCount = 4;
	constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

	Texture* sprite = Resources::GetResourceById<Texture>(spriteID);

	if (renderer2D.QuadIndexCount >= Renderer2DData::MaxIndices)
		NextBatch();

	float textureIndex = 0.0f;
	for (uint32_t i = 1; i < renderer2D.TextureSlotIndex; i++)
	{
		if (*renderer2D.TextureSlots[i] == *sprite)
		{
			textureIndex = (float)i;
			break;
		}
	}

	if (textureIndex == 0.0f)
	{
		if (renderer2D.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
			NextBatch();

		textureIndex = (float)renderer2D.TextureSlotIndex;
		renderer2D.TextureSlots[renderer2D.TextureSlotIndex] = sprite;
		renderer2D.TextureSlotIndex++;
	}

	for (size_t i = 0; i < quadVertexCount; i++)
	{
		renderer2D.QuadVertexBufferPtr->Position = transform * renderer2D.QuadVertexPositions[i];
		renderer2D.QuadVertexBufferPtr->Color = tintColor;
		renderer2D.QuadVertexBufferPtr->TexCoord = textureCoords[i];
		renderer2D.QuadVertexBufferPtr->TexIndex = textureIndex;
		renderer2D.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		renderer2D.QuadVertexBufferPtr++;
	}

	renderer2D.QuadIndexCount += 6;

	renderer2D.Stats.QuadCount++;
}

void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
{
	DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
}

void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
		* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	DrawQuad(transform, color);
}

void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, ResourceId spriteID, float tilingFactor, const glm::vec4& tintColor)
{
	DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, spriteID, tilingFactor, tintColor);
}

void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, ResourceId spriteID, float tilingFactor, const glm::vec4& tintColor)
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
		* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	DrawQuad(transform, spriteID, tilingFactor, tintColor);
}

void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness /*= 1.0f*/, float fade /*= 0.005f*/)
{
	for (size_t i = 0; i < 4; i++)
	{
		renderer2D.CircleVertexBufferPtr->WorldPosition = transform * renderer2D.QuadVertexPositions[i];
		renderer2D.CircleVertexBufferPtr->LocalPosition = renderer2D.QuadVertexPositions[i] * 2.0f;
		renderer2D.CircleVertexBufferPtr->Color = color;
		renderer2D.CircleVertexBufferPtr->Thickness = thickness;
		renderer2D.CircleVertexBufferPtr->Fade = fade;
		renderer2D.CircleVertexBufferPtr++;
	}

	renderer2D.CircleIndexCount += 6;

	renderer2D.Stats.QuadCount++;
}

void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color)
{
	renderer2D.LineVertexBufferPtr->Position = p0;
	renderer2D.LineVertexBufferPtr->Color = color;
	renderer2D.LineVertexBufferPtr++;

	renderer2D.LineVertexBufferPtr->Position = p1;
	renderer2D.LineVertexBufferPtr->Color = color;
	renderer2D.LineVertexBufferPtr++;

	renderer2D.LineVertexCount += 2;
}

void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
{
	glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
	glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
	glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
	glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

	DrawLine(p0, p1, color);
	DrawLine(p1, p2, color);
	DrawLine(p2, p3, color);
	DrawLine(p3, p0, color);
}

void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color)
{
	glm::vec3 lineVertices[4];
	for (size_t i = 0; i < 4; i++)
		lineVertices[i] = transform * renderer2D.QuadVertexPositions[i];

	DrawLine(lineVertices[0], lineVertices[1], color);
	DrawLine(lineVertices[1], lineVertices[2], color);
	DrawLine(lineVertices[2], lineVertices[3], color);
	DrawLine(lineVertices[3], lineVertices[0], color);
}

float Renderer2D::GetLineWidth()
{
	return renderer2D.LineWidth;
}

void Renderer2D::SetLineWidth(float width)
{
	renderer2D.LineWidth = width;
}

void Renderer2D::ResetStats()
{
	memset(&renderer2D.Stats, 0, sizeof(Statistics));
}

Renderer2D::Statistics Renderer2D::GetStats()
{
	return renderer2D.Stats;
}

