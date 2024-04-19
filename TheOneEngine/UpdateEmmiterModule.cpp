#include "UpdateEmmiterModule.h"

AccelerationUpdate::AccelerationUpdate(Emmiter* owner)
{
	type = ACCELERATION;
	this->owner = owner;

	acceleration = vec3(0, -1, 0);
}

AccelerationUpdate::AccelerationUpdate(Emmiter* owner, AccelerationUpdate* ref)
{
	type = ACCELERATION;
	this->owner = owner;

	acceleration = ref->acceleration;
}

void AccelerationUpdate::Update(double dt, Particle* particle)
{
	vec3 vectorSpeed = (double)particle->speed * particle->direction;

	vectorSpeed = vectorSpeed + (acceleration * dt);

	particle->direction = glm::normalize(vectorSpeed);
	particle->speed = glm::length(vectorSpeed);
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
