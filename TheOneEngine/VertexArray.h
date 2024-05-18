#pragma once

#include "Buffer.h"

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void Bind() const;
	void Unbind() const;

	void Delete();

	void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
	void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

	uint32_t GetRendererID() { return m_RendererID; }

	const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
	const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }
private:
	uint32_t m_RendererID = 0;
	uint32_t m_VertexBufferIndex = 0;
	std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
	std::shared_ptr<IndexBuffer> m_IndexBuffer;
};

class StackVertexArray
{
public:
	StackVertexArray();
	~StackVertexArray();

	void Bind() const;
	void Unbind() const;

	void Delete();

	void AddVertexBuffer(const VertexBuffer& vertexBuffer);
	void SetIndexBuffer(const IndexBuffer& indexBuffer);

	uint32_t GetRendererID() { return m_RendererID; }

	const std::vector<VertexBuffer>& GetVertexBuffers() const { return m_VertexBuffers; }
	const IndexBuffer& GetIndexBuffer() const { return m_IndexBuffer; }
private:
	uint32_t m_RendererID = 0;
	uint32_t m_VertexBufferIndex = 0;
	std::vector<VertexBuffer> m_VertexBuffers;
	IndexBuffer m_IndexBuffer;
};