#pragma once
#include "Particle.h"

#include "Billboard.h"
#include "Camera.h"
#include "Defs.h"

#include <vector>

class Emmiter;

class RenderEmmiterModule {
public:
	virtual void Reset() {};

	virtual void Update(Particle* particle, Camera* camera) {};

	virtual json SaveModule() { return json(); };

	virtual void LoadModule(const json& moduleJSON) { };

	enum RenderEmmiterModuleType {
		BILLBOARD
	};

	RenderEmmiterModuleType type;

protected:
	Emmiter* owner;
};


class BillboardRender : public RenderEmmiterModule {
public:
	BillboardRender(Emmiter* owner);

	BillboardType billboardType;

	void Update(Particle* particle, Camera* camera) override;

	json SaveModule();

	void LoadModule(const json& moduleJSON);

};