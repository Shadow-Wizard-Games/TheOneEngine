#pragma once
#include "Particle.h"
#include "SingleOrRandom.h"
#include "Defs.h"
#include <vector>


class Emmiter;

class SpawnEmmiterModule {
public:
	virtual void Reset() {};

	virtual void Update(double dt) {};

	//ConstantSpawnRate & ConstantBurstSpawn
	virtual const float getSpawnRate() { return 0; };
	//SingleBurstSpawn & ConstantBurstSpawn
	virtual const float getAmount() { return 0; };

	virtual json SaveModule() { return json(); };

	virtual void LoadModule(const json& moduleJSON) { };

	enum SpawnEmmiterModuleType {
		CONSTANT,
		SINGLE_BURST,
		CONSTANT_BURST
	};

	SpawnEmmiterModuleType type;

	float duration;

protected:
	Emmiter* owner;
};

// modules
class ConstantSpawnRate : public SpawnEmmiterModule {
public:
	ConstantSpawnRate(Emmiter* owner);

	float spawnRate;
	float timeFromLastSpawn;

	const float getSpawnRate() override {
		return spawnRate;
	};

	void Update(double dt) override;

	json SaveModule();

	void LoadModule(const json& moduleJSON);

};

class SingleBurstSpawn : public SpawnEmmiterModule {
public:
	SingleBurstSpawn(Emmiter* owner);

	float amount;

	bool activated;

	const float getAmount() override {
		return amount;
	};

	void Reset() override;

	void Update(double dt) override;

	json SaveModule();

	void LoadModule(const json& moduleJSON);

};

class ConstantBurstSpawn : public SpawnEmmiterModule {
public:
	ConstantBurstSpawn(Emmiter* owner);

	float spawnRate;
	float timeFromLastSpawn;

	float amount;

	const float getSpawnRate() override {
		return spawnRate;
	};
	const float getAmount() override {
		return amount;
	};

	void Update(double dt) override;

	json SaveModule();

	void LoadModule(const json& moduleJSON);

};