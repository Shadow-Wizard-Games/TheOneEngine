#include "Material.h"
#include "Shader.h"
#include "Uniform.h"
#include "Resources.h"
#include "Defs.h"

//#include "rapidjson.h"
#include <GL/glew.h>

Material::Material() {}

Material::Material(Shader* shader)
{
	if (!shader)
		LoadBasicShader();
	else
		setShader(shader, shader->getPath());

}

Material::Material(const std::string& file)
{
}

Material::~Material()
{
	if (shader)
		shader->DeleteRef(this);
	uniforms.clear();
}

void Material::Refresh()
{
	std::vector<UniformField>& fields = shader->getUniforms();
	for (UniformField& field : fields)
	{

		Uniform* uniform = getUniform(field.name.c_str());
		if (uniform)
		{
			if (uniform->getType() != field.type)
			{
				uniforms.erase(uniforms.begin() + getUniformIndex(uniform->name.c_str()));
				uniforms.emplace_back(field.name, field.type);
			}
		}
		else
		{
			uniforms.emplace_back(field.name, field.type);
		}
	}

	for (size_t i = 0; i < uniforms.size(); i++)
	{
		size_t j = 0;
		for (; j < fields.size(); j++)
		{
			if (uniforms[i].name == fields[j].name
				&& uniforms[i].getType() == fields[j].type)
				break;
		}
		if (j == fields.size())
		{
			uniforms.erase(uniforms.begin() + i);
			i--;
		}
	}
}

void Material::setShader(Shader* shader, const char* path)
{
	uniforms.clear();
	if (shader)
		shader->DeleteRef(this);

	shader = shader;
	shader->AddRef(this);
	shaderPath = path;
	Refresh();
}

Uniform* Material::getUniform(const char* name)
{
	for (size_t i = 0; i < uniforms.size(); i++)
	{
		if (uniforms[i].name == name)
		{
			return &uniforms[i];
		}
	}
	return nullptr;
}

void Material::LoadBasicShader()
{
	uniforms.clear();
	if (shader)
		shader->DeleteRef(this);

	shader = new Shader("Assets/Shaders/basicShader");
	shader->addUniform("tex", UniformType::Sampler2D);

	shader->AddRef(this);
	shaderPath = "Assets/Shaders/basicShader";

	Refresh();
}

void Material::Bind(int id)
{
	Bind(shader->getID(), id);
}

void Material::Bind(uint32_t program_id, int id)
{
	int texureId = id;
	for (uint32_t i = 0; i < uniforms.size(); i++)
	{
		uniforms[i].sendToShader(program_id, texureId);
	}
}

void Material::UnBind(int id)
{
	int texureId = id;
	for (size_t i = 0; i < uniforms.size(); i++)
	{
		if (uniforms[i].getType() == UniformType::Sampler2D)
		{
			glActiveTexture(texureId);
			glBindTexture(GL_TEXTURE_2D, 0);
			texureId++;
		}
	}

	shader->UnBind();
}

void Material::Load(const std::string& path)
{
}

void Material::Save(const std::string& path)
{
	std::string file = path;

	json doc;
	Resources::standarizePath(shaderPath);
	doc.AddMember("shader", shaderPath.c_str());
	Wiwa::JSONValue uniformsDoc = doc.AddMemberObject("uniforms");
	for (Uniform uniform : uniforms)
	{
		const char* name = uniform.name.c_str();
		switch (uniform.getType())
		{
		case UniformType::Bool:
			uniforms.AddMember(name, uniform.getData<bool>());
			break;
		case UniformType::Int:
			uniforms.AddMember(name, uniform.getData<int>());
			break;
		case UniformType::Uint:
			uniforms.AddMember(name, uniform.getData<unsigned int>());
			break;
		case UniformType::Float:
			uniforms.AddMember(name, uniform.getData<float>());
			break;
		case UniformType::fVec2:
		{
			Wiwa::JSONValue vec = uniforms.AddMemberObject(name);
			vec.AddMember("x", uniform.getData<glm::vec2>().x);
			vec.AddMember("y", uniform.getData<glm::vec2>().y);
		}break;
		case UniformType::fVec3:
		{
			Wiwa::JSONValue vec = uniforms.AddMemberObject(name);
			vec.AddMember("x", uniform.getData<glm::vec3>().x);
			vec.AddMember("y", uniform.getData<glm::vec3>().y);
			vec.AddMember("z", uniform.getData<glm::vec3>().z);
		}break;
		case UniformType::fVec4:
		{
			Wiwa::JSONValue vec = uniforms.AddMemberObject(name);
			vec.AddMember("r", uniform.getData<glm::vec4>().r);
			vec.AddMember("g", uniform.getData<glm::vec4>().g);
			vec.AddMember("b", uniform.getData<glm::vec4>().b);
			vec.AddMember("a", uniform.getData<glm::vec4>().a);
		}break;
		case UniformType::Mat2:
		{
			Wiwa::JSONValue mat = uniforms.AddMemberObject(name);

			Wiwa::JSONValue vec1 = mat.AddMemberObject("vec1");
			vec1.AddMember("x", uniform.getData<glm::mat2>()[0].x);
			vec1.AddMember("y", uniform.getData<glm::mat2>()[0].y);

			Wiwa::JSONValue vec2 = mat.AddMemberObject("vec2");
			vec2.AddMember("x", uniform.getData<glm::mat2>()[1].x);
			vec2.AddMember("y", uniform.getData<glm::mat2>()[1].y);
		}break;
		case UniformType::Mat3:
		{
			Wiwa::JSONValue mat = uniforms.AddMemberObject(name);

			Wiwa::JSONValue vec1 = mat.AddMemberObject("vec1");
			vec1.AddMember("x", uniform.getData<glm::mat3>()[0].x);
			vec1.AddMember("y", uniform.getData<glm::mat3>()[0].y);
			vec1.AddMember("z", uniform.getData<glm::mat3>()[0].z);

			Wiwa::JSONValue vec2 = mat.AddMemberObject("vec2");
			vec2.AddMember("x", uniform.getData<glm::mat3>()[1].x);
			vec2.AddMember("y", uniform.getData<glm::mat3>()[1].y);
			vec2.AddMember("z", uniform.getData<glm::mat3>()[1].z);

			Wiwa::JSONValue vec3 = mat.AddMemberObject("vec3");
			vec3.AddMember("x", uniform.getData<glm::mat3>()[2].x);
			vec3.AddMember("y", uniform.getData<glm::mat3>()[2].y);
			vec3.AddMember("z", uniform.getData<glm::mat3>()[2].z);
		}break;
		case UniformType::Mat4:
		{
			Wiwa::JSONValue mat = uniforms.AddMemberObject(name);

			Wiwa::JSONValue vec1 = mat.AddMemberObject("vec1");
			vec1.AddMember("x", uniform.getData<glm::mat4>()[0].x);
			vec1.AddMember("y", uniform.getData<glm::mat4>()[0].y);
			vec1.AddMember("z", uniform.getData<glm::mat4>()[0].z);
			vec1.AddMember("w", uniform.getData<glm::mat4>()[0].w);

			Wiwa::JSONValue vec2 = mat.AddMemberObject("vec2");
			vec2.AddMember("x", uniform.getData<glm::mat4>()[1].x);
			vec2.AddMember("y", uniform.getData<glm::mat4>()[1].y);
			vec2.AddMember("z", uniform.getData<glm::mat4>()[1].z);
			vec2.AddMember("w", uniform.getData<glm::mat4>()[1].w);

			Wiwa::JSONValue vec3 = mat.AddMemberObject("vec3");
			vec3.AddMember("x", uniform.getData<glm::mat4>()[2].x);
			vec3.AddMember("y", uniform.getData<glm::mat4>()[2].y);
			vec3.AddMember("z", uniform.getData<glm::mat4>()[2].z);
			vec3.AddMember("w", uniform.getData<glm::mat4>()[2].w);

			Wiwa::JSONValue vec4 = mat.AddMemberObject("vec4");
			vec4.AddMember("x", uniform.getData<glm::mat4>()[2].x);
			vec4.AddMember("y", uniform.getData<glm::mat4>()[2].y);
			vec4.AddMember("z", uniform.getData<glm::mat4>()[2].z);
			vec4.AddMember("w", uniform.getData<glm::mat4>()[2].w);
		}break;
		case UniformType::Sampler2D:
		{
			Uniform::SamplerData* sdata = uniform.getPtrData<Uniform::SamplerData>();

			std::string string = sdata->tex_path;
			Resources::standarizePath(string);
			uniforms.AddMember(name, string.c_str());
		}break;
		default:
			break;
		}
	}
	doc.save_file(file.c_str());
	materialPath = file;
}
