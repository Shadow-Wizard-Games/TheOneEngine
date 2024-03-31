#pragma once

#include "Uniform.h"
#include "glm/glm.hpp"


#define SHADER_TEST
class Material;
struct UniformField
{
	std::string name;
	UniformType type;
	uint32_t location;
};

class Shader
{
public:
	enum State {
		ToCompile,
		Compiled,
		Error
	};

	Shader();
	Shader(const std::string& path);

	~Shader();

	void Init(const std::string& filename);
	void Compile(const std::string& filename);
	void CompileFiles(const char* vertexShaderSource, const char* fragmentShaderSource, bool hasGS, std::string* geometryShaderSourceStr, bool& retflag);

	bool LoadFromTOEasset(const char* filename);

	void Bind();
	void UnBind();
	void Delete();


	template<class T> void setUniform(unsigned int uniform_id, T value);

	void SetInt(const std::string& name, int value);
	void SetUInt(const std::string& name, unsigned int value);
	void SetFloat(const std::string& name, float value);
	void SetFloat2(const std::string& name, const glm::vec2& value);
	void SetFloat3(const std::string& name, const glm::vec3& value);
	void SetFloat4(const std::string& name, const glm::vec4& value);
	void SetMat4(const std::string& name, const glm::mat4& value);


	// Dynamic shader functions
	void addUniform(const std::string& name, const UniformType type);
	void deleteUniform(const char* name);
	void setUniformType(const char* name, const UniformType type);
	void setUniformName(const char* oldName, const char* newName);
	void SetMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj);
	void SetViewProj(const glm::mat4& view, const glm::mat4& proj);
	void SetModel(const glm::mat4& model);

	unsigned int getUniformLocation(const char* uniform_name);
	UniformField* getUniform(const char* name);
	std::string* getFileData(const char* file);
	inline std::vector<Material*>& getRefs() { return matRefs; }
	inline std::vector<UniformField>& getUniforms() { return uniforms; }
	inline const char* getPath() { return path.c_str(); }
	inline uint32_t getID() { return ProgramID; }
	inline State getState() const { return compileState; }
	inline void setPath(const char* path) { path = path; }

	inline void AddRef(Material* ref)
	{
		matRefs.emplace_back(ref);
	}
	void DeleteRef(Material* ref)
	{
		for (size_t i = 0; i < matRefs.size(); i++)
		{
			if (matRefs[i] == ref)
			{
				matRefs.erase(matRefs.begin() + i);
				break;
			}
		}
	}

protected:
	uint32_t ProgramID;
	State compileState;
	std::vector<UniformField> uniforms;

	std::vector<Material*> matRefs;

	std::string path;


	uint32_t modelMatrixID;
	uint32_t viewMatrixID;
	uint32_t projectionMatrixID;
};