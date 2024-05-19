#pragma once

#include "Buffer.h"

class HeapVertexArray
{
public:
	HeapVertexArray();
	~HeapVertexArray();

	void Bind() const;
	void Unbind() const;

	void Delete();

	void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
	void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

	const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
	const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }

	bool operator==(const HeapVertexArray& other) const {
		return this->m_RendererID == other.m_RendererID;
	}
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

	void Create();
	void Delete();

	void AddVertexBuffer(const VertexBuffer& vertexBuffer);
	void SetIndexBuffer(const IndexBuffer& indexBuffer);

	const VertexBuffer& GetVertexBuffer() const { return m_VertexBuffer; }
	const IndexBuffer& GetIndexBuffer() const { return m_IndexBuffer; }

	bool operator==(const StackVertexArray& other) const {
		return this->m_RendererID == other.m_RendererID;
	}
private:
	uint32_t m_RendererID = 0;
	uint32_t m_VertexBufferIndex = 0;
	VertexBuffer m_VertexBuffer;
	IndexBuffer m_IndexBuffer;
};