#pragma once
#include "Particle.h"
#include "SingleOrRandom.h"
#include "Defs.h"
#include <vector>

class Emmiter;

class UpdateEmmiterModule {
public:
	virtual void Reset() {};

	virtual void Update(double dt, Particle* particle) {};

	virtual json SaveModule() { return json(); };

	virtual void LoadModule(const json& moduleJSON) { };

	enum UpdateEmmiterModuleType {
		ACCELERATION
	};

	UpdateEmmiterModuleType type;

protected:
	Emmiter* owner;
};

// modules
class AccelerationUpdate : public UpdateEmmiterModule {
public:
	AccelerationUpdate(Emmiter* owner);

	vec3 acceleration;

	void Update(double dt, Particle* particle) override;

	json SaveModule();

	void LoadModule(const json& moduleJSON);

};