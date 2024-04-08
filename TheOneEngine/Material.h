#pragma once
#include <glm/glm.hpp>
#include "Uniform.h"
#include "Log.h"

class Shader;

class Material
{
public:
	Material();
	// Create new material based on a shader
	Material(Shader* shader);
	// Load existing
	Material(const std::string& file);
	~Material();

	void Bind(int id = 0x84C0);
	void Bind(uint32_t program_id, int id = 0x84C0);

	void UnBind(int id = 0x84C0);

	void Load(const std::string& path);
	void Save(const std::string& path);

	void Refresh();


	void setShader(Shader* shader, const std::string& path);
	template<class T> void SetUniformData(const std::string& name, const T& value);

	inline Shader* getShader() const { return shader; }
	inline const char* getShaderPath() { return shaderPath.c_str(); }
	Uniform* getUniform(const std::string& name);
	inline std::vector<Uniform>& getUniforms() { return uniforms; }
	inline size_t getUniformIndex(const char* name)
	{
		for (size_t i = 0; i < uniforms.size(); i++)
		{
			if (uniforms[i].name == name)
			{
				return i;
			}
		}
		LOG(LogType::LOG_ERROR, "Can't find uniform with name %s in material", name);
		return -1;
	}

private:

	void LoadBasicShader();

private:
	std::string shaderPath;
	std::string materialPath;

	Shader* shader = nullptr;
	std::vector<Uniform> uniforms;
};

template<class T>
void Material::SetUniformData(const std::string& name, const T& value)
{
	Uniform* uniform = getUniform(name);
	if (!uniform)
	{
		LOG(LogType::LOG_ERROR, "Uniform name %s doesn't exist in the current material", name);
		return;
	}
	uniform->setData(value, uniform->getType());
}