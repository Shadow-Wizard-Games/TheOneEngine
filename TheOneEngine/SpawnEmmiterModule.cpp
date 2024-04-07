#include "SpawnEmmiterModule.h"
#include "Emmiter.h"

ConstantSpawnRate::ConstantSpawnRate(Emmiter* owner)
{
	type = CONSTANT;
	this->owner = owner;
	duration = 10;
	spawnRate = 0.5f;
	timeFromLastSpawn = 0;
}

void ConstantSpawnRate::Update(double dt)
{
	if (spawnRate <= 0.001) return;

	timeFromLastSpawn += dt;

	if (timeFromLastSpawn >= 1 / spawnRate) {
		int ticks = 0;

		while (timeFromLastSpawn >= 1 / spawnRate) {
			timeFromLastSpawn -= 1 / spawnRate;
			ticks++;
		}

		owner->SpawnParticles(ticks);
	}
}

json ConstantSpawnRate::SaveModule()
{
	json moduleJSON;

	moduleJSON["Type"] = type;

	moduleJSON["Duration"] = duration;

	moduleJSON["SpawnRate"] = spawnRate;

	return moduleJSON;
}

void ConstantSpawnRate::LoadModule(const json& moduleJSON)
{
	if (moduleJSON.contains("Type"))
	{
		type = moduleJSON["Type"];
	}

	if (moduleJSON.contains("Duration"))
	{
		duration = moduleJSON["Duration"];
	}

	if (moduleJSON.contains("SpawnRate"))
	{
		spawnRate = moduleJSON["SpawnRate"];
	}
}

SingleBurstSpawn::SingleBurstSpawn(Emmiter* owner)
{
	type = SINGLE_BURST;
	this->owner = owner;
	duration = 10;
	amount = 10;
	activated = false;
}

void SingleBurstSpawn::Reset()
{
	activated = false;
}

void SingleBurstSpawn::Update(double dt)
{
	if (activated) return;
	owner->SpawnParticles(amount);

	activated = true;
}

json SingleBurstSpawn::SaveModule()
{
	json moduleJSON;

	moduleJSON["Type"] = type;

	moduleJSON["Duration"] = duration;

	moduleJSON["Amount"] = amount;

	return moduleJSON;
}

void SingleBurstSpawn::LoadModule(const json& moduleJSON)
{
	if (moduleJSON.contains("Type"))
	{
		type = moduleJSON["Type"];
	}

	if (moduleJSON.contains("Duration"))
	{
		duration = moduleJSON["Duration"];
	}

	if (moduleJSON.contains("Amount"))
	{
		amount = moduleJSON["Amount"];
	}
}

ConstantBurstSpawn::ConstantBurstSpawn(Emmiter* owner)
{
	type = CONSTANT_BURST;
	this->owner = owner;
	duration = 10;
	spawnRate = 0.5f;
	timeFromLastSpawn = 0;
	amount = 10;
}

void ConstantBurstSpawn::Update(double dt)
{
	if (spawnRate <= 0.001) return;

	timeFromLastSpawn += dt;

	if (timeFromLastSpawn >= 1 / spawnRate) {
		int ticks = 0;

		while (timeFromLastSpawn >= 1 / spawnRate) {
			timeFromLastSpawn -= 1 / spawnRate;
			ticks++;
		}

		owner->SpawnParticles(ticks * amount);
	}
}

json ConstantBurstSpawn::SaveModule()
{
	json moduleJSON;

	moduleJSON["Type"] = type;

	moduleJSON["Duration"] = duration;

	moduleJSON["SpawnRate"] = spawnRate;
	moduleJSON["Amount"] = amount;

	return moduleJSON;
}

void ConstantBurstSpawn::LoadModule(const json& moduleJSON)
{
	if (moduleJSON.contains("Type"))
	{
		type = moduleJSON["Type"];
	}

	if (moduleJSON.contains("Duration"))
	{
		duration = moduleJSON["Duration"];
	}

	if (moduleJSON.contains("SpawnRate"))
	{
		spawnRate = moduleJSON["SpawnRate"];
	}

	if (moduleJSON.contains("Amount"))
	{
		amount = moduleJSON["Amount"];
	}
}
