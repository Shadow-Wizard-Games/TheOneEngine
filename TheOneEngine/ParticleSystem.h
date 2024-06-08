#pragma once

#include "Component.h"

#include "Transform.h"
#include "Emmiter.h"

#include <vector>
#include <memory>

class ParticleSystem : public Component {

public:
	ParticleSystem(std::shared_ptr<GameObject> containerGO);
	ParticleSystem(std::shared_ptr<GameObject> containerGO, ParticleSystem* ref);
	~ParticleSystem();

	void Update(double dt) override;
	void DrawComponent(Camera* camera) override;

	// continues from where it was
	void Play();
	void Pause();
	// starts from the beginning
	void Replay();
	void Stop();
	// continues without spawning more
	void End();

	bool HasToEnd();

	void ClearEmmiters();
	void RemoveEmmiter(Emmiter* emmiter);
	Emmiter* AddEmmiter();
	Emmiter* AddEmmiter(Emmiter* ref);

	json SaveComponent();
	void LoadComponent(const json& transformJSON);

	void ExportParticles();

	bool IsON();

	Transform* GetTransform();

	std::vector<std::unique_ptr<Emmiter>> emmiters;

	bool startON;

protected:

private:
	Transform* transform;

	bool isON;
	bool hasToEnd;
};