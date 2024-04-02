#pragma once
#include "Particle.h"
#include "SingleOrRandom.h"
#include "Defs.h"
#include <vector>

class Emmiter;

class InitializeEmmiterModule {
public:
	virtual void Reset() {};

	virtual void Initialize(Particle* particle) {};

	virtual json SaveModule() { return json(); };

	virtual void LoadModule(const json& moduleJSON) { };

	enum InitializeEmmiterModuleType {
		SET_SPEED,
		SET_COLOR,
		SET_SCALE
	};

	InitializeEmmiterModuleType type;

protected:
	Emmiter* owner;
};


class SetSpeed : public InitializeEmmiterModule {
public:
	SetSpeed(Emmiter* owner);

	void Initialize(Particle* particle);

	json SaveModule();

	void LoadModule(const json& moduleJSON);

	SingleOrRandom<vec3> speed;
};

class SetColor : public InitializeEmmiterModule {
public:
	SetColor(Emmiter* owner);

	void Initialize(Particle* particle);

	json SaveModule();

	void LoadModule(const json& moduleJSON);

	SingleOrRandom<vec3> color;
};

class SetScale : public InitializeEmmiterModule {
public:
	SetScale(Emmiter* owner);

	void Initialize(Particle* particle);

	json SaveModule();

	void LoadModule(const json& moduleJSON);

	SingleOrRandom<vec3> scale;
};