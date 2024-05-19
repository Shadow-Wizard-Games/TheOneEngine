#pragma once
#include "Defs.h"
#include "VertexArray.h"

struct DefaultMesh
{
	DefaultMesh(StackVertexArray vao, int mat) : rendererID(vao), matID(mat){}
	~DefaultMesh() {}

	bool CheckID(StackVertexArray id) const {
		return id == rendererID;
	}
	StackVertexArray rendererID;
	int matID;
};

class InstanceCall
{
public:
	InstanceCall(StackVertexArray vao, int mat) : mesh(vao, mat) {}
	InstanceCall(DefaultMesh id) : mesh(id) {}
	~InstanceCall() { models.clear(); }

	void AddInstance(const glm::mat4& modelMat) {
		models.push_back(modelMat);
	}

	bool CheckID(StackVertexArray id) const {
		return id == mesh.rendererID;
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

	static void AddMesh(StackVertexArray meshID, int matID);

	static void AddMeshToQueue(StackVertexArray meshID, int matID, const glm::mat4& modelMat);

	//TODO: handle the delete of the VAO's
private:
	static void AddInstanceCall(StackVertexArray meshID, int matID);

	static void UpdateInstanceBuffer(const std::vector<InstanceCall>& calls);
};