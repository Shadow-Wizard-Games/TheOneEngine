#include "UniformBuffer.h"

#include <GL/glew.h>

UniformBuffer::UniformBuffer(uint size, uint binding)
{
	GLCALL(glCreateBuffers(1, &m_RendererID));
	GLCALL(glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW));
	GLCALL(glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID));
}

UniformBuffer::~UniformBuffer()
{
	Delete();
}


void UniformBuffer::SetData(const void* data, uint size, uint offset)
{
	GLCALL(glNamedBufferSubData(m_RendererID, offset, size, data));
}

void UniformBuffer::Delete()
{
	if (m_RendererID)
	{
		GLCALL(glDeleteBuffers(1, &m_RendererID));
		m_RendererID = 0;
	}
}
