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


// color over life ------------------------------------------------------------------------------------------
ColorOverLifeUpdate::ColorOverLifeUpdate(Emmiter* owner)
{
	type = COLOR_OVER_LIFE;
	this->owner = owner;

	finalColor = vec4(255, 255, 255, 0);

	affectR = false;
	affectG = false;
	affectB = false;
	affectA = true;
}

ColorOverLifeUpdate::ColorOverLifeUpdate(Emmiter* owner, ColorOverLifeUpdate* ref)
{
	type = COLOR_OVER_LIFE;
	this->owner = owner;

	finalColor = ref->finalColor;

	affectR = ref->affectR;
	affectG = ref->affectG;
	affectB = ref->affectB;
	affectA = ref->affectA;
}

void ColorOverLifeUpdate::Update(double dt, Particle* particle)
{
	if (affectR) particle->color.r = particle->initialColor.r * (1 - particle->lifetimeOverOne) + finalColor.r * particle->lifetimeOverOne;
	if (affectG) particle->color.g = particle->initialColor.g * (1 - particle->lifetimeOverOne) + finalColor.g * particle->lifetimeOverOne;
	if (affectB) particle->color.b = particle->initialColor.b * (1 - particle->lifetimeOverOne) + finalColor.b * particle->lifetimeOverOne;
	if (affectA) particle->color.a = particle->initialColor.a * (1 - particle->lifetimeOverOne) + finalColor.a * particle->lifetimeOverOne;

}

json ColorOverLifeUpdate::SaveModule()
{
	json moduleJSON;

	moduleJSON["Type"] = type;

	moduleJSON["FinalColor"] = { finalColor.r, finalColor.g, finalColor.b, finalColor.a };

	moduleJSON["AffectR"] = affectR;
	moduleJSON["AffectG"] = affectG;
	moduleJSON["AffectB"] = affectB;
	moduleJSON["AffectA"] = affectA;

	return moduleJSON;
}

void ColorOverLifeUpdate::LoadModule(const json& moduleJSON)
{
	if (moduleJSON.contains("Type"))
	{
		type = moduleJSON["Type"];
	}

	if (moduleJSON.contains("FinalColor"))
	{
		finalColor.r = moduleJSON["FinalColor"][0];
		finalColor.g = moduleJSON["FinalColor"][1];
		finalColor.b = moduleJSON["FinalColor"][2];
		finalColor.a = moduleJSON["FinalColor"][3];
	}

	if (moduleJSON.contains("AffectR"))
	{
		affectR = moduleJSON["AffectR"];
	}
	
	if (moduleJSON.contains("AffectG"))
	{
		affectG = moduleJSON["AffectG"];
	}
	
	if (moduleJSON.contains("AffectB"))
	{
		affectB = moduleJSON["AffectB"];
	}

	if (moduleJSON.contains("AffectA"))
	{
		affectA = moduleJSON["AffectA"];
	}
}

// scale over life -----------------------------------------------------------------------------
ScaleOverLifeUpdate::ScaleOverLifeUpdate(Emmiter* owner)
{
	type = SCALE_OVER_LIFE;
	this->owner = owner;

	finalScale = vec3(0, 0, 0);
}

ScaleOverLifeUpdate::ScaleOverLifeUpdate(Emmiter* owner, ScaleOverLifeUpdate* ref)
{
	type = SCALE_OVER_LIFE;
	this->owner = owner;

	finalScale = ref->finalScale;
}

void ScaleOverLifeUpdate::Update(double dt, Particle* particle)
{
	particle->scale.x = particle->initialScale.x * (1 - particle->lifetimeOverOne) + finalScale.x * particle->lifetimeOverOne;
	particle->scale.y = particle->initialScale.y * (1 - particle->lifetimeOverOne) + finalScale.y * particle->lifetimeOverOne;
	particle->scale.z = particle->initialScale.z * (1 - particle->lifetimeOverOne) + finalScale.z * particle->lifetimeOverOne;
}

json ScaleOverLifeUpdate::SaveModule()
{
	json moduleJSON;

	moduleJSON["Type"] = type;

	moduleJSON["FinalScale"] = { finalScale.x, finalScale.y, finalScale.z};

	return moduleJSON;
}

void ScaleOverLifeUpdate::LoadModule(const json& moduleJSON)
{
	if (moduleJSON.contains("Type"))
	{
		type = moduleJSON["Type"];
	}

	if (moduleJSON.contains("FinalScale"))
	{
		finalScale.x = moduleJSON["FinalScale"][0];
		finalScale.y = moduleJSON["FinalScale"][1];
		finalScale.z = moduleJSON["FinalScale"][2];
	}

}
