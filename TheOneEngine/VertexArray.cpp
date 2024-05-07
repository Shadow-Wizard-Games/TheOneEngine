#include "VertexArray.h"

#include <GL/glew.h>

static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:    return GL_FLOAT;
	case ShaderDataType::Float2:   return GL_FLOAT;
	case ShaderDataType::Float3:   return GL_FLOAT;
	case ShaderDataType::Float4:   return GL_FLOAT;
	case ShaderDataType::Mat3:     return GL_FLOAT;
	case ShaderDataType::Mat4:     return GL_FLOAT;
	case ShaderDataType::Int:      return GL_INT;
	case ShaderDataType::Int2:     return GL_INT;
	case ShaderDataType::Int3:     return GL_INT;
	case ShaderDataType::Int4:     return GL_INT;
	case ShaderDataType::Bool:     return GL_BOOL;
	}

	return 0;
}

VertexArray::VertexArray()
{
	GLCALL(glCreateVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray()
{
	Delete();
}

void VertexArray::Bind() const
{
	GLCALL(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind() const
{
	GLCALL(glBindVertexArray(0));
}

void VertexArray::Delete()
{
	if (m_RendererID)
	{
		GLCALL(glDeleteVertexArrays(1, &m_RendererID));
		m_RendererID = 0;

		for (auto& vbo : m_VertexBuffers)
			vbo->Delete();

		m_IndexBuffer->Delete();
	}
}

void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
{

	GLCALL(glBindVertexArray(m_RendererID));
	vertexBuffer->Bind();

	const auto& layout = vertexBuffer->GetLayout();
	for (const auto& element : layout)
	{
		switch (element.Type)
		{
		case ShaderDataType::Float:
		case ShaderDataType::Float2:
		case ShaderDataType::Float3:
		case ShaderDataType::Float4:
		{
			GLCALL(glEnableVertexAttribArray(m_VertexBufferIndex));
			GLCALL(glVertexAttribPointer(m_VertexBufferIndex,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset));
			m_VertexBufferIndex++;
			break;
		}
		case ShaderDataType::Int:
		case ShaderDataType::Int2:
		case ShaderDataType::Int3:
		case ShaderDataType::Int4:
		case ShaderDataType::Bool:
		{
			GLCALL(glEnableVertexAttribArray(m_VertexBufferIndex));
			GLCALL(glVertexAttribIPointer(m_VertexBufferIndex,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				layout.GetStride(),
				(const void*)element.Offset));
			m_VertexBufferIndex++;
			break;
		}
		case ShaderDataType::Mat3:
		case ShaderDataType::Mat4:
		{
			uint8_t count = element.GetComponentCount();
			for (uint8_t i = 0; i < count; i++)
			{
				GLCALL(glEnableVertexAttribArray(m_VertexBufferIndex));
				GLCALL(glVertexAttribPointer(m_VertexBufferIndex,
					count,
					ShaderDataTypeToOpenGLBaseType(element.Type),
					element.Normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(),
					(const void*)(element.Offset + sizeof(float) * count * i)));
				GLCALL(glVertexAttribDivisor(m_VertexBufferIndex, 1));
				m_VertexBufferIndex++;
			}
			break;
		}
		}
	}

	m_VertexBuffers.push_back(vertexBuffer);
}

void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
{
	GLCALL(glBindVertexArray(m_RendererID));
	indexBuffer->Bind();

	m_IndexBuffer = indexBuffer;
}
