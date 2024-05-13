#pragma once
#include "Particle.h"

#include "Billboard.h"
#include "Camera.h"
#include "Defs.h"
#include "Resources.h"

#include <vector>

class Emmiter;

class RenderEmmiterModule {
public:
	virtual void Reset() {};

	virtual void Update(Particle* particle, Camera* camera) {};

	virtual json SaveModule() { return json(); };

	virtual void LoadModule(const json& moduleJSON) { };

	void SetTexture(const std::string& filename);
	ResourceId GetTextureID() { return textureID; }

	enum RenderEmmiterModuleType {
		BILLBOARD
	};

	RenderEmmiterModuleType type;

protected:
	std::string texturePath;
	ResourceId textureID = -1;
	Emmiter* owner;
};


class BillboardRender : public RenderEmmiterModule {
public:
	BillboardRender(Emmiter* owner);
	BillboardRender(Emmiter* owner, BillboardRender* ref);

	BillboardType billboardType;

	void Update(Particle* particle, Camera* camera) override;

	json SaveModule();

	void LoadModule(const json& moduleJSON);

};