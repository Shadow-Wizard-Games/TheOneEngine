#pragma once

#include "Defs.h"
#include "VertexArray.h"
#include "GameObject.h"
#include "Camera.h"
#include "Light.h"
#include "RenderTarget.h"
#include "FrameBuffer.h"

#include <memory>


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

class SkeletalCall
{
public:
	SkeletalCall(int meshID, int mat, const glm::mat4& modelMat) :
		resourceID(meshID), matID(mat), model(modelMat) {}

	~SkeletalCall() = default;

	bool CheckID(const int& id) const {
		resourceID == id;
	}

	const int& GetMeshResourceID() const { return resourceID; }
	const int& GetMatID() const { return matID; }
	const glm::mat4& GetModel() const { return model; }

private:
	int resourceID;
	int matID;
	glm::mat4 model;
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
	static FrameBuffer* GetFrameBuffer(unsigned int targetID, std::string name);


	static void AddLight(std::shared_ptr<GameObject> container);
	static void CleanLights();

	static void AddMesh(StackVertexArray meshID, int matID);
	static void AddMeshToQueue(StackVertexArray meshID, int matID, const glm::mat4& modelMat);
	static void AddSkeletalMeshToQueue(int meshID, int matID, const glm::mat4& modelMat);

private:
	static void AddInstanceCall(StackVertexArray meshID, int matID, const glm::mat4& modelMat);
	static void UpdateInstanceBuffer(const std::vector<InstanceCall>& calls);

	static void DrawInstanced(const InstanceCall& call);
	static void DrawSkeletal(const SkeletalCall& call);

	static void GeometryPass(RenderTarget target);
	static void PostProcess(RenderTarget target);
	static void ShadowPass(RenderTarget target);
	static void LightPass(RenderTarget target);
	static void DrawScreenQuad();

	static void InitPreLightingShader();
	static void InitPostLightingShader();
};