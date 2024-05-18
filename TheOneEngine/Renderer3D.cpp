#include "Renderer3D.h"

struct Renderer3DData
{
	std::vector<DefaultMesh> meshes;
	std::vector<InstanceCall> instanceCalls;
};

static Renderer3DData renderer3D;

void Renderer3D::Update()
{
}

void Renderer3D::AddMesh(unsigned int meshID, int matID)
{
	bool exists = false;
	for (DefaultMesh mesh : renderer3D.meshes) {
		if (!mesh.CheckID(meshID))
			continue;

		exists = true;
		break;
	}
	if(!exists)
		renderer3D.meshes.emplace_back(meshID, matID);
}

void Renderer3D::AddMeshToQueue(unsigned int meshID, const glm::mat4& modelMat)
{
	for (InstanceCall call : renderer3D.instanceCalls) {
		if (call.CheckID(meshID))
			call.AddInstance(modelMat);
		else
			AddInstanceCall(meshID);
	}
}

void Renderer3D::AddInstanceCall(unsigned int meshID)
{
	renderer3D.instanceCalls.emplace_back(meshID);
}
