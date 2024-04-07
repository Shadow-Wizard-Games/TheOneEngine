#include "ParticleSystem.h"
#include "GameObject.h"
#include "SingleOrRandom.h"
#include "EngineCore.h"
#include <ostream>
#include <istream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

ParticleSystem::ParticleSystem(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::ParticleSystem)
{
	this->transform = containerGO.get()->GetComponent<Transform>();

	if (transform == nullptr)
	{
		LOG(LogType::LOG_ERROR, "GameObject Container invalid!");
	}

	isON = true;
	AddEmmiter();
}

ParticleSystem::ParticleSystem(std::shared_ptr<GameObject> containerGO, ParticleSystem* ref) : Component(containerGO, ComponentType::ParticleSystem),
isON(ref->isON)
{
	this->transform = containerGO.get()->GetComponent<Transform>();

	if (transform == nullptr)
	{
		LOG(LogType::LOG_ERROR, "GameObject Container invalid!");
	}

	for (const auto& emmiter : ref->emmiters) {

	}

	isON = true;
	AddEmmiter();
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Update()
{
	if (isON) {
		for (auto i = emmiters.begin(); i != emmiters.end(); ++i) {
			(*i)->Update(engine->dt);
		}
	}
}

void ParticleSystem::DrawComponent(Camera* camera)
{
	for (auto i = emmiters.begin(); i != emmiters.end(); ++i) {
		(*i)->Render(camera);
	}
}

void ParticleSystem::Play()
{
	isON = true;
}

void ParticleSystem::Stop()
{
	isON = false;
}

void ParticleSystem::Replay()
{
	Play();
	for (auto i = emmiters.begin(); i != emmiters.end(); ++i) {
		(*i)->Start();
	}
}

void ParticleSystem::ClearEmmiters()
{
	emmiters.clear();
}

Emmiter* ParticleSystem::AddEmmiter()
{
	auto e = std::make_unique<Emmiter>(this);
	emmiters.push_back(std::move(e));
	return emmiters[emmiters.size() - 1].get();
}

Emmiter* ParticleSystem::AddEmmiter(Emmiter* ref)
{
	auto e = std::make_unique<Emmiter>(this, ref);
	emmiters.push_back(std::move(e));
	return emmiters[emmiters.size() - 1].get();
}

json ParticleSystem::SaveComponent()
{
	json particleSystemJSON;

	if (!emmiters.empty())
	{
		json emmitersJSON;

		for (const auto& emmiter : emmiters)
		{
			emmitersJSON.push_back(emmiter.get()->SaveEmmiter());
		}
		particleSystemJSON["Emmiters"] = emmitersJSON;
	}

	particleSystemJSON["UID"] = UID;
	particleSystemJSON["Name"] = name;
	particleSystemJSON["Type"] = type;
	particleSystemJSON["IsON"] = isON;

	// save copy of the json as the ps individually


	return particleSystemJSON;
}

void ParticleSystem::LoadComponent(const json& transformJSON)
{
	// Load basic properties
	if (transformJSON.contains("UID"))
	{
		UID = transformJSON["UID"];
	}

	if (transformJSON.contains("Name"))
	{
		name = transformJSON["Name"];
	}

	if (transformJSON.contains("Type"))
	{
		type = transformJSON["Type"];
	}

	if (transformJSON.contains("IsON"))
	{
		isON = transformJSON["IsON"];
	}

	ClearEmmiters();

	// load emmiters
	if (transformJSON.contains("Emmiters"))
	{
		const json& emmitersJSON = transformJSON["Emmiters"];

		for (const auto& emmiterJSON : emmitersJSON)
		{
			auto e = AddEmmiter();
			e->ClearModules();
			e->LoadEmmiter(emmiterJSON);
		}
	}

}

void ParticleSystem::ExportParticles()
{
	//Change to save the Scene Class
	name.shrink_to_fit();
	std::string fileNameExt = name;

	std::string finalName;

	for (int i = 0; fileNameExt.at(i) != '\0'; ++i) {
		finalName += fileNameExt.at(i);
	}

	name = finalName;

	finalName.append(".particles");

	fs::path filename = fs::path(ASSETS_PATH) / "Particles" / finalName;
	//string filename = "Assets/Scenes/";
	fs::path folderName = fs::path(ASSETS_PATH) / "Particles";
	fs::create_directories(folderName);

	json particlesJSON;

	particlesJSON = SaveComponent();

	std::ofstream(filename) << particlesJSON.dump(2);
	LOG(LogType::LOG_OK, "SAVE SUCCESFUL");
}

bool ParticleSystem::IsON()
{
	return isON;
}

Transform* ParticleSystem::GetTransform()
{
	return transform;
}