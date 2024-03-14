#include "Material.h"
#include "Shader.h"
#include "Uniform.h"

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
			glActiveTexture(texureId));
			glBindTexture(GL_TEXTURE_2D, 0));
			texureId++;
		}
	}

	shader->UnBind();
}
