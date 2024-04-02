#include "UpdateEmmiterModule.h"

AccelerationUpdate::AccelerationUpdate(Emmiter* owner)
{
	type = ACCELERATION;
	this->owner = owner;
	acceleration = vec3(0, -1, 0);
}

void AccelerationUpdate::Update(double dt, Particle* particle)
{
	particle->speed = particle->speed + (acceleration * dt);
}

json AccelerationUpdate::SaveModule()
{
	json moduleJSON;

	moduleJSON["Type"] = type;

	moduleJSON["Acceleration"] = { acceleration.x, acceleration.y, acceleration.z };

	return moduleJSON;
}

void AccelerationUpdate::LoadModule(const json& moduleJSON)
{
	if (moduleJSON.contains("Type"))
	{
		type = moduleJSON["Type"];
	}

	if (moduleJSON.contains("Acceleration"))
	{
		acceleration.x = moduleJSON["Acceleration"][0];
		acceleration.y = moduleJSON["Acceleration"][1];
		acceleration.z = moduleJSON["Acceleration"][2];
	}
}
