#include "Uniform.h"
#include <GL/glew.h>
#include "Defs.h"

#include <glm/glm.hpp>

//Sampler2D x : OpenGL id y: ResourceId to retrieve the path when saving
void Uniform::sendToShader(uint16_t shaderProgram, int& textureId)
{
	if (type == UniformType::Invalid)
		return;

	if (uniformID == -1)
		uniformID = glGetUniformLocation(shaderProgram, name.c_str());

	if (uniformID == -1)
		return;

	switch (type)
	{
	case UniformType::Bool:
		GLCALL(glUniform1i(uniformID, *(GLint*)data));
		break;
	case UniformType::Int:
		GLCALL(glUniform1i(uniformID, *(GLint*)data));
		break;
	case UniformType::Uint:
		GLCALL(glUniform1ui(uniformID, *(GLint*)data));
		break;
	case UniformType::Float:
		GLCALL(glUniform1f(uniformID, *(GLfloat*)data));
		break;
	case UniformType::fVec2:
		GLCALL(glUniform2fv(uniformID, 1, (GLfloat*)data));
		break;
	case UniformType::fVec3:
		GLCALL(glUniform3fv(uniformID, 1, (GLfloat*)data));
		break;
	case UniformType::fVec4:
		GLCALL(glUniform4fv(uniformID, 1, (GLfloat*)data));
		break;
	case UniformType::Mat2:
		GLCALL(glUniformMatrix2fv(uniformID, 1, GL_FALSE, (GLfloat*)data));
		break;
	case UniformType::Mat3:
		GLCALL(glUniformMatrix3fv(uniformID, 1, GL_FALSE, (GLfloat*)data));
		break;
	case UniformType::Mat4:
		GLCALL(glUniformMatrix4fv(uniformID, 1, GL_FALSE, (GLfloat*)data));
		break;
	case UniformType::Sampler2D:
	{
		if (((SamplerData*)data)->tex_id > 0)
		{
			GLCALL(glActiveTexture(textureId));
			GLCALL(glBindTexture(GL_TEXTURE_2D, ((SamplerData*)data)->tex_id));
			GLCALL(glUniform1i(uniformID, textureId - GL_TEXTURE0));
			textureId++;
		}
	}break;
	default:
		break;
	}
}

void Uniform::setEmptyData()
{
	switch (type)
	{
	case UniformType::Bool:
		setData(false, type);
		break;
	case UniformType::Int:
		setData(0, type);
		break;
	case UniformType::Uint:
		setData(0, type);
		break;
	case UniformType::Float:
		setData(1.0f, type);
		break;
	case UniformType::fVec2:
		setData(glm::vec2(), type);
		break;
	case UniformType::fVec3:
		setData(glm::vec3(0.0f), type);
		break;
	case UniformType::fVec4:
		setData(glm::vec4(1.0f), type);
		break;
	case UniformType::Mat2:
		setData(glm::mat2(), type);
		break;
	case UniformType::Mat3:
		setData(glm::mat3(), type);
		break;
	case UniformType::Mat4:
		setData(glm::mat4(), type);
		break;
	case UniformType::Sampler2D:
	{
		SamplerData data;
		data.tex_id = -1;
		data.resource_id = -1;
		setData(data, type);
	}
	break;
	default:
		break;
	}
}