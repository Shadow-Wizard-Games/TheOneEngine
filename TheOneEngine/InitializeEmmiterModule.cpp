#include "InitializeEmmiterModule.h"

// set speed ---------------------------------------------------------------------------------------------------
SetSpeed::SetSpeed(Emmiter* owner)
{
	type = SET_SPEED;
	this->owner = owner;

	speed.usingSingleValue = true;
	speed.rangeValue.lowerLimit = vec3(-0.5, 1, -0.5);
	speed.rangeValue.upperLimit = vec3(0.5, 2, 0.5);
}

void SetSpeed::Initialize(Particle* particle)
{
	if (speed.usingSingleValue) {
		particle->speed = speed.singleValue;
	}
	else {
		vec3 randomVec = vec3{
			randomFloat(speed.rangeValue.lowerLimit.x, speed.rangeValue.upperLimit.x),
			randomFloat(speed.rangeValue.lowerLimit.y, speed.rangeValue.upperLimit.y),
			randomFloat(speed.rangeValue.lowerLimit.z, speed.rangeValue.upperLimit.z) };

		particle->speed = randomVec;
	}
}

json SetSpeed::SaveModule()
{
	json moduleJSON;

	moduleJSON["Type"] = type;

	moduleJSON["UsingSingleValueSpeed"] = speed.usingSingleValue;
	moduleJSON["MinSpeed"] = { speed.rangeValue.lowerLimit.x, speed.rangeValue.lowerLimit.y, speed.rangeValue.lowerLimit.z };
	moduleJSON["MaxSpeed"] = { speed.rangeValue.upperLimit.x, speed.rangeValue.upperLimit.y, speed.rangeValue.upperLimit.z };

	return moduleJSON;
}

void SetSpeed::LoadModule(const json& moduleJSON)
{
	if (moduleJSON.contains("Type"))
	{
		type = moduleJSON["Type"];
	}

	if (moduleJSON.contains("UsingSingleValueSpeed"))
	{
		speed.usingSingleValue = moduleJSON["UsingSingleValueSpeed"];
	}

	if (moduleJSON.contains("MinSpeed"))
	{
		speed.rangeValue.lowerLimit.x = moduleJSON["MinSpeed"][0];
		speed.rangeValue.lowerLimit.y = moduleJSON["MinSpeed"][1];
		speed.rangeValue.lowerLimit.z = moduleJSON["MinSpeed"][2];

	}

	if (moduleJSON.contains("MaxSpeed"))
	{
		speed.rangeValue.upperLimit.x = moduleJSON["MaxSpeed"][0];
		speed.rangeValue.upperLimit.y = moduleJSON["MaxSpeed"][1];
		speed.rangeValue.upperLimit.z = moduleJSON["MaxSpeed"][2];
	}
}

// set color ---------------------------------------------------------------------------------------------------
SetColor::SetColor(Emmiter* owner)
{
	type = SET_COLOR;
	this->owner = owner;

	color.usingSingleValue = true;
	color.rangeValue.lowerLimit = vec3(0, 0, 0);
	color.rangeValue.upperLimit = vec3(255, 255, 255);
}

void SetColor::Initialize(Particle* particle)
{
	if (color.usingSingleValue) {
		particle->color = color.singleValue;
	}
	else {
		vec3 randomVec = vec3{
			randomInt(color.rangeValue.lowerLimit.r, color.rangeValue.upperLimit.r),
			randomInt(color.rangeValue.lowerLimit.g, color.rangeValue.upperLimit.g),
			randomInt(color.rangeValue.lowerLimit.b, color.rangeValue.upperLimit.b) };

		particle->color = randomVec;
	}
}

json SetColor::SaveModule()
{
	json moduleJSON;

	moduleJSON["Type"] = type;

	moduleJSON["UsingSingleValueColor"] = color.usingSingleValue;
	moduleJSON["MinColor"] = { color.rangeValue.lowerLimit.x, color.rangeValue.lowerLimit.y, color.rangeValue.lowerLimit.z };
	moduleJSON["MaxColor"] = { color.rangeValue.upperLimit.x, color.rangeValue.upperLimit.y, color.rangeValue.upperLimit.z };

	return moduleJSON;
}

void SetColor::LoadModule(const json& moduleJSON)
{
	if (moduleJSON.contains("Type"))
	{
		type = moduleJSON["Type"];
	}

	if (moduleJSON.contains("UsingSingleValueColor"))
	{
		color.usingSingleValue = moduleJSON["UsingSingleValueColor"];
	}

	if (moduleJSON.contains("MinColor"))
	{
		color.rangeValue.lowerLimit.x = moduleJSON["MinColor"][0];
		color.rangeValue.lowerLimit.y = moduleJSON["MinColor"][1];
		color.rangeValue.lowerLimit.z = moduleJSON["MinColor"][2];

	}

	if (moduleJSON.contains("MaxColor"))
	{
		color.rangeValue.upperLimit.x = moduleJSON["MaxColor"][0];
		color.rangeValue.upperLimit.y = moduleJSON["MaxColor"][1];
		color.rangeValue.upperLimit.z = moduleJSON["MaxColor"][2];
	}
}

// set scale ---------------------------------------------------------------------------------------------------
SetScale::SetScale(Emmiter* owner)
{
	type = SET_SCALE;
	this->owner = owner;

	scale.usingSingleValue = true;
	scale.rangeValue.lowerLimit = vec3(1, 1, 1);
	scale.rangeValue.upperLimit = vec3(2, 2, 2);
}

void SetScale::Initialize(Particle* particle)
{
	if (scale.usingSingleValue) {
		particle->scale = scale.singleValue;
	}
	else {
		vec3 randomVec = vec3{
			randomFloat(scale.rangeValue.lowerLimit.x, scale.rangeValue.upperLimit.x),
			randomFloat(scale.rangeValue.lowerLimit.y, scale.rangeValue.upperLimit.y),
			randomFloat(scale.rangeValue.lowerLimit.z, scale.rangeValue.upperLimit.z) };

		particle->scale = randomVec;
	}
}

json SetScale::SaveModule()
{
	json moduleJSON;

	moduleJSON["Type"] = type;

	moduleJSON["UsingSingleValueScale"] = scale.usingSingleValue;
	moduleJSON["MinScale"] = { scale.rangeValue.lowerLimit.x, scale.rangeValue.lowerLimit.y, scale.rangeValue.lowerLimit.z };
	moduleJSON["MaxScale"] = { scale.rangeValue.upperLimit.x, scale.rangeValue.upperLimit.y, scale.rangeValue.upperLimit.z };

	return moduleJSON;
}

void SetScale::LoadModule(const json& moduleJSON)
{
	if (moduleJSON.contains("Type"))
	{
		type = moduleJSON["Type"];
	}

	if (moduleJSON.contains("UsingSingleValueScale"))
	{
		scale.usingSingleValue = moduleJSON["UsingSingleValueScale"];
	}

	if (moduleJSON.contains("MinScale"))
	{
		scale.rangeValue.lowerLimit.x = moduleJSON["MinScale"][0];
		scale.rangeValue.lowerLimit.y = moduleJSON["MinScale"][1];
		scale.rangeValue.lowerLimit.z = moduleJSON["MinScale"][2];

	}

	if (moduleJSON.contains("MaxScale"))
	{
		scale.rangeValue.upperLimit.x = moduleJSON["MaxScale"][0];
		scale.rangeValue.upperLimit.y = moduleJSON["MaxScale"][1];
		scale.rangeValue.upperLimit.z = moduleJSON["MaxScale"][2];
	}
}
