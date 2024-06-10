#pragma once

#include "Defs.h"
#include "VertexArray.h"
#include "GameObject.h"
#include "Resource.h"
#include "Camera.h"
#include "Light.h"
#include "Shader.h"
#include "RenderTarget.h"
#include "FrameBuffer.h"
#include "Easing.h"

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
	SkeletalCall(int meshID, int mat, const glm::mat4& modelMat, bool fx = true) :
		resourceID(meshID), matID(mat), model(modelMat), hasEffect(fx) {}

	~SkeletalCall() = default;

	bool CheckID(const int& id) const {
		resourceID == id;
	}

	const int& GetMeshResourceID() const { return resourceID; }
	const int& GetMatID() const { return matID; }
	const glm::mat4& GetModel() const { return model; }
	bool HasEffect() const { return hasEffect; }

private:
	int resourceID;
	int matID;
	glm::mat4 model;
	bool hasEffect;
};

class InstanceCall
{
public:
	InstanceCall(const StackVertexArray& vao, int mat, const glm::mat4& modelMat) :
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

	void DeleteInstance() {
		instanceBuffer.Delete();
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
	static void Update(RenderTarget target);
	static void Shutdown();

	static void ResetCalls();

	static void GeometryPass(RenderTarget target);
	static void IndexPass(RenderTarget target);
	static bool InitPostProcess(RenderTarget target);
	static void EndPostProcess(RenderTarget target);
	static void ShadowPass(RenderTarget target);
	static void LightPass(RenderTarget target);
	static void UIComposition(RenderTarget target);
	static void CRTShader(RenderTarget target);

	static void SetUniformsParticleShader(ResourceId materialID, RenderTarget target);

	static void AddLight(std::shared_ptr<GameObject> container);
	static void RemoveLight(std::shared_ptr<GameObject> container);
	static void CleanLights();
	static void ResetUniforms(LightType lightType);
	static void ResetAllUniforms();

	static void AddMesh(StackVertexArray meshID, int matID);
	static void AddMeshToQueue(StackVertexArray meshID, int matID, const glm::mat4& modelMat);
	static void AddSkeletalMeshToQueue(int meshID, int matID, const glm::mat4& modelMat, bool hasEffect = false);

	// Stats
	struct Statistics
	{
		uint32_t InstanceCalls = 0;
		uint32_t SkeletalCalls = 0;
		uint32_t MeshesInQueue = 0;
	};
	static Statistics GetStats();
	static void ResetStats();
	static int GetMeshCount();
	static void ActivateBloodEffect();
	static void ActivateHealingEffect();
	static void ActivateAdrenalineEffect();
	static void ActivateBloodEffectConstant();

private:
	static void AddInstanceCall(StackVertexArray meshID, int matID, const glm::mat4& modelMat);
	static void UpdateInstanceBuffer(const std::vector<InstanceCall>& calls);

	static void DrawInstanced(const InstanceCall& call);
	static void DrawSkeletal(const SkeletalCall& call, bool overrideEffects = false);

	static void InitPreLightingShader();
	static void InitPostLightingShader();
	static void InitIndexShaders();
	static void InitCRTShader();

	// CRT Shader Effects
	static void BloodEffect(float& kBlood);
	static void AdrenalineEffect(float& kAdrenaline);
};