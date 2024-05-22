#pragma once

#include "Defs.h"
#include "VertexArray.h"
#include "GameObject.h"
#include "Camera.h"
#include "RenderTarget.h"
#include "FrameBuffer.h"


#define MAX_INSTANCES 1000


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
	InstanceCall(StackVertexArray vao, int mat, const glm::mat4& modelMat) :
		mesh(vao, mat), 
		instanceBuffer(MAX_INSTANCES)
	{ 
		AddInstance(modelMat);
		instanceBuffer.SetLayout({{ ShaderDataType::Mat4,"a_Model" }});
		mesh.rendererID.AddVertexBuffer(instanceBuffer);
	}

	~InstanceCall() { models.clear(); }

	void AddInstance(const glm::mat4& modelMat) {
		models.push_back(modelMat);
	}

	bool CheckID(const StackVertexArray& id) const {
		return mesh.rendererID == id;
	}

	const StackVertexArray& GetVAO() const { return mesh.rendererID; }
	const VertexBuffer& GetInstanceBuffer() const { return instanceBuffer; }
	const int& GetMatID() const { return mesh.matID; }
	const std::vector<glm::mat4>& GetModels() const { return models; }

private:
	DefaultMesh mesh;
	std::vector<glm::mat4> models;
	VertexBuffer instanceBuffer;
};


class Renderer3D
{
public:
	static void Init();
	static void Update();
	static void Shutdown();

	static unsigned int AddRenderTarget(DrawMode mode, Camera* camera, glm::vec2 viewportSize, std::vector<std::vector<Attachment>> frameBuffers);
	static std::vector<FrameBuffer>* GetFrameBuffers(unsigned int targetID);

	static void AddMesh(StackVertexArray meshID, int matID);
	static void AddMeshToQueue(StackVertexArray meshID, int matID, const glm::mat4& modelMat);

private:
	static void AddInstanceCall(StackVertexArray meshID, int matID, const glm::mat4& modelMat);
	static void UpdateInstanceBuffer(const std::vector<InstanceCall>& calls);
};