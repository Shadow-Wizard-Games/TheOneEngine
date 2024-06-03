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
		SET_SCALE,
		SET_OFFSET,
		SET_DIRECTION
	};

	InitializeEmmiterModuleType type;

protected:
	Emmiter* owner;
};


class SetSpeed : public InitializeEmmiterModule {
public:
	SetSpeed(Emmiter* owner);
	SetSpeed(Emmiter* owner, SetSpeed* ref);

	void Initialize(Particle* particle);

	json SaveModule();

	void LoadModule(const json& moduleJSON);

	SingleOrRandom<float> speed;
};

class SetColor : public InitializeEmmiterModule {
public:
	SetColor(Emmiter* owner);
	SetColor(Emmiter* owner, SetColor* ref);

	void Initialize(Particle* particle);

	json SaveModule();

	void LoadModule(const json& moduleJSON);

	SingleOrRandom<glm::vec4> color;
};

class SetScale : public InitializeEmmiterModule {
public:
	SetScale(Emmiter* owner);
	SetScale(Emmiter* owner, SetScale* ref);

	void Initialize(Particle* particle);

	json SaveModule();

	void LoadModule(const json& moduleJSON);

	SingleOrRandom<vec3> scale;
	bool isProportional;
};

class SetOffset : public InitializeEmmiterModule {
public:
	SetOffset(Emmiter* owner);
	SetOffset(Emmiter* owner, SetOffset* ref);

	void Initialize(Particle* particle);

	json SaveModule();

	void LoadModule(const json& moduleJSON);

	SingleOrRandom<vec3> offset;
};

class SetDirection : public InitializeEmmiterModule {
public:
	SetDirection(Emmiter* owner);
	SetDirection(Emmiter* owner, SetDirection* ref);

	void Initialize(Particle* particle);

	json SaveModule();

	void LoadModule(const json& moduleJSON);

	SingleOrRandom<vec3> direction;
};