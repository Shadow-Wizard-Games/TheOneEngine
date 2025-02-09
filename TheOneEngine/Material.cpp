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
	{
		size_t id = Resources::Load<Shader>("Assets/Shaders/MeshTexture");
		setShader(Resources::GetResourceById<Shader>(id), "Assets/Shaders/MeshTexture");
	}
	else
		setShader(shader, shader->getPath());

}

Material::Material(const std::string& file)
{
	Load(file);
}

Material::~Material()
{
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

void Material::setShader(Shader* sh, const std::string& path)
{
	uniforms.clear();

	shader = sh;
	shaderPath = path;
	Refresh();
}

Uniform* Material::getUniform(const std::string& name)
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

	shader = new Shader("Assets/Shaders/basicShader");
	shader->addUniform("tex", UniformType::Sampler2D);

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
			GLCALL(glActiveTexture(texureId));
			GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
			texureId++;
		}
	}

	shader->UnBind();
}

void Material::Load(const std::string& path)
{
	json matJSON = Resources::OpenJSON(path);

	if (!matJSON.contains("shader"))
	{
		LOG(LogType::LOG_ERROR, "Shader path from material unexistent");
		return;
	}

	shaderPath = matJSON["shader"];

	size_t shaderId = Resources::LoadFromLibrary<Shader>(shaderPath.c_str());
	Shader* shader = Resources::GetResourceById<Shader>(shaderId);


	setShader(shader, shaderPath.c_str());

	if (matJSON.contains("uniforms"))
	{
		json uniformsJSON = matJSON["uniforms"];
		for (const auto& u : uniformsJSON)
		{
			if (!u.contains("Name"))
				return;

			Uniform* uniform = getUniform(u["Name"]);

			if (!uniform)
				return;

			const char* name = uniform->name.c_str();
			switch (uniform->getType())
			{
			case UniformType::Bool:
				uniform->setData((bool)u["Value"], uniform->getType());
				break;
			case UniformType::Int:
				uniform->setData((int)u["Value"], uniform->getType());
				break;
			case UniformType::Uint:
				uniform->setData((unsigned int)u["Value"], uniform->getType());
				break;
			case UniformType::Float:
				uniform->setData((float)u["Value"], uniform->getType());
				break;
			case UniformType::fVec2:
			{
				json Vec2;
				if (u.contains("Vec2"))
					Vec2 = u["Vec2"];

				glm::vec2 data;
				data.x = (float)Vec2["x"];
				data.y = (float)Vec2["y"];
				uniform->setData(data, uniform->getType());
			}break;
			case UniformType::fVec3:
			{
				json Vec3;
				if (u.contains("Vec3"))
					Vec3 = u["Vec3"];

				glm::vec3 data;
				data.x = (float)Vec3["x"];
				data.y = (float)Vec3["y"];
				data.z = (float)Vec3["z"];
				uniform->setData(data, uniform->getType());
			}break;
			case UniformType::fVec4:
			{
				json Vec4;
				if (u.contains("Vec4"))
					Vec4 = u["Vec4"];

				glm::vec4 data;
				data.x = (float)Vec4["x"];
				data.y = (float)Vec4["y"];
				data.z = (float)Vec4["z"];
				data.w = (float)Vec4["w"];
				uniform->setData(data, uniform->getType());
			}break;
			case UniformType::Mat2:
			{
				glm::mat2 data;
				int it = 0;
				for (int i = 0; i < 2; i++) {
					for (int j = 0; j < 2; j++) {
						data[i][j] = u["Mat2"][it];
						it++;
					}
				}
				uniform->setData(data, uniform->getType());
			}break;
			case UniformType::Mat3:
			{
				glm::mat3 data;
				int it = 0;
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						data[i][j] = u["Mat3"][it];
						it++;
					}
				}
				uniform->setData(data, uniform->getType());
			}break;
			case UniformType::Mat4:
			{
				glm::mat4 data;
				int it = 0;
				for (int i = 0; i < 4; i++) {
					for (int j = 0; j < 4; j++) {
						data[i][j] = u["Mat4"][it];
						it++;
					}
				}
				uniform->setData(data, uniform->getType());
			}break;
			case UniformType::Sampler2D:
			{
				Uniform::SamplerData sdata;
				std::string texPath = u["Value"];
				memcpy(sdata.tex_path, &texPath[0], texPath.size() + 1);
				Resources::Import<Texture>(texPath);
				if (sdata.tex_path != "") {
					sdata.resource_id = Resources::LoadFromLibrary<Texture>(sdata.tex_path);

					Texture* img = Resources::GetResourceById<Texture>(sdata.resource_id);

					if (img)
					{
						sdata.tex_id = img->GetTextureId();

						uniform->setData(sdata, uniform->getType());
					}
				}
				else
				{
					sdata.tex_id = -1;
					sdata.resource_id = -1;
					uniform->setData(sdata, uniform->getType());
				}
			}break;
			default:
				break;
			}
		}
	}

	materialPath = path;
}

void Material::Save(const std::string& path)
{
	materialPath = path;

	json matJSON;
	Resources::StandarizePath(shaderPath);
	matJSON["shader"] = shaderPath.c_str();


	json uniformsJSON;
	for (Uniform uniform : uniforms)
	{
		json uniformJSON;
		const char* name = uniform.name.c_str();
		uniformJSON["Name"] = name;
		switch (uniform.getType())
		{
		case UniformType::Bool:
			uniformJSON["Value"] = uniform.getData<bool>();
			break;
		case UniformType::Int:
			uniformJSON["Value"] = uniform.getData<int>();
			break;
		case UniformType::Uint:
			uniformJSON["Value"] = uniform.getData<unsigned int>();
			break;
		case UniformType::Float:
			uniformJSON["Value"] = uniform.getData<float>();
			break;
		case UniformType::fVec2:
		{
			json vecJSON;
			vecJSON["x"] = uniform.getData<glm::vec2>().x;
			vecJSON["y"] = uniform.getData<glm::vec2>().y;
			uniformJSON["Vec2"] = vecJSON;
		}break;
		case UniformType::fVec3:
		{
			json vecJSON;
			vecJSON["x"] = uniform.getData<glm::vec3>().x;
			vecJSON["y"] = uniform.getData<glm::vec3>().y;
			vecJSON["z"] = uniform.getData<glm::vec3>().z;
			uniformJSON["Vec3"] = vecJSON;
		}break;
		case UniformType::fVec4:
		{
			json vecJSON;
			vecJSON["x"] = uniform.getData<glm::vec4>().x;
			vecJSON["y"] = uniform.getData<glm::vec4>().y;
			vecJSON["z"] = uniform.getData<glm::vec4>().z;
			vecJSON["w"] = uniform.getData<glm::vec4>().w;
			uniformJSON["Vec4"] = vecJSON;
		}break;
		case UniformType::Mat2:
		{
			glm::mat2 Mat2 = uniform.getData<glm::mat2>();

			uniformJSON["Mat2"] = { Mat2[0][0], Mat2[0][1],
									Mat2[1][0], Mat2[1][1] };
		}break;
		case UniformType::Mat3:
		{
			glm::mat3 Mat3 = uniform.getData<glm::mat3>();

			uniformJSON["Mat3"] = { Mat3[0][0], Mat3[0][1], Mat3[0][2],
									Mat3[1][0], Mat3[1][1], Mat3[1][2],
									Mat3[2][0], Mat3[2][1], Mat3[2][2]};
		}break;
		case UniformType::Mat4:
		{
			glm::mat4 Mat4 = uniform.getData<glm::mat4>();

			uniformJSON["Mat4"] = { Mat4[0][0], Mat4[0][1], Mat4[0][2], Mat4[0][3],
									Mat4[1][0], Mat4[1][1], Mat4[1][2], Mat4[1][3],
									Mat4[2][0], Mat4[2][1], Mat4[2][2], Mat4[2][3],
									Mat4[3][0], Mat4[3][1], Mat4[3][2], Mat4[3][3] };
		}break;
		case UniformType::Sampler2D:
		{
			Uniform::SamplerData* sdata = uniform.getPtrData<Uniform::SamplerData>();

			std::string string = sdata->tex_path;
			Resources::StandarizePath(string);
			uniformJSON["Value"] = string.c_str();
		}break;
		default:
			break;
		}

		uniformsJSON.push_back(uniformJSON);
	}
	matJSON["uniforms"] = uniformsJSON;

	Resources::SaveJSON(materialPath, matJSON);
}
