#include "Shader.h"
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include "Defs.h"
#include "Resources.h"

Shader::Shader()
{
	compileState = State::ToCompile;
}

Shader::Shader(const std::string& path)
{
	compileState = State::ToCompile;
	Init(path.c_str());
}

Shader::~Shader() {}

void Shader::Init(const std::string& filename)
{
	Compile(filename);
}

void Shader::Compile(const std::string& filename)
{
	std::string vertexFile = filename;
	vertexFile += ".vs";

	std::string fragmentFile = filename;
	fragmentFile += ".fs";

	std::string geometryFile = filename;
	geometryFile += ".gs";

	std::string* vertexShaderSourceStr = getFileData(vertexFile.c_str());

	if (!vertexShaderSourceStr) {
		std::string msg = "Couldn't open file: ";
		msg += vertexFile;
		LOG(LogType::LOG_ERROR, msg.c_str());
		compileState = State::Error;
		return;
	}

	const char* vertexShaderSource = vertexShaderSourceStr->c_str();

	std::string* fragmentShaderSourceStr = getFileData(fragmentFile.c_str());

	if (!fragmentShaderSourceStr) {
		std::string msg = "Couldn't open file: ";
		msg += fragmentFile;
		LOG(LogType::LOG_ERROR, msg.c_str());
		compileState = State::Error;
		return;
	}

	const char* fragmentShaderSource = fragmentShaderSourceStr->c_str();

	std::string* geometryShaderSourceStr = getFileData(geometryFile.c_str());
	bool hasGS = geometryShaderSourceStr;

	bool retflag;
	CompileFiles(vertexShaderSource, fragmentShaderSource, hasGS, geometryShaderSourceStr, retflag);
	delete vertexShaderSourceStr;
	delete fragmentShaderSourceStr;
	delete geometryShaderSourceStr;

	if (retflag)
		return;

	path = filename;
}

void Shader::CompileFiles(const char* vertexShaderSource, const char* fragmentShaderSource, bool hasGS, std::string* geometryShaderSourceStr, bool& retflag)
{
	GLERR;
	retflag = true;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLCALL(glShaderSource(vertexShader, 1, &vertexShaderSource, NULL));
	GLCALL(glCompileShader(vertexShader));

	int success;
	char infoLog[512];

	GLCALL(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success));

	if (!success) {
		GLCALL(glGetShaderInfoLog(vertexShader, 512, NULL, infoLog));

		std::string msg = "Vertex shader compile error: ";
		msg += infoLog;

		LOG(LogType::LOG_ERROR, msg.c_str());
		compileState = State::Error;
		return;
	}

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLCALL(glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL));
	GLCALL(glCompileShader(fragmentShader));

	GLCALL(glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success));

	if (!success) {
		GLCALL(glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog));
		std::string msg = "Fragment shader compile error: ";
		msg += infoLog;

		LOG(LogType::LOG_ERROR, msg.c_str());
		compileState = State::Error;
		return;
	}

	ProgramID = glCreateProgram();
	GLCALL(glAttachShader(ProgramID, vertexShader));
	GLCALL(glAttachShader(ProgramID, fragmentShader));


	unsigned int geometryShader;
	if (hasGS)
	{
		const char* geometryShaderSource = geometryShaderSourceStr->c_str();
		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		GLCALL(glShaderSource(geometryShader, 1, &geometryShaderSource, NULL));
		GLCALL(glCompileShader(geometryShader));

		GLCALL(glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success));

		if (!success) {
			GLCALL(glGetShaderInfoLog(geometryShader, 512, NULL, infoLog));
			std::string msg = "Geometry shader compile error: ";
			msg += infoLog;

			LOG(LogType::LOG_ERROR, msg.c_str());
			compileState = State::Error;
			return;
		}
		GLCALL(glAttachShader(ProgramID, geometryShader));
	}

	GLCALL(glLinkProgram(ProgramID));

	GLCALL(glGetProgramiv(ProgramID, GL_LINK_STATUS, &success));

	if (!success) {
		GLCALL(glGetProgramInfoLog(ProgramID, 512, NULL, infoLog));
		std::string msg = "Shader program link error: ";
		msg += infoLog;

		LOG(LogType::LOG_ERROR, msg.c_str());
		compileState = State::Error;
		return;
	}

	GLCALL(glDeleteShader(vertexShader));
	GLCALL(glDeleteShader(fragmentShader));
	if (hasGS)
		GLCALL(glDeleteShader(geometryShader));

	retflag = false;

	compileState = State::Compiled;

	modelMatrixID = glGetUniformLocation(ProgramID, "u_Model");
	//projectionMatrixID = GLCALL(glGetUniformLocation(ProgramID, "projection");
	//viewMatrixID = GLCALL(glGetUniformLocation(ProgramID, "view");
	GLERR;
}

bool Shader::LoadFromTOEasset(const std::string& filename)
{
	json shaderJSON = Resources::OpenJSON(filename);

	if (shaderJSON.empty())
		LOG(LogType::LOG_ERROR, "Shader JSON not found");

	std::string vertexShader;
	std::string fragmentShader;
	std::string geometryShader;

	bool hasGeometry = false;

	if (shaderJSON.contains("vertex"))
		vertexShader = shaderJSON["vertex"];
	if (shaderJSON.contains("fragment"))
		fragmentShader = shaderJSON["fragment"];
	if (shaderJSON.contains("geometry"))
		geometryShader = shaderJSON["geometry"];

	if (!geometryShader.empty())
		hasGeometry = true;

	bool ret = false;
	CompileFiles(vertexShader.c_str(), fragmentShader.c_str(), hasGeometry, &geometryShader, ret);

	if (shaderJSON.contains("uniforms"))
	{
		json uniforms = shaderJSON["uniforms"];
		for (const auto& uniform : uniforms)
		{
			addUniform(uniform["Name"], (UniformType)uniform["Type"]);
		}
	}

	path = filename;

	return true;
}

void Shader::Bind()
{
	if (compileState != State::Compiled)
		return;
	GLCALL(glUseProgram(ProgramID));
}

void Shader::UnBind()
{
	GLCALL(glUseProgram(0));
}

void Shader::Delete()
{
	GLCALL(glDeleteProgram(ProgramID));
}

std::string* Shader::getFileData(const char* file)
{
	std::fstream shaderFile;

	shaderFile.open(file, std::ios::in);

	if (!shaderFile.is_open())
		return NULL;

	std::stringstream buffer;

	buffer << shaderFile.rdbuf();

	shaderFile.close();

	return new std::string(buffer.str());
}

unsigned int Shader::getUniformLocation(const char* uniform_name)
{
	return glGetUniformLocation(ProgramID, uniform_name);
}

void Shader::SetInt(const std::string& name, int value)
{
	GLCALL(glUniform1i(getUniformLocation(name.c_str()), value));
}

void Shader::SetUInt(const std::string& name, unsigned int value)
{
	GLCALL(glUniform1i(getUniformLocation(name.c_str()), value));
}

void Shader::SetFloat(const std::string& name, float value)
{
	GLCALL(glUniform1f(getUniformLocation(name.c_str()), value));
}

void Shader::SetFloat2(const std::string& name, const glm::vec2& value)
{
	GLCALL(glUniform2f(getUniformLocation(name.c_str()), value.x, value.y));
}

void Shader::SetFloat3(const std::string& name, const glm::vec3& value)
{
	GLCALL(glUniform3f(getUniformLocation(name.c_str()), value.x, value.y, value.z));
}

void Shader::SetFloat4(const std::string& name, const glm::vec4& value)
{
	GLCALL(glUniform4f(getUniformLocation(name.c_str()), value.x, value.y, value.z, value.w));
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value)
{
	GLCALL(glUniformMatrix4fv(getUniformLocation(name.c_str()), 1, GL_FALSE, glm::value_ptr(value)));
}

void Shader::addUniform(const std::string& name, const UniformType type)
{
	UniformField* uniform = getUniform(name.c_str());
	//Checking if we are
	if (uniform)
	{
		if (uniform->type == type)
			return;

		uniform->type = type;
		uniform->location = glGetUniformLocation(ProgramID, name.c_str());
		return;
	}
	UniformField field;
	field.name = name;
	field.type = type;
	field.location = glGetUniformLocation(ProgramID, name.c_str());


	uniforms.emplace_back(field);
}

void Shader::deleteUniform(const char* name)
{
	for (size_t i = 0; i < uniforms.size(); i++)
	{
		if (uniforms[i].name == name)
		{
			uniforms.erase(uniforms.begin() + i);
			i--;
		}
	}
}

void Shader::setUniformType(const char* name, const UniformType type)
{
	UniformField* uniform = getUniform(name);
	if (!uniform)
	{
		LOG(LogType::LOG_ERROR, "Uniform name can't be find");
		return;
	}
	uniform->type = type;
}

void Shader::setUniformName(const char* oldName, const char* newName)
{
	UniformField* uniform = getUniform(oldName);
	if (!uniform)
	{
		LOG(LogType::LOG_ERROR, "Uniform name can't be find in shader");
		return;
	}
	uniform->name = newName;
}

void Shader::SetMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj)
{
	GLCALL(glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, glm::value_ptr(model)));
	GLCALL(glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, glm::value_ptr(view)));
	GLCALL(glUniformMatrix4fv(projectionMatrixID, 1, GL_FALSE, glm::value_ptr(proj)));
}

void Shader::SetViewProj(const glm::mat4& view, const glm::mat4& proj)
{
	GLCALL(glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, glm::value_ptr(view)));
	GLCALL(glUniformMatrix4fv(projectionMatrixID, 1, GL_FALSE, glm::value_ptr(proj)));
}

void Shader::SetModel(const glm::mat4& model)
{
	GLCALL(glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, glm::value_ptr(model)));
}

UniformField* Shader::getUniform(const char* name)
{
	if (uniforms.empty())
		return nullptr;
	for (size_t i = 0; i < uniforms.size(); i++)
	{
		if (uniforms[i].name == name)
		{
			return &uniforms[i];
		}
	}
	return nullptr;
}


template<>
inline void Shader::setUniform<int>(unsigned int uniform_id, int value) 
{
	GLCALL(glUseProgram(ProgramID));
	GLCALL(glUniform1i(uniform_id, value));
}
template<>
inline void Shader::setUniform<unsigned int>(unsigned int uniform_id, unsigned int value) 
{
	GLCALL(glUseProgram(ProgramID));
	GLCALL(glUniform1ui(uniform_id, value));
}
template<>
inline void Shader::setUniform<glm::mat4>(unsigned int uniform_id, glm::mat4 value) 
{
	GLCALL(glUseProgram(ProgramID));
	GLCALL(glUniformMatrix4fv(uniform_id, 1, GL_FALSE, glm::value_ptr(value)));
}

template<>
inline void Shader::setUniform<float>(unsigned int uniform_id, float value) 
{
	GLCALL(glUseProgram(ProgramID));
	GLCALL(glUniform1f(uniform_id, value));
}
template<>
inline void Shader::setUniform<glm::vec2>(unsigned int uniform_id, glm::vec2 value)
{
	GLCALL(glUseProgram(ProgramID));
	GLCALL(glUniform2f(uniform_id, value.x, value.y));
}
template<>
inline void Shader::setUniform<glm::vec3>(unsigned int uniform_id, glm::vec3 value) 
{
	GLCALL(glUseProgram(ProgramID));
	GLCALL(glUniform3f(uniform_id, value.x, value.y, value.z));
}

template<>
inline void Shader::setUniform<glm::vec4>(unsigned int uniform_id, glm::vec4 value)
{
	GLCALL(glUseProgram(ProgramID));
	GLCALL(glUniform4f(uniform_id, value.r, value.g, value.b, value.a));
}

template<class T>
inline void Shader::setUniform(unsigned int uniform_id, T value)
{
	LOG(LogType::LOG_ERROR, "Setting uniform for a non existant specialization.");
}