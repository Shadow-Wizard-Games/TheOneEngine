#include "Emmiter.h"
#include <map>
#include <GL/glew.h>

#include "ParticleSystem.h"

Emmiter::Emmiter(ParticleSystem* owner)
{
	this->owner = owner;

	delay = 0;
	maxParticles = 100;
	duration = 10;
	lifetime = 0;
	isLooping = true;
	isON = true;

	spawnModule = std::make_unique<ConstantSpawnRate>(this);

	renderModule = std::make_unique<BillboardRender>(this);

	auto setSpeed = std::make_unique<SetSpeed>(this);
	setSpeed->speed.usingSingleValue = false;
	setSpeed->speed.rangeValue.lowerLimit = vec3{ -0.5, 1, -0.5 };
	setSpeed->speed.rangeValue.upperLimit = vec3{ 0.5, 2, 0.5 };


	initializeModules.push_back(std::move(setSpeed));

	RestartParticlePool();
}

Emmiter::Emmiter(ParticleSystem* owner, Emmiter* ref)
{
	this->owner = owner;

	delay = ref->delay;
	maxParticles = ref->maxParticles;
	duration = ref->duration;
	lifetime = ref->lifetime;
	isLooping = ref->isLooping;
	isON = ref->isON;

	// TO-DO
	spawnModule = std::make_unique<ConstantSpawnRate>(this);

	renderModule = std::make_unique<BillboardRender>(this);

	auto setSpeed = std::make_unique<SetSpeed>(this);
	setSpeed->speed.usingSingleValue = false;
	setSpeed->speed.rangeValue.lowerLimit = vec3{ -0.5, 1, -0.5 };
	setSpeed->speed.rangeValue.upperLimit = vec3{ 0.5, 2, 0.5 };


	initializeModules.push_back(std::move(setSpeed));

	RestartParticlePool();
}

Emmiter::~Emmiter()
{
}

void Emmiter::Start()
{
	lifetime = 0;
	while (!usingParticlesIDs.empty()) {
		freeParticlesIDs.push(usingParticlesIDs[usingParticlesIDs.size() - 1]);
		usingParticlesIDs.pop_back();
	}

	if (spawnModule) {
		spawnModule->Reset();
	}

	RestartParticlePool();
}

void Emmiter::Update(double dt)
{
	if (!isON) return;

	lifetime += dt;

	if (lifetime < delay) return;

	if (lifetime < duration + delay || isLooping) {
		if (spawnModule) {
			spawnModule->Update(dt);
		}
	}

	for (auto i = updateModules.begin(); i != updateModules.end(); ++i) {
		for (auto j = usingParticlesIDs.begin(); j != usingParticlesIDs.end(); ++j) {
			(*i)->Update(dt, particles[*j].get());
		}
	}

	std::vector<std::vector<int>::iterator> particlesToFree;

	for (auto i = usingParticlesIDs.begin(); i != usingParticlesIDs.end(); ++i) {
		particles[*i]->Update(dt);
		if (particles[*i]->lifetime > particles[*i]->duration) {
			particlesToFree.push_back(i);
		}
	}

	for (auto i = particlesToFree.rbegin(); i != particlesToFree.rend(); ++i) {
		freeParticlesIDs.push(*(*i));
		usingParticlesIDs.erase(*i);

	}
}

void Emmiter::Render(Camera* camera)
{
	//// sort particles by distance to the camera
	//std::map<float, Particle*> particlesToRender;
	//float modelview[16];
	//glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

	//for () {
	//	particlesToRender.insert
	//}

	std::vector<Particle*> particlesToRender;
	for (auto i = usingParticlesIDs.begin(); i != usingParticlesIDs.end(); ++i) {
		particlesToRender.push_back(particles[(*i)].get());
	}

	if (renderModule) {
		for (auto i = particlesToRender.begin(); i != particlesToRender.end(); ++i) {
			renderModule->Update(*i, camera);
		}
	}
}

void Emmiter::SpawnParticles(int amount)
{
	for (int i = 0; i < amount; ++i) {
		if (!freeParticlesIDs.empty()) {
			int spawnedParticleID = freeParticlesIDs.front();
			usingParticlesIDs.push_back(spawnedParticleID);
			freeParticlesIDs.pop();
			InitializeParticle(particles[spawnedParticleID].get());
		}
	}
}

void Emmiter::RestartParticlePool()
{
	usingParticlesIDs.clear();

	while (!freeParticlesIDs.empty()) {
		freeParticlesIDs.pop();
	}

	for (int i = 0; i < maxParticles; ++i) {
		freeParticlesIDs.push(i);
	}

	particles.clear();

	for (int i = 0; i < maxParticles; ++i) {
		particles.push_back(std::move(std::make_unique<Particle>()));
	}

	particles.shrink_to_fit();
}

void Emmiter::InitializeParticle(Particle* particle)
{
	if (spawnModule) {
		particle->duration = spawnModule->duration;
	}

	particle->lifetime = 0;
	particle->position = vec3{ 0, 0, 0 };
	particle->color = vec3{ 0, 0, 0 };

	for (auto i = initializeModules.begin(); i != initializeModules.end(); ++i) {
		(*i)->Initialize(particle);
	}
}

void Emmiter::ClearModules()
{
	spawnModule.reset();
	initializeModules.clear();
	updateModules.clear();
	renderModule.reset();
}

SpawnEmmiterModule* Emmiter::AddModule(SpawnEmmiterModule::SpawnEmmiterModuleType type)
{
	SpawnEmmiterModule* newModule = nullptr;
	switch (type)
	{
	case SpawnEmmiterModule::CONSTANT:
		spawnModule = std::make_unique<ConstantSpawnRate>(this);
		newModule = spawnModule.get();
		break;
	case SpawnEmmiterModule::SINGLE_BURST:
		spawnModule = std::make_unique<SingleBurstSpawn>(this);
		newModule = spawnModule.get();
		break;
	case SpawnEmmiterModule::CONSTANT_BURST:
		spawnModule = std::make_unique<ConstantBurstSpawn>(this);
		newModule = spawnModule.get();
		break;
	default:
		break;
	}
	return newModule;
}

InitializeEmmiterModule* Emmiter::AddModule(InitializeEmmiterModule::InitializeEmmiterModuleType type)
{
	InitializeEmmiterModule* newModule = nullptr;
	switch (type)
	{
	case InitializeEmmiterModule::SET_SPEED:
		initializeModules.push_back(std::move(std::make_unique<SetSpeed>(this)));
		newModule = initializeModules[initializeModules.size() - 1].get();
		break;
	case InitializeEmmiterModule::SET_COLOR:
		initializeModules.push_back(std::move(std::make_unique<SetColor>(this)));
		newModule = initializeModules[initializeModules.size() - 1].get();
		break;
	case InitializeEmmiterModule::SET_SCALE:
		initializeModules.push_back(std::move(std::make_unique<SetScale>(this)));
		newModule = initializeModules[initializeModules.size() - 1].get();
		break;
	default:
		break;
	}
	return newModule;
}

UpdateEmmiterModule* Emmiter::AddModule(UpdateEmmiterModule::UpdateEmmiterModuleType type)
{
	UpdateEmmiterModule* newModule = nullptr;
	switch (type)
	{
	case UpdateEmmiterModule::ACCELERATION:
		updateModules.push_back(std::move(std::make_unique<AccelerationUpdate>(this)));
		newModule = updateModules[updateModules.size() - 1].get();
		break;
	default:
		break;
	}
	return newModule;
}

RenderEmmiterModule* Emmiter::AddModule(RenderEmmiterModule::RenderEmmiterModuleType type)
{
	RenderEmmiterModule* newModule = nullptr;
	switch (type)
	{
	case RenderEmmiterModule::BILLBOARD:
		renderModule = std::make_unique<BillboardRender>(this);
		newModule = renderModule.get();
		break;
	default:
		break;
	}
	return newModule;
}

json Emmiter::SaveEmmiter()
{
	json emmiterJSON;

	if (spawnModule != nullptr)
	{
		json spawnJSON;

		spawnJSON.push_back(spawnModule->SaveModule());

		emmiterJSON["SpawnModule"] = spawnJSON;
	}

	if (!initializeModules.empty())
	{
		json initializeJSON;

		for (const auto& initModule : initializeModules)
		{
			initializeJSON.push_back(initModule->SaveModule());
		}

		emmiterJSON["InitializeModules"] = initializeJSON;
	}

	if (!updateModules.empty())
	{
		json updateJSON;

		for (const auto& updateModule : updateModules)
		{
			updateJSON.push_back(updateModule->SaveModule());
		}

		emmiterJSON["UpdateModules"] = updateJSON;
	}

	if (renderModule != nullptr)
	{
		json renderJSON;

		renderJSON.push_back(renderModule->SaveModule());

		emmiterJSON["RenderModule"] = renderJSON;
	}

	emmiterJSON["IsON"] = isON;
	emmiterJSON["MaxParticles"] = maxParticles;
	emmiterJSON["Duration"] = duration;
	emmiterJSON["Lifetime"] = lifetime;
	emmiterJSON["Delay"] = delay;
	emmiterJSON["IsLooping"] = isLooping;

	return emmiterJSON;
}

void Emmiter::LoadEmmiter(const json& emmiterJSON)
{
	// Load basic properties
	if (emmiterJSON.contains("IsON"))
	{
		isON = emmiterJSON["IsON"];
	}

	if (emmiterJSON.contains("MaxParticles"))
	{
		maxParticles = emmiterJSON["MaxParticles"];
	}

	if (emmiterJSON.contains("Duration"))
	{
		duration = emmiterJSON["Duration"];
	}

	if (emmiterJSON.contains("Lifetime"))
	{
		lifetime = emmiterJSON["Lifetime"];
	}

	if (emmiterJSON.contains("Delay"))
	{
		delay = emmiterJSON["Delay"];
	}

	if (emmiterJSON.contains("IsLooping"))
	{
		isLooping = emmiterJSON["IsLooping"];
	}

	if (emmiterJSON.contains("SpawnModule"))
	{
		auto spawnM = AddModule((SpawnEmmiterModule::SpawnEmmiterModuleType)emmiterJSON["SpawnModule"][0]["Type"]);
		spawnM->LoadModule(emmiterJSON["SpawnModule"][0]);
	}

	if (emmiterJSON.contains("InitializeModules"))
	{
		const json& initializeJSON = emmiterJSON["InitializeModules"];

		for (const auto& initJSON : initializeJSON)
		{
			auto initM = AddModule((InitializeEmmiterModule::InitializeEmmiterModuleType)initJSON["Type"]);
			initM->LoadModule(initJSON);
		}
		
	}

	if (emmiterJSON.contains("UpdateModules"))
	{
		const json& updateJSON = emmiterJSON["UpdateModules"];

		for (const auto& uJSON : updateJSON)
		{
			auto updateM = AddModule((UpdateEmmiterModule::UpdateEmmiterModuleType)uJSON["Type"]);
			updateM->LoadModule(uJSON);
		}
	}

	if (emmiterJSON.contains("RenderModule"))
	{
		auto spawnM = AddModule((RenderEmmiterModule::RenderEmmiterModuleType)emmiterJSON["RenderModule"][0]["Type"]);
		spawnM->LoadModule(emmiterJSON["RenderModule"][0]);
	}
}
