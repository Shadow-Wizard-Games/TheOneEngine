#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "UniformBuffer.h"
#include "Texture.h"
#include "Resources.h"
#include "Renderer.h"
#include "RenderTarget.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "MSDFData.h"

static const uint32_t MaxQuads = 20000;
static const uint32_t MaxVertices = MaxQuads * 4;
static const uint32_t MaxIndices = MaxQuads * 6;
static const uint32_t MaxTextureSlots = 32;

static std::shared_ptr<Texture> WhiteTexture;

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

struct TextVertex
{
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec2 TexCoord;
};

struct Batch
{
	void Init()
	{
		QuadVertexArray = std::make_shared<HeapVertexArray>();

		QuadVertexBuffer = std::make_shared<VertexBuffer>(MaxVertices * sizeof(QuadVertex));
		QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position"     },
			{ ShaderDataType::Float4, "a_Color"        },
			{ ShaderDataType::Float2, "a_TexCoord"     },
			{ ShaderDataType::Float,  "a_TexIndex"     },
			{ ShaderDataType::Float,  "a_TilingFactor" }
			});
		QuadVertexArray->AddVertexBuffer(QuadVertexBuffer);

		QuadVertexBufferBase = new QuadVertex[MaxVertices];

		uint32_t* quadIndices = new uint32_t[MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		std::shared_ptr<IndexBuffer> quadIB = std::make_shared<IndexBuffer>(quadIndices, MaxIndices);
		QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		// Circles
		CircleVertexArray = std::make_shared<HeapVertexArray>();

		CircleVertexBuffer = std::make_shared<VertexBuffer>(MaxVertices * sizeof(CircleVertex));
		CircleVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_WorldPosition" },
			{ ShaderDataType::Float3, "a_LocalPosition" },
			{ ShaderDataType::Float4, "a_Color"         },
			{ ShaderDataType::Float,  "a_Thickness"     },
			{ ShaderDataType::Float,  "a_Fade"          }
			});
		CircleVertexArray->AddVertexBuffer(CircleVertexBuffer);
		CircleVertexArray->SetIndexBuffer(quadIB); // Use quad IB
		CircleVertexBufferBase = new CircleVertex[MaxVertices];

		// Lines
		LineVertexArray = std::make_shared<HeapVertexArray>();

		LineVertexBuffer = std::make_shared<VertexBuffer>(MaxVertices * sizeof(LineVertex));
		LineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"    }
			});
		LineVertexArray->AddVertexBuffer(LineVertexBuffer);
		LineVertexBufferBase = new LineVertex[MaxVertices];

		// Text
		TextVertexArray = std::make_shared<HeapVertexArray>();

		TextVertexBuffer = std::make_shared<VertexBuffer>(MaxVertices * sizeof(TextVertex));
		TextVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position"     },
			{ ShaderDataType::Float4, "a_Color"        },
			{ ShaderDataType::Float2, "a_TexCoord"     }
			});
		TextVertexArray->AddVertexBuffer(TextVertexBuffer);
		TextVertexBufferBase = new TextVertex[MaxVertices];

		uint32_t* textIndices = new uint32_t[MaxIndices];

		offset = 0;
		for (uint32_t i = 0; i < MaxIndices; i += 6)
		{
			textIndices[i + 0] = offset + 0;
			textIndices[i + 1] = offset + 2;
			textIndices[i + 2] = offset + 1;

			textIndices[i + 3] = offset + 0;
			textIndices[i + 4] = offset + 3;
			textIndices[i + 5] = offset + 2;

			offset += 4;
		}

		std::shared_ptr<IndexBuffer> textIB = std::make_shared<IndexBuffer>(textIndices, MaxIndices);
		TextVertexArray->SetIndexBuffer(quadIB);
		delete[] textIndices;

		int32_t samplers[MaxTextureSlots];
		for (uint32_t i = 0; i < MaxTextureSlots; i++)
			samplers[i] = i;


		// Set first texture slot to 0
		TextureSlots[0] = WhiteTexture.get();
	}

	void Delete() {
		QuadVertexArray->Delete();
		CircleVertexArray->Delete();
		LineVertexArray->Delete();
		TextVertexArray->Delete();
		delete[] QuadVertexBufferBase;
	}
	std::shared_ptr<HeapVertexArray> QuadVertexArray;
	std::shared_ptr<VertexBuffer> QuadVertexBuffer;

	std::shared_ptr<HeapVertexArray> CircleVertexArray;
	std::shared_ptr<VertexBuffer> CircleVertexBuffer;

	std::shared_ptr<HeapVertexArray> LineVertexArray;
	std::shared_ptr<VertexBuffer> LineVertexBuffer;

	std::shared_ptr<HeapVertexArray> TextVertexArray;
	std::shared_ptr<VertexBuffer> TextVertexBuffer;

	uint32_t QuadIndexCount = 0;
	QuadVertex* QuadVertexBufferBase = nullptr;
	QuadVertex* QuadVertexBufferPtr = nullptr;

	uint32_t CircleIndexCount = 0;
	CircleVertex* CircleVertexBufferBase = nullptr;
	CircleVertex* CircleVertexBufferPtr = nullptr;

	uint32_t LineVertexCount = 0;
	LineVertex* LineVertexBufferBase = nullptr;
	LineVertex* LineVertexBufferPtr = nullptr;

	uint32_t TextIndexCount = 0;
	TextVertex* TextVertexBufferBase = nullptr;
	TextVertex* TextVertexBufferPtr = nullptr;


	std::array<Texture*, MaxTextureSlots> TextureSlots;
	uint32_t TextureSlotIndex = 1; // 0 = white texture
};

struct Renderer2DData
{
	std::shared_ptr<Shader> QuadShader;
	std::shared_ptr<Shader> ParticleShader;
	std::shared_ptr<Shader> QuadIndexShader;
	std::shared_ptr<Shader> CircleShader;
	std::shared_ptr<Shader> LineShader;
	std::shared_ptr<Shader> TextShader;

	std::array<Batch, BatchType::MAX> batches;

	std::shared_ptr<Texture> FontAtlasTexture;

	glm::vec4 QuadVertexPositions[4];

	Renderer2D::Statistics Stats;

	float LineWidth = 2.0f;
};

static Renderer2DData renderer2D;

static void DrawIndexed(const std::shared_ptr<HeapVertexArray>& vertexArray, uint32_t indexCount)
{
	vertexArray->Bind();
	uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
	GLCALL(glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr));
}

static void DrawLines(const std::shared_ptr<HeapVertexArray>& vertexArray, uint32_t vertexCount)
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
	// Shaders
	renderer2D.QuadShader = std::make_shared<Shader>("Assets/Shaders/Renderer2DQuad");
	renderer2D.ParticleShader = std::make_shared<Shader>("Assets/Shaders/Renderer2DParticle");
	renderer2D.QuadIndexShader = std::make_shared<Shader>("Assets/Shaders/Renderer2DQuadIndex");
	renderer2D.CircleShader = std::make_shared<Shader>("Assets/Shaders/Renderer2DCircle");
	renderer2D.LineShader = std::make_shared<Shader>("Assets/Shaders/Renderer2DLine");
	renderer2D.TextShader = std::make_shared<Shader>("Assets/Shaders/Renderer2DText");

	// White Texture
	WhiteTexture = std::make_shared<Texture>();
	uint32_t whiteTextureData = 0xffffffff;
	WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

	for (Batch& batch : renderer2D.batches)
	{
		batch.Init();

		ResetQuadBatch(batch);
		ResetCircleBatch(batch);
		ResetLineBatch(batch);
		ResetTextBatch(batch);
	}


	renderer2D.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
	renderer2D.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
	renderer2D.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
	renderer2D.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
}

void Renderer2D::Shutdown()
{
	WhiteTexture->Delete();

	for (Batch& batch : renderer2D.batches)
		batch.Delete();
}

void Renderer2D::Update(BatchType type, RenderTarget target)
{
	Batch& batch = renderer2D.batches[type];

	FrameBuffer* buffer;

	switch (type)
	{
		case WORLD:
		{
			if (Renderer::Settings()->particlesLight.isEnabled)
			{
				buffer = target.GetFrameBuffer("gBuffer");
				break;
			}
			buffer = target.GetFrameBuffer("postBuffer");
			break;
		}
			
		case UI:		buffer = target.GetFrameBuffer("uiBuffer");		break;
		case EDITOR:	buffer = target.GetFrameBuffer("postBuffer");	break;
		default: return;
	}

	buffer->Bind();

	GLCALL(glDisable(GL_CULL_FACE));
	GLCALL(glEnable(GL_BLEND));

	DrawQuadBatch(batch, type);
	DrawCircleBatch(batch);
	DrawLineBatch(batch);
	DrawTextBatch(batch);

	GLCALL(glEnable(GL_CULL_FACE));

	buffer->Unbind();
}

void Renderer2D::UpdateIndexed(BatchType type, RenderTarget target)
{
	Batch& batch = renderer2D.batches[type];

	GLCALL(glDisable(GL_CULL_FACE));
	DrawQuadIndexedBatch(batch);
	GLCALL(glEnable(GL_CULL_FACE));
}

void Renderer2D::ResetBatches()
{
	for (Batch& batch : renderer2D.batches) {
		ResetQuadBatch(batch);
		ResetCircleBatch(batch);
		ResetLineBatch(batch);
		ResetTextBatch(batch);
	}
}

void Renderer2D::NextQuadBatch(Batch& batch)
{
	//TODO: Cambiar esta funcion, tal y como está no va a funcionar
	/*DrawQuadBatch(batch);
	ResetQuadBatch(batch);*/
}

void Renderer2D::DrawQuadBatch(const Batch& batch, BatchType type)
{
	if (batch.QuadIndexCount)
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)batch.QuadVertexBufferPtr - (uint8_t*)batch.QuadVertexBufferBase);
		batch.QuadVertexBuffer->SetData(batch.QuadVertexBufferBase, dataSize);

		// Bind textures
		for (uint32_t i = 0; i < batch.TextureSlotIndex; i++)
			batch.TextureSlots[i]->Bind(i);

		type == BatchType::UI ?
			renderer2D.QuadShader->Bind() :
			renderer2D.ParticleShader->Bind();
		DrawIndexed(batch.QuadVertexArray, batch.QuadIndexCount);
		renderer2D.Stats.DrawCalls++;
		renderer2D.QuadShader->UnBind();
	}
}

void Renderer2D::DrawQuadIndexedBatch(const Batch& batch)
{
	if (batch.QuadIndexCount)
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)batch.QuadVertexBufferPtr - (uint8_t*)batch.QuadVertexBufferBase);
		batch.QuadVertexBuffer->SetData(batch.QuadVertexBufferBase, dataSize);

		renderer2D.QuadIndexShader->Bind();
		DrawIndexed(batch.QuadVertexArray, batch.QuadIndexCount);
		renderer2D.Stats.DrawCalls++;
		renderer2D.QuadIndexShader->UnBind();
	}
}

void Renderer2D::DrawCircleBatch(const Batch& batch)
{
	if (batch.CircleIndexCount)
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)batch.CircleVertexBufferPtr - (uint8_t*)batch.CircleVertexBufferBase);
		batch.CircleVertexBuffer->SetData(batch.CircleVertexBufferBase, dataSize);

		renderer2D.CircleShader->Bind();
		DrawIndexed(batch.CircleVertexArray, batch.CircleIndexCount);
		renderer2D.Stats.DrawCalls++;
		renderer2D.CircleShader->UnBind();
	}
}

void Renderer2D::DrawLineBatch(const Batch& batch)
{
	if (batch.LineVertexCount)
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)batch.LineVertexBufferPtr - (uint8_t*)batch.LineVertexBufferBase);
		batch.LineVertexBuffer->SetData(batch.LineVertexBufferBase, dataSize);

		renderer2D.LineShader->Bind();
		SetLineWidth(renderer2D.LineWidth);
		DrawLines(batch.LineVertexArray, batch.LineVertexCount);
		renderer2D.Stats.DrawCalls++;
		renderer2D.LineShader->UnBind();
	}
}

void Renderer2D::DrawTextBatch(const Batch& batch)
{
	if (batch.TextIndexCount)
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)batch.TextVertexBufferPtr - (uint8_t*)batch.TextVertexBufferBase);
		batch.TextVertexBuffer->SetData(batch.TextVertexBufferBase, dataSize);

		renderer2D.FontAtlasTexture->Bind();

		renderer2D.TextShader->Bind();
		DrawIndexed(batch.TextVertexArray, batch.TextIndexCount);
		renderer2D.Stats.DrawCalls++;
		renderer2D.TextShader->UnBind();
	}
}

void Renderer2D::ResetQuadBatch(Batch& batch)
{
	batch.QuadIndexCount = 0;
	batch.QuadVertexBufferPtr = batch.QuadVertexBufferBase;
	batch.TextureSlotIndex = 1;
}

void Renderer2D::ResetCircleBatch(Batch& batch)
{
	batch.CircleIndexCount = 0;
	batch.CircleVertexBufferPtr = batch.CircleVertexBufferBase;
}

void Renderer2D::ResetLineBatch(Batch& batch)
{
	batch.LineVertexCount = 0;
	batch.LineVertexBufferPtr = batch.LineVertexBufferBase;
}

void Renderer2D::ResetTextBatch(Batch& batch)
{
	batch.TextIndexCount = 0;
	batch.TextVertexBufferPtr = batch.TextVertexBufferBase;
}

void Renderer2D::DrawQuad(BatchType type, const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
{
	DrawQuad(type, { position.x, position.y, 0.0f }, size, color);
}

void Renderer2D::DrawQuad(BatchType type, const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	DrawQuad(type, transform, color);
}

void Renderer2D::DrawQuad(BatchType type, const glm::vec2& position, const glm::vec2& size, ResourceId spriteID, const glm::vec4& tintColor, float tilingFactor)
{
	DrawQuad(type, { position.x, position.y, 0.0f }, size, spriteID, tintColor, tilingFactor);
}

void Renderer2D::DrawQuad(BatchType type, const glm::vec3& position, const glm::vec2& size, ResourceId spriteID, const glm::vec4& tintColor, float tilingFactor)
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	DrawQuad(type, transform, spriteID, tintColor, tilingFactor);
}

void Renderer2D::DrawQuad(BatchType type, const glm::vec2& position, const glm::vec2& size, ResourceId imageID, const TexCoordsSection& texCoords, const glm::vec4& tintColor, float tilingFactor)
{
	DrawQuad(type, { position.x, position.y, 0.0f }, size, imageID, texCoords, tintColor, tilingFactor);
}

void Renderer2D::DrawQuad(BatchType type, const glm::vec3& position, const glm::vec2& size, ResourceId imageID, const TexCoordsSection& texCoords, const glm::vec4& tintColor, float tilingFactor)
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	DrawQuad(type, transform, imageID, texCoords, tintColor, tilingFactor);
}

void Renderer2D::DrawQuad(BatchType type, const glm::mat4& transform, const glm::vec4& color)
{
	Batch& batch = renderer2D.batches[type];
	constexpr size_t quadVertexCount = 4;
	const float textureIndex = 0.0f; // White Texture
	constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
	const float tilingFactor = 1.0f;

	if (batch.QuadIndexCount >= MaxIndices)
		NextQuadBatch(batch);

	for (size_t i = 0; i < quadVertexCount; i++)
	{
		batch.QuadVertexBufferPtr->Position = transform * renderer2D.QuadVertexPositions[i];
		batch.QuadVertexBufferPtr->Color = color;
		batch.QuadVertexBufferPtr->TexCoord = textureCoords[i];
		batch.QuadVertexBufferPtr->TexIndex = textureIndex;
		batch.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		batch.QuadVertexBufferPtr++;
	}

	batch.QuadIndexCount += 6;

	renderer2D.Stats.QuadCount++;
}

void Renderer2D::DrawQuad(BatchType type, const glm::mat4& transform, ResourceId spriteID, const glm::vec4& tintColor, float tilingFactor)
{
	Batch& batch = renderer2D.batches[type];
	constexpr size_t quadVertexCount = 4;
	constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

	Texture* sprite = Resources::GetResourceById<Texture>(spriteID);

	if (batch.QuadIndexCount >= MaxIndices)
		NextQuadBatch(batch);

	float textureIndex = 0.0f;
	for (uint32_t i = 1; i < batch.TextureSlotIndex; i++)
	{
		if (*batch.TextureSlots[i] == *sprite)
		{
			textureIndex = (float)i;
			break;
		}
	}

	if (textureIndex == 0.0f)
	{
		if (batch.TextureSlotIndex >= MaxTextureSlots)
			NextQuadBatch(batch);

		textureIndex = (float)batch.TextureSlotIndex;
		batch.TextureSlots[batch.TextureSlotIndex] = sprite;
		batch.TextureSlotIndex++;
	}

	for (size_t i = 0; i < quadVertexCount; i++)
	{
		batch.QuadVertexBufferPtr->Position = transform * renderer2D.QuadVertexPositions[i];
		batch.QuadVertexBufferPtr->Color = tintColor;
		batch.QuadVertexBufferPtr->TexCoord = textureCoords[i];
		batch.QuadVertexBufferPtr->TexIndex = textureIndex;
		batch.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		batch.QuadVertexBufferPtr++;
	}

	batch.QuadIndexCount += 6;

	renderer2D.Stats.QuadCount++;
}

void Renderer2D::DrawQuad(BatchType type, const glm::mat4& transform, ResourceId imageID, const TexCoordsSection& texCoords, const glm::vec4& tintColor, float tilingFactor)
{
	Batch& batch = renderer2D.batches[type];
	constexpr size_t quadVertexCount = 4;
	glm::vec2 textureCoords[] = { texCoords.leftBottom, texCoords.rightBottom, texCoords.rightTop, texCoords.leftTop };

	Texture* sprite = Resources::GetResourceById<Texture>(imageID);

	if (batch.QuadIndexCount >= MaxIndices)
		NextQuadBatch(batch);

	float textureIndex = 0.0f;
	for (uint32_t i = 1; i < batch.TextureSlotIndex; i++)
	{
		if (*batch.TextureSlots[i] == *sprite)
		{
			textureIndex = (float)i;
			break;
		}
	}

	if (textureIndex == 0.0f)
	{
		if (batch.TextureSlotIndex >= MaxTextureSlots)
			NextQuadBatch(batch);

		textureIndex = (float)batch.TextureSlotIndex;
		batch.TextureSlots[batch.TextureSlotIndex] = sprite;
		batch.TextureSlotIndex++;
	}

	for (size_t i = 0; i < quadVertexCount; i++)
	{
		batch.QuadVertexBufferPtr->Position = transform * renderer2D.QuadVertexPositions[i];
		batch.QuadVertexBufferPtr->Color = tintColor;
		batch.QuadVertexBufferPtr->TexCoord = textureCoords[i];
		batch.QuadVertexBufferPtr->TexIndex = textureIndex;
		batch.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		batch.QuadVertexBufferPtr++;
	}

	batch.QuadIndexCount += 6;

	renderer2D.Stats.QuadCount++;
}

void Renderer2D::DrawRotatedQuad(BatchType type, const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
{
	DrawRotatedQuad(type, { position.x, position.y, 0.0f }, size, rotation, color);
}

void Renderer2D::DrawRotatedQuad(BatchType type, const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
		* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	DrawQuad(type, transform, color);
}

void Renderer2D::DrawRotatedQuad(BatchType type, const glm::vec2& position, const glm::vec2& size, float rotation, ResourceId spriteID, const glm::vec4& tintColor, float tilingFactor)
{
	DrawRotatedQuad(type, { position.x, position.y, 0.0f }, size, rotation, spriteID, tintColor, tilingFactor);
}

void Renderer2D::DrawRotatedQuad(BatchType type, const glm::vec3& position, const glm::vec2& size, float rotation, ResourceId spriteID, const glm::vec4& tintColor, float tilingFactor)
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
		* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	DrawQuad(type, transform, spriteID, tintColor, tilingFactor);
}

void Renderer2D::DrawCircle(BatchType type, const glm::mat4& transform, const glm::vec4& color, float thickness /*= 1.0f*/, float fade /*= 0.005f*/)
{
	Batch& batch = renderer2D.batches[type];
	for (size_t i = 0; i < 4; i++)
	{
		batch.CircleVertexBufferPtr->WorldPosition = transform * renderer2D.QuadVertexPositions[i];
		batch.CircleVertexBufferPtr->LocalPosition = renderer2D.QuadVertexPositions[i] * 2.0f;
		batch.CircleVertexBufferPtr->Color = color;
		batch.CircleVertexBufferPtr->Thickness = thickness;
		batch.CircleVertexBufferPtr->Fade = fade;
		batch.CircleVertexBufferPtr++;
	}

	batch.CircleIndexCount += 6;

	renderer2D.Stats.QuadCount++;
}

void Renderer2D::DrawLine(BatchType type, const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color)
{
	Batch& batch = renderer2D.batches[type];
	batch.LineVertexBufferPtr->Position = p0;
	batch.LineVertexBufferPtr->Color = color;
	batch.LineVertexBufferPtr++;

	batch.LineVertexBufferPtr->Position = p1;
	batch.LineVertexBufferPtr->Color = color;
	batch.LineVertexBufferPtr++;

	batch.LineVertexCount += 2;
}

void Renderer2D::DrawRect(BatchType type, const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
{
	glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
	glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
	glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
	glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

	DrawLine(type, p0, p1, color);
	DrawLine(type, p1, p2, color);
	DrawLine(type, p2, p3, color);
	DrawLine(type, p3, p0, color);
}

void Renderer2D::DrawRect(BatchType type, const glm::mat4& transform, const glm::vec4& color)
{
	glm::vec3 lineVertices[4];
	for (size_t i = 0; i < 4; i++)
		lineVertices[i] = transform * renderer2D.QuadVertexPositions[i];

	DrawLine(type, lineVertices[0], lineVertices[1], color);
	DrawLine(type, lineVertices[1], lineVertices[2], color);
	DrawLine(type, lineVertices[2], lineVertices[3], color);
	DrawLine(type, lineVertices[3], lineVertices[0], color);
}

void Renderer2D::DrawString(BatchType type, const std::string& string, Font* font, const glm::vec2& position, const glm::vec2& size, const TextParams& textParams)
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), { position.x, position.y, 0.0f })
		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	DrawString(type, string, font, transform, textParams);
}

void Renderer2D::DrawString(BatchType type, const std::string& string, Font* font, const glm::mat4& transform, const TextParams& textParams)
{
	Batch& batch = renderer2D.batches[type];
	const auto& fontGeometry = font->GetMSDFData()->FontGeometry;
	const auto& metrics = fontGeometry.getMetrics();
	std::shared_ptr<Texture> fontAtlas = font->GetAtlasTexture();

	renderer2D.FontAtlasTexture = fontAtlas;

	double x = 0.0;
	double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
	double y = 0.0;

	const float spaceGlyphAdvance = fontGeometry.getGlyph(' ')->getAdvance();

	for (size_t i = 0; i < string.size(); i++)
	{
		char character = string[i];
		if (character == '\r')
			continue;

		if (character == '\n')
		{
			x = 0;
			y += fsScale * metrics.lineHeight + textParams.LineSpacing;
			continue;
		}

		if (character == ' ')
		{
			float advance = spaceGlyphAdvance;
			if (i < string.size() - 1)
			{
				char nextCharacter = string[i + 1];
				double dAdvance;
				fontGeometry.getAdvance(dAdvance, character, nextCharacter);
				advance = (float)dAdvance;
			}

			x += fsScale * advance + textParams.Kerning;
			continue;
		}

		if (character == '\t')
		{
			x += 4.0f * (fsScale * spaceGlyphAdvance + textParams.Kerning);
			continue;
		}

		auto glyph = fontGeometry.getGlyph(character);
		if (!glyph)
			glyph = fontGeometry.getGlyph('?');
		if (!glyph)
			return;

		double al, ab, ar, at;
		glyph->getQuadAtlasBounds(al, ab, ar, at);
		glm::vec2 texCoordMin((float)al, (float)ab);
		glm::vec2 texCoordMax((float)ar, (float)at);

		double pl, pb, pr, pt;
		glyph->getQuadPlaneBounds(pl, pt, pr, pb);
		glm::vec2 quadMin((float)pl, (float)pb);
		glm::vec2 quadMax((float)pr, (float)pt);

		quadMin.y *= -1, quadMax.y *= -1;
		quadMin *= fsScale, quadMax *= fsScale;
		quadMin += glm::vec2(x, y);
		quadMax += glm::vec2(x, y);

		float texelWidth = 1.0f / fontAtlas->GetWidth();
		float texelHeight = 1.0f / fontAtlas->GetHeight();
		texCoordMin *= glm::vec2(texelWidth, texelHeight);
		texCoordMax *= glm::vec2(texelWidth, texelHeight);

		// render here

		batch.TextVertexBufferPtr->Position = transform * glm::vec4(quadMin, 0.0f, 1.0f);
		batch.TextVertexBufferPtr->Color = textParams.Color;
		batch.TextVertexBufferPtr->TexCoord = { texCoordMin.x, texCoordMax.y };
		batch.TextVertexBufferPtr++;

		batch.TextVertexBufferPtr->Position = transform * glm::vec4(quadMax.x, quadMin.y, 0.0f, 1.0f);
		batch.TextVertexBufferPtr->Color = textParams.Color;
		batch.TextVertexBufferPtr->TexCoord = texCoordMax; 
		batch.TextVertexBufferPtr++;

		batch.TextVertexBufferPtr->Position = transform * glm::vec4(quadMax, 0.0f, 1.0f);
		batch.TextVertexBufferPtr->Color = textParams.Color;
		batch.TextVertexBufferPtr->TexCoord = { texCoordMax.x, texCoordMin.y };
		batch.TextVertexBufferPtr++;

		batch.TextVertexBufferPtr->Position = transform * glm::vec4(quadMin.x, quadMax.y, 0.0f, 1.0f);
		batch.TextVertexBufferPtr->Color = textParams.Color;
		batch.TextVertexBufferPtr->TexCoord = texCoordMin;
		batch.TextVertexBufferPtr++;

		batch.TextIndexCount += 6;
		renderer2D.Stats.QuadCount++;

		if (i < string.size() - 1)
		{
			double advance = glyph->getAdvance();
			char nextCharacter = string[i + 1];
			fontGeometry.getAdvance(advance, character, nextCharacter);

			x += fsScale * advance + textParams.Kerning;
		}
	}
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

