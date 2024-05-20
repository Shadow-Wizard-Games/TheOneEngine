#pragma once
#include "Defs.h"
#include "VertexArray.h"

struct DefaultMesh
{
	DefaultMesh(StackVertexArray vao, int mat) : rendererID(vao), matID(mat){}
	~DefaultMesh() {}

	bool CheckID(const StackVertexArray& id) const {
		return id == rendererID;
	}
	StackVertexArray rendererID;
	int matID;
};

class InstanceCall
{
public:
	InstanceCall(StackVertexArray vao, int mat, const glm::mat4& modelMat) : mesh(vao, mat) { AddInstance(modelMat); }
	~InstanceCall() { models.clear(); }

	void AddInstance(const glm::mat4& modelMat) {
		models.push_back(modelMat);
	}

	bool CheckID(const StackVertexArray& id) const {
		return mesh.rendererID == id;
	}

	const StackVertexArray& GetVAO() const { return mesh.rendererID; }
	const int& GetMatID() const { return mesh.matID; }
	const std::vector<glm::mat4>& GetModels() const { return models; }

private:
	DefaultMesh mesh;
	std::vector<glm::mat4> models;
};

class Renderer3D
{
public:
	static void Update();

	static void Shutdown();

	static void AddMesh(StackVertexArray meshID, int matID);

	static void AddMeshToQueue(StackVertexArray meshID, int matID, const glm::mat4& modelMat);
private:
	static void AddInstanceCall(StackVertexArray meshID, int matID, const glm::mat4& modelMat);

	static void UpdateInstanceBuffer(const std::vector<InstanceCall>& calls);
};