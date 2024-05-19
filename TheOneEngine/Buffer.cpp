#include "Buffer.h"

#include <GL/glew.h>

/////////////////////////////////////////////////////////////////////////////
// VertexBuffer /////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

VertexBuffer::VertexBuffer(uint32_t size)
{
	GLCALL(glCreateBuffers(1, &m_RendererID));
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
	GLCALL(glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));
}

VertexBuffer::VertexBuffer(float* vertices, uint32_t size)
{
	GLCALL(glCreateBuffers(1, &m_RendererID));
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
	GLCALL(glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer()
{
	//Delete();
}

void VertexBuffer::Bind() const
{
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void VertexBuffer::Unbind() const
{
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexBuffer::Delete()
{
	if (m_RendererID)
	{
		GLCALL(glDeleteBuffers(1, &m_RendererID));
		m_RendererID = 0;
	}
}

void VertexBuffer::SetData(const void* data, uint32_t size, uint32_t offset) const
{
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
	GLCALL(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
}

/////////////////////////////////////////////////////////////////////////////
// IndexBuffer //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

IndexBuffer::IndexBuffer(uint32_t* indices, uint32_t count)
	: m_Count(count)
{
	GLCALL(glCreateBuffers(1, &m_RendererID));

	// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
	// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
	GLCALL(glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
	//Delete();
}

void IndexBuffer::Bind() const
{
	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
}

void IndexBuffer::Unbind() const
{
	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void IndexBuffer::Delete()
{
	if (this != nullptr && m_RendererID)
	{
		GLCALL(glDeleteBuffers(1, &m_RendererID));
		m_RendererID = 0;
	}
}
