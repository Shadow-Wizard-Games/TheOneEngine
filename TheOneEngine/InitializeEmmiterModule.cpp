#include "InitializeEmmiterModule.h"

// set speed ---------------------------------------------------------------------------------------------------
SetSpeed::SetSpeed(Emmiter* owner)
{
	type = SET_SPEED;
	this->owner = owner;

	speed.usingSingleValue = false;
	speed.rangeValue.lowerLimit = 1;
	speed.rangeValue.upperLimit = 2;
}

SetSpeed::SetSpeed(Emmiter* owner, SetSpeed* ref)
{
	type = SET_SPEED;
	this->owner = owner;

	speed = ref->speed;
}

void SetSpeed::Initialize(Particle* particle)
{
	if (speed.usingSingleValue) {
		particle->speed = speed.singleValue;
	}
	else {
		particle->speed = randomFloat(speed.rangeValue.lowerLimit, speed.rangeValue.upperLimit);
	}
}

json SetSpeed::SaveModule()
{
	json moduleJSON;

	moduleJSON["Type"] = type;

	moduleJSON["UsingSingleValueSpeed"] = speed.usingSingleValue;
	moduleJSON["MinSpeed"] = speed.rangeValue.lowerLimit;
	moduleJSON["MaxSpeed"] = speed.rangeValue.upperLimit;

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
		speed.rangeValue.lowerLimit = moduleJSON["MinSpeed"];
	}

	if (moduleJSON.contains("MaxSpeed"))
	{
		speed.rangeValue.upperLimit = moduleJSON["MaxSpeed"];
	}
}

// set color ---------------------------------------------------------------------------------------------------
SetColor::SetColor(Emmiter* owner)
{
	type = SET_COLOR;
	this->owner = owner;

	color.usingSingleValue = true;
	color.rangeValue.lowerLimit = vec4(0, 0, 0, 255);
	color.rangeValue.upperLimit = vec4(255, 255, 255, 0);
}

SetColor::SetColor(Emmiter* owner, SetColor* ref)
{
	type = SET_COLOR;
	this->owner = owner;

	color = ref->color;
}

void SetColor::Initialize(Particle* particle)
{
	if (color.usingSingleValue) {
		particle->color = color.singleValue;
	}
	else {
		vec4 randomVec = vec4{
			randomInt(color.rangeValue.lowerLimit.r, color.rangeValue.upperLimit.r),
			randomInt(color.rangeValue.lowerLimit.g, color.rangeValue.upperLimit.g),
			randomInt(color.rangeValue.lowerLimit.b, color.rangeValue.upperLimit.b),
			randomInt(color.rangeValue.lowerLimit.a, color.rangeValue.upperLimit.a),
		};

		particle->color = randomVec;
	}
}

json SetColor::SaveModule()
{
	json moduleJSON;

	moduleJSON["Type"] = type;

	moduleJSON["UsingSingleValueColor"] = color.usingSingleValue;
	moduleJSON["MinColor"] = { color.rangeValue.lowerLimit.x, color.rangeValue.lowerLimit.y, color.rangeValue.lowerLimit.z, color.rangeValue.lowerLimit.a };
	moduleJSON["MaxColor"] = { color.rangeValue.upperLimit.x, color.rangeValue.upperLimit.y, color.rangeValue.upperLimit.z, color.rangeValue.upperLimit.a };

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
		color.rangeValue.lowerLimit.z = moduleJSON["MinColor"][3];
	}

	if (moduleJSON.contains("MaxColor"))
	{
		color.rangeValue.upperLimit.x = moduleJSON["MaxColor"][0];
		color.rangeValue.upperLimit.y = moduleJSON["MaxColor"][1];
		color.rangeValue.upperLimit.z = moduleJSON["MaxColor"][2];
		color.rangeValue.upperLimit.z = moduleJSON["MaxColor"][3];
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

SetScale::SetScale(Emmiter* owner, SetScale* ref)
{
	type = SET_SCALE;
	this->owner = owner;

	scale = ref->scale;
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

// set offset --------------------------------------------------------------------------------------
SetOffset::SetOffset(Emmiter* owner)
{
	type = SET_OFFSET;
	this->owner = owner;

	offset.usingSingleValue = true;
	offset.rangeValue.lowerLimit = vec3(0, 1, 0);
	offset.rangeValue.upperLimit = vec3(0, -1, 0);
}

SetOffset::SetOffset(Emmiter* owner, SetOffset* ref)
{
	type = SET_OFFSET;
	this->owner = owner;

	offset = ref->offset;
}

void SetOffset::Initialize(Particle* particle)
{
	if (offset.usingSingleValue) {
		particle->position += offset.singleValue;
	}
	else {
		vec3 randomVec = vec3{
			randomFloat(offset.rangeValue.lowerLimit.x, offset.rangeValue.upperLimit.x),
			randomFloat(offset.rangeValue.lowerLimit.y, offset.rangeValue.upperLimit.y),
			randomFloat(offset.rangeValue.lowerLimit.z, offset.rangeValue.upperLimit.z) };

		particle->position += randomVec;
	}
}

json SetOffset::SaveModule()
{
	json moduleJSON;

	moduleJSON["Type"] = type;

	moduleJSON["UsingSingleValueOffset"] = offset.usingSingleValue;
	moduleJSON["MinOffset"] = { offset.rangeValue.lowerLimit.x, offset.rangeValue.lowerLimit.y, offset.rangeValue.lowerLimit.z };
	moduleJSON["MaxOffset"] = { offset.rangeValue.upperLimit.x, offset.rangeValue.upperLimit.y, offset.rangeValue.upperLimit.z };

	return moduleJSON;
}

void SetOffset::LoadModule(const json& moduleJSON)
{
	if (moduleJSON.contains("Type"))
	{
		type = moduleJSON["Type"];
	}

	if (moduleJSON.contains("UsingSingleValueOffset"))
	{
		offset.usingSingleValue = moduleJSON["UsingSingleValueOffset"];
	}

	if (moduleJSON.contains("MinOffset"))
	{
		offset.rangeValue.lowerLimit.x = moduleJSON["MinOffset"][0];
		offset.rangeValue.lowerLimit.y = moduleJSON["MinOffset"][1];
		offset.rangeValue.lowerLimit.z = moduleJSON["MinOffset"][2];

	}

	if (moduleJSON.contains("MaxOffset"))
	{
		offset.rangeValue.upperLimit.x = moduleJSON["MaxOffset"][0];
		offset.rangeValue.upperLimit.y = moduleJSON["MaxOffset"][1];
		offset.rangeValue.upperLimit.z = moduleJSON["MaxOffset"][2];
	}
}

// set direction -----------------------------------------------------------------------------------
SetDirection::SetDirection(Emmiter* owner)
{
	type = SET_DIRECTION;
	this->owner = owner;

	direction.usingSingleValue = false;
	direction.rangeValue.lowerLimit = vec3(-0.5f, 1, -0.5f);
	direction.rangeValue.upperLimit = vec3(0.5f, 2, 0.5f);
}

SetDirection::SetDirection(Emmiter* owner, SetDirection* ref)
{
	type = SET_DIRECTION;
	this->owner = owner;

	direction = ref->direction;

}

void SetDirection::Initialize(Particle* particle)
{
	if (direction.usingSingleValue) {
		particle->direction = direction.singleValue;
	}
	else {
		vec3 randomVec = vec3{
			randomFloat(direction.rangeValue.lowerLimit.x, direction.rangeValue.upperLimit.x),
			randomFloat(direction.rangeValue.lowerLimit.y, direction.rangeValue.upperLimit.y),
			randomFloat(direction.rangeValue.lowerLimit.z, direction.rangeValue.upperLimit.z) };

		particle->direction = randomVec;
	}

	glm::normalize(particle->direction);
}

json SetDirection::SaveModule()
{
	json moduleJSON;

	moduleJSON["Type"] = type;

	moduleJSON["UsingSingleValueDirection"] = direction.usingSingleValue;
	moduleJSON["MinDirection"] = { direction.rangeValue.lowerLimit.x, direction.rangeValue.lowerLimit.y, direction.rangeValue.lowerLimit.z };
	moduleJSON["MaxDirection"] = { direction.rangeValue.upperLimit.x, direction.rangeValue.upperLimit.y, direction.rangeValue.upperLimit.z };

	return moduleJSON;
}

void SetDirection::LoadModule(const json& moduleJSON)
{
	if (moduleJSON.contains("Type"))
	{
		type = moduleJSON["Type"];
	}

	if (moduleJSON.contains("UsingSingleValueDirection"))
	{
		direction.usingSingleValue = moduleJSON["UsingSingleValueDirection"];
	}

	if (moduleJSON.contains("MinDirection"))
	{
		direction.rangeValue.lowerLimit.x = moduleJSON["MinDirection"][0];
		direction.rangeValue.lowerLimit.y = moduleJSON["MinDirection"][1];
		direction.rangeValue.lowerLimit.z = moduleJSON["MinDirection"][2];

	}

	if (moduleJSON.contains("MaxDirection"))
	{
		direction.rangeValue.upperLimit.x = moduleJSON["MaxDirection"][0];
		direction.rangeValue.upperLimit.y = moduleJSON["MaxDirection"][1];
		direction.rangeValue.upperLimit.z = moduleJSON["MaxDirection"][2];
	}
}
