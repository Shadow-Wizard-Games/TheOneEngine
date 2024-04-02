#pragma once
#include "SpawnEmmiterModule.h"
#include "InitializeEmmiterModule.h"
#include "UpdateEmmiterModule.h"
#include "RenderEmmiterModule.h"

#include "Particle.h"

#include "Camera.h"

#include <vector>
#include <queue>
#include <array>
#include <memory>

class ParticleSystem;

class Emmiter {
public:
	Emmiter(ParticleSystem* owner);
	Emmiter(ParticleSystem* owner, Emmiter* ref);
	~Emmiter();

	void Start();
	void Update(double dt);
	void Render(Camera* camera);

	void SpawnParticles(int amount);

	// function for when we change the maxParticles or when we start
	void RestartParticlePool();

	void InitializeParticle(Particle* particle);

	void ClearModules();

	SpawnEmmiterModule* AddModule(SpawnEmmiterModule::SpawnEmmiterModuleType type);
	InitializeEmmiterModule* AddModule(InitializeEmmiterModule::InitializeEmmiterModuleType type);
	UpdateEmmiterModule* AddModule(UpdateEmmiterModule::UpdateEmmiterModuleType type);
	RenderEmmiterModule* AddModule(RenderEmmiterModule::RenderEmmiterModuleType type);

	json SaveEmmiter();
	void LoadEmmiter(const json& emmiterJSON);

public:
	bool isON;

	int maxParticles;

	float duration;
	float lifetime;

	float delay;

	bool isLooping;

	// specific behaviors
	std::unique_ptr<SpawnEmmiterModule> spawnModule;

	std::vector<std::unique_ptr<InitializeEmmiterModule>> initializeModules;

	std::vector<std::unique_ptr<UpdateEmmiterModule>> updateModules;

	std::unique_ptr<RenderEmmiterModule> renderModule;

	ParticleSystem* owner;

private:
	std::vector<std::unique_ptr<Particle>> particles;
	std::vector<int> usingParticlesIDs;
	std::queue<int> freeParticlesIDs;

};