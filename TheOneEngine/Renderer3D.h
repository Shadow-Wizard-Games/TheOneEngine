#pragma once
#include "Defs.h"

struct DefaultMesh
{
	unsigned int meshID;
	unsigned int textureID;
};

class InstanceCall
{
public:
	InstanceCall(unsigned int id) : meshID(id) {}
	~InstanceCall() { models.clear(); }

	void AddInstance(const glm::mat4& modelMat) {
		models.push_back(modelMat);
	}

	bool CheckID(unsigned int id) {
		return id == meshID;
	}

private:
	unsigned int meshID;
	std::vector<glm::mat4> models;
};

class Renderer3D
{
public:
	static void Update();

	static void AddMesh(unsigned int meshID, unsigned int textureID);

	static void AddMeshToQueue(unsigned int meshID, const glm::mat4& modelMat);

private:
	static void AddInstanceCall(unsigned int meshID);
};