#include "Renderer3D.h"
#include "Resources.h"

struct Renderer3DData
{
	std::vector<DefaultMesh> meshes;
	std::vector<InstanceCall> instanceCalls;
};

static Renderer3DData renderer3D;

void Renderer3D::Update()
{
	//Update Instances
	UpdateInstanceBuffer(renderer3D.instanceCalls);

	for (const InstanceCall& call : renderer3D.instanceCalls) {
		Material* mat;
		if (call.GetMatID() == -1)
			mat = Resources::GetResourceById<Material>(0);
		else
			mat = Resources::GetResourceById<Material>(call.GetMatID());

		Shader* matShader = mat->getShader();
		matShader->Bind();

		mat->Bind();

		call.GetVAO().Bind();
		GLCALL(glDrawElementsInstanced(
			GL_TRIANGLES, call.GetVAO().GetIndexBuffer().GetCount(), GL_UNSIGNED_INT, 0, call.GetModels().size()))
			
		call.GetVAO().Unbind();
		mat->UnBind();
	}

	renderer3D.instanceCalls.clear();
}

void Renderer3D::Shutdown()
{
	for (DefaultMesh& mesh : renderer3D.meshes)
		mesh.rendererID.Delete();
}

void Renderer3D::AddMesh(StackVertexArray meshID, int matID)
{
	bool exists = false;
	for (const DefaultMesh& mesh : renderer3D.meshes) {
		if (!mesh.CheckID(meshID))
			continue;

		exists = true;
		break;
	}
	if(!exists)
		renderer3D.meshes.emplace_back(meshID, matID);
}

void Renderer3D::AddMeshToQueue(StackVertexArray meshID, int matID, const glm::mat4& modelMat)
{
	if (renderer3D.instanceCalls.empty()) {
		AddInstanceCall(meshID, matID, modelMat);
		return;
	}

	for (InstanceCall& call : renderer3D.instanceCalls) {
		if (call.CheckID(meshID))
		{
			call.AddInstance(modelMat);
			return;
		}
	}

	AddInstanceCall(meshID, matID, modelMat);
}

void Renderer3D::AddInstanceCall(StackVertexArray meshID, int matID, const glm::mat4& modelMat)
{
	renderer3D.instanceCalls.emplace_back(meshID, matID, modelMat);
}

void Renderer3D::UpdateInstanceBuffer(const std::vector<InstanceCall>& calls)
{
	for (const InstanceCall& call : calls)
	{
		const VertexBuffer& VBO = call.GetVAO().GetVertexBuffer();
		VBO.Bind();
		size_t size = call.GetModels().size() * sizeof(glm::mat4);
		VBO.SetData(call.GetModels().data(), size, 32);
	}
}
