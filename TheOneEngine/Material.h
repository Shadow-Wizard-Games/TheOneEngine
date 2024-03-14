#pragma once
#include <glm/glm.hpp>
#include "Uniform.h"

class Shader;

class Material
{
public:
	Material();
	Material(Shader* shader);
	~Material();

	void Bind(int id = 0x84C0);
	void Bind(uint32_t program_id, int id = 0x84C0);

	void UnBind(int id = 0x84C0);


	void Refresh();
	inline Shader* getShader() const { return shader; }
	void setShader(Shader* shader, const char* path);
	inline const char* getShaderPath() { return shaderPath.c_str(); }
	template<class T>
	void SetUniformData(const char* name, const T& value);
	Uniform* getUniform(const char* name);
	inline std::vector<Uniform>& getUniforms()
	{
		return uniforms;
	}
	inline size_t getUniformIndex(const char* name)
	{
		for (size_t i = 0; i < uniforms.size(); i++)
		{
			if (uniforms[i].name == name)
			{
				return i;
			}
		}
		//SMOL_CORE_ERROR("Can't find uniform with name {0} in material", name);
		return -1;
	}

private:

	void LoadBasicShader();

private:
	std::string shaderPath;

	Shader* shader = nullptr;
	std::vector<Uniform> uniforms;
};

template<class T>
void Material::SetUniformData(const char* name, const T& value)
{
	Uniform* uniform = getUniform(name);
	if (!uniform)
	{
		//SMOL_CORE_CRITICAL("Uniform name {0} doesn't exist in the current material", name);
		return;
	}
	uniform->setData(value, uniform->getType());
}