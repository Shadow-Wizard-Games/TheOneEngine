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
		ACCELERATION,
		COLOR_OVER_LIFE,
		SCALE_OVER_LIFE
	};

	UpdateEmmiterModuleType type;

protected:
	Emmiter* owner;
};

// modules
class AccelerationUpdate : public UpdateEmmiterModule {
public:
	AccelerationUpdate(Emmiter* owner);
	AccelerationUpdate(Emmiter* owner, AccelerationUpdate* ref);

	vec3 acceleration;

	void Update(double dt, Particle* particle) override;

	json SaveModule();

	void LoadModule(const json& moduleJSON);

};

class ColorOverLifeUpdate : public UpdateEmmiterModule {
public:
	ColorOverLifeUpdate(Emmiter* owner);
	ColorOverLifeUpdate(Emmiter* owner, ColorOverLifeUpdate* ref);

	vec4 finalColor;

	bool affectR;
	bool affectG;
	bool affectB;
	bool affectA;

	// interpolation
	// InterpolationType

	void Update(double dt, Particle* particle) override;

	json SaveModule();

	void LoadModule(const json& moduleJSON);

};

class ScaleOverLifeUpdate : public UpdateEmmiterModule {
public:
	ScaleOverLifeUpdate(Emmiter* owner);
	ScaleOverLifeUpdate(Emmiter* owner, ScaleOverLifeUpdate* ref);

	vec3 finalScale;

	void Update(double dt, Particle* particle) override;

	json SaveModule();

	void LoadModule(const json& moduleJSON);

};