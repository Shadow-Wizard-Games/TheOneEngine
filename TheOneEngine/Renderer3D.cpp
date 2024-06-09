#include "Renderer3D.h"
#include "Renderer.h"
#include "Resources.h"
#include "Shader.h"
#include "Material.h"
#include "EngineCore.h"
#include "N_SceneManager.h"
#include "SkeletalUtilities.h"
#include "Animation/OzzAnimation.h"

#include "TheOneAnimation/samples/framework/internal/shader.h"

#include "ozz/geometry/runtime/skinning_job.h"
#include "ozz/base/maths/internal/simd_math_config.h"
#include "ozz/base/span.h"

static const glm::mat4 cameraUI = glm::ortho(-1.0f, 1.0f, 1.0f, -1.0f);

struct Renderer3DData
{
	std::vector<std::shared_ptr<GameObject>> lights;
	std::vector<std::shared_ptr<GameObject>> transparentObjects;

	std::vector<DefaultMesh> meshes;
	std::vector<InstanceCall> instanceCalls;
	std::vector<SkeletalCall> skeletalCalls;

	ResourceId lightingMatID = -1;
	ResourceId skeletalShaderID = -1;
	ResourceId crtMatID = -1;

	uint dynamicVAO = 0;
	uint dynamicVBO = 0;
	uint dynamicIBO = 0;

	Renderer3D::Statistics stats;

	bool recievedDamage;
	Easing* screenBloodIn;
	Easing* screenBloodOut;
};

static Renderer3DData renderer3D;

void Renderer3D::Init()
{
	if (renderer3D.dynamicVAO == 0) {
		GLCALL(glGenVertexArrays(1, &renderer3D.dynamicVAO));
	}

	if (renderer3D.dynamicVBO == 0) {
		GLCALL(glGenBuffers(1, &renderer3D.dynamicVBO));
	}

	if (renderer3D.dynamicIBO == 0) {
		GLCALL(glGenBuffers(1, &renderer3D.dynamicIBO));
	}

	InitPreLightingShader();
	InitPostLightingShader();
	InitIndexShaders();
	InitCRTShader();

	renderer3D.screenBloodIn = new Easing(0.15);
	renderer3D.screenBloodOut = new Easing(0.45);
}

void Renderer3D::Update(RenderTarget target)
{
	// Update Instances
	UpdateInstanceBuffer(renderer3D.instanceCalls);
}

void Renderer3D::Shutdown()
{
	for (DefaultMesh& mesh : renderer3D.meshes)
		mesh.rendererID.Delete();

	if (renderer3D.dynamicVAO) {
		GLCALL(glDeleteVertexArrays(1, &renderer3D.dynamicVAO));
		renderer3D.dynamicVAO = 0;
	}

	if (renderer3D.dynamicVBO) {
		GLCALL(glDeleteBuffers(1, &renderer3D.dynamicVBO));
		renderer3D.dynamicVBO = 0;
	}

	if (renderer3D.dynamicIBO) {
		GLCALL(glDeleteBuffers(1, &renderer3D.dynamicIBO));
		renderer3D.dynamicIBO = 0;
	}
}

void Renderer3D::ResetCalls()
{
	for (auto& instance : renderer3D.instanceCalls)
		instance.DeleteInstance();

	renderer3D.instanceCalls.clear();
	renderer3D.skeletalCalls.clear();
}

void Renderer3D::AddLight(std::shared_ptr<GameObject> container)
{
	renderer3D.lights.push_back(container);
}

void Renderer3D::RemoveLight(std::shared_ptr<GameObject> container)
{
	if (!container.get())
		return;

	if (!container.get()->GetComponent<Light>())
		return;

	auto it = std::find_if(renderer3D.lights.begin(), renderer3D.lights.end(),
		[container](const std::shared_ptr<GameObject>& comp) {
			return comp.get() == container.get();
		});

	LightType lightType = container.get()->GetComponent<Light>()->lightType;

	if (it != renderer3D.lights.end())
		renderer3D.lights.erase(it);

	Renderer3D::ResetUniforms(lightType);
}

void Renderer3D::CleanLights()
{
	renderer3D.lights.clear();
}

void Renderer3D::ResetUniforms(LightType lightType)
{
	for (auto &light : renderer3D.lights)
	{
		if (lightType == light.get()->GetComponent<Light>()->lightType)
			return;
	}

	Material* mat = Resources::GetResourceById<Material>(renderer3D.lightingMatID);
	Shader* shader = mat->getShader();
	shader->Bind();

	switch (lightType)
	{
		case LightType::Directional:
		{
			mat->SetUniformData("u_DirLight[0].Position", (glm::vec3)(0));
			mat->SetUniformData("u_DirLight[0].Direction", (glm::vec3)(0));
			mat->SetUniformData("u_DirLight[0].Color", 0);
			mat->SetUniformData("u_DirLight[0].Intensity", 0);
			break;
		}
		case LightType::Point:
		{
			mat->SetUniformData("u_PointLights[0].Position", (glm::vec3)(0));
			mat->SetUniformData("u_PointLights[0].Color", 0);
			mat->SetUniformData("u_PointLights[0].Intensity", 0);
			mat->SetUniformData("u_PointLights[0].Linear", 0);
			mat->SetUniformData("u_PointLights[0].Quadratic", 0);
			mat->SetUniformData("u_PointLights[0].Radius", 0);
			break;
		}
		case LightType::Spot:
		{
			mat->SetUniformData("u_SpotLights[0].Position", (glm::vec3)(0));
			mat->SetUniformData("u_SpotLights[0].Direction", (glm::vec3)(0));
			mat->SetUniformData("u_SpotLights[0].Color", 0);
			mat->SetUniformData("u_SpotLights[0].Intensity", 0);
			mat->SetUniformData("u_SpotLights[0].Linear", 0);
			mat->SetUniformData("u_SpotLights[0].Quadratic", 0);
			mat->SetUniformData("u_SpotLights[0].Radius", 0);
			mat->SetUniformData("u_SpotLights[0].CutOff", 0);
			mat->SetUniformData("u_SpotLights[0].OuterCutOff", 0);
			mat->SetUniformData("u_SpotLights[0].ViewProjectionMat", glm::mat4(0));
			mat->SetUniformData("u_SpotLights[0].Depth", 0);
			break;
		}
	}
}

void Renderer3D::ResetAllUniforms()
{
	Material* mat = Resources::GetResourceById<Material>(renderer3D.lightingMatID);
	Shader* shader = mat->getShader();
	shader->Bind();
	for (int i = 0; i < 32; i++)
	{
		string iteration = to_string(i);
		mat->SetUniformData("u_DirLight[" + iteration + "].Position", (glm::vec3)(0));
		mat->SetUniformData("u_DirLight[" + iteration + "].Direction", (glm::vec3)(0));
		mat->SetUniformData("u_DirLight[" + iteration + "].Color", 0);
		mat->SetUniformData("u_DirLight[" + iteration + "].Intensity", 0);

		mat->SetUniformData("u_PointLights[" + iteration + "].Position", (glm::vec3)(0));
		mat->SetUniformData("u_PointLights[" + iteration + "].Color", 0);
		mat->SetUniformData("u_PointLights[" + iteration + "].Intensity", 0);
		mat->SetUniformData("u_PointLights[" + iteration + "].Linear", 0);
		mat->SetUniformData("u_PointLights[" + iteration + "].Quadratic", 0);
		mat->SetUniformData("u_PointLights[" + iteration + "].Radius", 0);

		mat->SetUniformData("u_SpotLights[" + iteration + "].Position", (glm::vec3)(0));
		mat->SetUniformData("u_SpotLights[" + iteration + "].Direction", (glm::vec3)(0));
		mat->SetUniformData("u_SpotLights[" + iteration + "].Color", 0);
		mat->SetUniformData("u_SpotLights[" + iteration + "].Intensity", 0);
		mat->SetUniformData("u_SpotLights[" + iteration + "].Linear", 0);
		mat->SetUniformData("u_SpotLights[" + iteration + "].Quadratic", 0);
		mat->SetUniformData("u_SpotLights[" + iteration + "].Radius", 0);
		mat->SetUniformData("u_SpotLights[" + iteration + "].CutOff", 0);
		mat->SetUniformData("u_SpotLights[" + iteration + "].OuterCutOff", 0);
		mat->SetUniformData("u_SpotLights[" + iteration + "].ViewProjectionMat", glm::mat4(0));
		mat->SetUniformData("u_SpotLights[" + iteration + "].Depth", 0);
	}
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
	renderer3D.stats.MeshesInQueue++;
}

void Renderer3D::AddSkeletalMeshToQueue(int meshID, int matID, const glm::mat4& modelMat, bool hasEffect)
{
	renderer3D.skeletalCalls.emplace_back(meshID, matID, modelMat, hasEffect);
	renderer3D.stats.SkeletalCalls++;
}

Renderer3D::Statistics Renderer3D::GetStats()
{
	return renderer3D.stats;
}

void Renderer3D::ResetStats()
{
	memset(&renderer3D.stats, 0, sizeof(Statistics));
}

int Renderer3D::GetMeshCount()
{
	return renderer3D.meshes.size();
}

void Renderer3D::ActivateBloodEffect()
{
	renderer3D.recievedDamage = true;
}

void Renderer3D::AddInstanceCall(StackVertexArray meshID, int matID, const glm::mat4& modelMat)
{
	renderer3D.instanceCalls.emplace_back(meshID, matID, modelMat);
	renderer3D.stats.InstanceCalls++;
}

void Renderer3D::UpdateInstanceBuffer(const std::vector<InstanceCall>& calls)
{
	for (const InstanceCall& call : calls)
	{
		call.GetInstanceBuffer().Bind();
		call.GetInstanceBuffer().AllocateData(call.GetModels().data(), call.GetModels().size() * sizeof(glm::mat4));
	}
}

void Renderer3D::DrawInstanced(const InstanceCall& call)
{
	Material* mat;
	if (call.GetMatID() == -1)
		mat = Resources::GetResourceById<Material>(0);
	else
		mat = Resources::GetResourceById<Material>(call.GetMatID());

	Shader* matShader = mat->getShader();

	matShader->Bind();
	mat->Bind();
	call.GetVAO().Bind();
	call.GetInstanceBuffer().Bind();

	GLCALL(glDrawElementsInstanced(
		GL_TRIANGLES, call.GetVAO().GetIndexBuffer().GetCount(), GL_UNSIGNED_INT, 0, call.GetModels().size()));

	call.GetVAO().Unbind();
	mat->UnBind();
	matShader->UnBind();
}

void Renderer3D::DrawSkeletal(const SkeletalCall& call, bool overrideEffects)
{
	SkeletalModel* mesh = Resources::GetResourceById<SkeletalModel>(call.GetMeshResourceID());

	const ozz::span<ozz::math::Float4x4> skinningMatrices = ozz::make_span(mesh->getSkinningMatrices());

	Material* material;
	if (call.GetMatID() == -1)
		material = Resources::GetResourceById<Material>(0);
	else
		material = Resources::GetResourceById<Material>(call.GetMatID());

	ozz::sample::Mesh ozzMesh = mesh->GetOzzMesh();
	const int vertexCount = ozzMesh.vertex_count();

	// Positions and normals are interleaved to improve caching while executing
	// skinning job.

	const GLsizei positionsVertexDataOffset = 0;
	const GLsizei positionsVertexDataStride = sizeof(float) * 3;
	const GLsizei normalsVertexDataOffset = vertexCount * positionsVertexDataStride;
	const GLsizei normalsVertexDataStride = sizeof(float) * 3;
	const GLsizei tangentsOffset =
		normalsVertexDataOffset + vertexCount * normalsVertexDataStride;
	const GLsizei tangentsStride = sizeof(float) * 3;
	const GLsizei skinnedDataSize =
		tangentsOffset + vertexCount * tangentsStride;

	// Colors and uvs are contiguous. They aren't transformed, so they can be
	// directly copied from source mesh which is non-interleaved as-well.
	// Colors will be filled with white if _options.colors is false.
	// UVs will be skipped if _options.textured is false.
	const GLsizei colorsVertexDataOffset = skinnedDataSize;
	const GLsizei colorsVertexDataStride = sizeof(uint8_t) * 4;
	const GLsizei colorsSize = vertexCount * colorsVertexDataStride;
	const GLsizei uvsVertexDataOffset = colorsVertexDataOffset + colorsSize;
	const GLsizei uvsVertexDataStride = sizeof(float) * 2;
	const GLsizei uvsSize = vertexCount * uvsVertexDataStride;
	const GLsizei fixedDataSize = colorsSize + uvsSize;

	// Reallocate vertex buffer.
	const GLsizei vboSize = skinnedDataSize + fixedDataSize;

	void* vboMap = engine->scratch_buffer_.Resize(vboSize);

	// Iterate mesh parts and fills vbo.
	// Runs a skinning job per mesh part. Triangle indices are shared
	// across parts.
	size_t processedVertexCount = 0;
	for (size_t i = 0; i < ozzMesh.parts.size(); ++i) {
		const ozz::sample::Mesh::Part& meshpart = ozzMesh.parts[i];

		// Skip this iteration if no vertex.
		const size_t meshpartVertexCount = meshpart.positions.size() / 3;
		if (meshpartVertexCount == 0) {
			continue;
		}

		// Fills the job.
		ozz::geometry::SkinningJob skinningJob;
		skinningJob.vertex_count = static_cast<int>(meshpartVertexCount);
		const int part_influences_count = meshpart.influences_count();

		// Clamps joints influence count according to the option.
		skinningJob.influences_count = part_influences_count;

		// Setup skinning matrices, that came from the animation stage before being
		// multiplied by inverse model-space bind-pose.
		skinningJob.joint_matrices = skinningMatrices;

		// Setup joint's indices.
		skinningJob.joint_indices = make_span(meshpart.joint_indices);
		skinningJob.joint_indices_stride =
			sizeof(uint16_t) * part_influences_count;

		// Setup joint's weights.
		if (part_influences_count > 1) {
			skinningJob.joint_weights = make_span(meshpart.joint_weights);
			skinningJob.joint_weights_stride =
				sizeof(float) * (part_influences_count - 1);
		}

		// Setup input positions, coming from the loaded mesh.
		skinningJob.in_positions = make_span(meshpart.positions);
		skinningJob.in_positions_stride =
			sizeof(float) * ozz::sample::Mesh::Part::kPositionsCpnts;

		// Setup output positions, coming from the rendering output mesh buffers.
		// We need to offset the buffer every loop.
		float* outPositionsBegin = reinterpret_cast<float*>(ozz::PointerStride(
			vboMap, positionsVertexDataOffset + processedVertexCount * positionsVertexDataStride));
		float* outPositionsEnd = ozz::PointerStride(
			outPositionsBegin, meshpartVertexCount * positionsVertexDataStride);
		skinningJob.out_positions = { outPositionsBegin, outPositionsEnd };
		skinningJob.out_positions_stride = positionsVertexDataStride;

		// Setup normals if input are provided.
		float* outNormalBegin = reinterpret_cast<float*>(ozz::PointerStride(
			vboMap, normalsVertexDataOffset + processedVertexCount * normalsVertexDataStride));
		float* outNormalEnd = ozz::PointerStride(
			outNormalBegin, meshpartVertexCount * normalsVertexDataStride);

		if (meshpart.normals.size() / ozz::sample::Mesh::Part::kNormalsCpnts ==
			meshpartVertexCount) {
			// Setup input normals, coming from the loaded mesh.
			skinningJob.in_normals = make_span(meshpart.normals);
			skinningJob.in_normals_stride =
				sizeof(float) * ozz::sample::Mesh::Part::kNormalsCpnts;

			// Setup output normals, coming from the rendering output mesh buffers.
			// We need to offset the buffer every loop.
			skinningJob.out_normals = { outNormalBegin, outNormalEnd };
			skinningJob.out_normals_stride = normalsVertexDataStride;
		}
		else {
			// Fills output with default normals.
			for (float* normal = outNormalBegin; normal < outNormalEnd;
				normal = ozz::PointerStride(normal, normalsVertexDataStride)) {
				normal[0] = 0.f;
				normal[1] = 1.f;
				normal[2] = 0.f;
			}
		}

		// Setup tangents if input are provided.
		float* outTangentBegin = reinterpret_cast<float*>(ozz::PointerStride(
			vboMap, tangentsOffset + processedVertexCount * tangentsStride));
		float* outTangentEnd = ozz::PointerStride(
			outTangentBegin, meshpartVertexCount * tangentsStride);

		if (meshpart.tangents.size() / ozz::sample::Mesh::Part::kTangentsCpnts ==
			meshpartVertexCount) {
			// Setup input tangents, coming from the loaded mesh.
			skinningJob.in_tangents = make_span(meshpart.tangents);
			skinningJob.in_tangents_stride =
				sizeof(float) * ozz::sample::Mesh::Part::kTangentsCpnts;

			// Setup output tangents, coming from the rendering output mesh buffers.
			// We need to offset the buffer every loop.
			skinningJob.out_tangents = { outTangentBegin, outTangentEnd };
			skinningJob.out_tangents_stride = tangentsStride;
		}
		else {
			// Fills output with default tangents.
			for (float* tangent = outTangentBegin; tangent < outTangentEnd;
				tangent = ozz::PointerStride(tangent, tangentsStride)) {
				tangent[0] = 1.f;
				tangent[1] = 0.f;
				tangent[2] = 0.f;
			}
		}

		// Execute the job, which should succeed unless a parameter is invalid.
		if (!skinningJob.Run()) {
			return;
		}

		// OZZ COLORS
		// Un-optimal path used when the right number of colors is not provided.
		static_assert(sizeof(kDefaultColorsArray[0]) == colorsVertexDataStride,
			"Stride mismatch");

		for (size_t j = 0; j < meshpartVertexCount;
			j += OZZ_ARRAY_SIZE(kDefaultColorsArray)) {
			const size_t this_loop_count = ozz::math::Min(
				OZZ_ARRAY_SIZE(kDefaultColorsArray), meshpartVertexCount - j);
			memcpy(ozz::PointerStride(
				vboMap, colorsVertexDataOffset +
				(processedVertexCount + j) * colorsVertexDataStride),
				kDefaultColorsArray, colorsVertexDataStride * this_loop_count);
		}

		if (meshpartVertexCount ==
			meshpart.uvs.size() / ozz::sample::Mesh::Part::kUVsCpnts) {
			// Optimal path used when the right number of uvs is provided.
			memcpy(ozz::PointerStride(
				vboMap, uvsVertexDataOffset + processedVertexCount * uvsVertexDataStride),
				array_begin(meshpart.uvs), meshpartVertexCount * uvsVertexDataStride);
		}
		else {
			// Un-optimal path used when the right number of uvs is not provided.
			assert(sizeof(kDefaultUVsArray[0]) == uvsVertexDataStride);
			for (size_t j = 0; j < meshpartVertexCount;
				j += OZZ_ARRAY_SIZE(kDefaultUVsArray)) {
				const size_t this_loop_count = ozz::math::Min(
					OZZ_ARRAY_SIZE(kDefaultUVsArray), meshpartVertexCount - j);
				memcpy(ozz::PointerStride(
					vboMap,
					uvsVertexDataOffset + (processedVertexCount + j) * uvsVertexDataStride),
					kDefaultUVsArray, uvsVertexDataStride * this_loop_count);
			}
		}

		// Some more vertices were processed.
		processedVertexCount += meshpartVertexCount;
	}


	// ========================= RENDERING =====================================

	Shader* animShader;

	if (overrideEffects || !call.HasEffect())
	{
		animShader = material->getShader();
		animShader->Bind();
		animShader->SetModel(call.GetModel());
		material->Bind();
	}
	else
	{
		animShader = Resources::GetResourceById<Shader>(renderer3D.skeletalShaderID);
		animShader->Bind();
		animShader->SetModel(call.GetModel());
	}

	GLCALL(glBindVertexArray(renderer3D.dynamicVAO));
	// Updates dynamic vertex buffer with skinned data.
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, renderer3D.dynamicVBO));
	GLCALL(glBufferData(GL_ARRAY_BUFFER, vboSize, vboMap, GL_STREAM_DRAW));

	GLCALL(glEnableVertexAttribArray(0));
	GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, positionsVertexDataStride,
		GL_PTR_OFFSET(positionsVertexDataOffset)));

	GLCALL(glEnableVertexAttribArray(1));
	GLCALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, uvsVertexDataStride,
		GL_PTR_OFFSET(uvsVertexDataOffset)));

	GLCALL(glEnableVertexAttribArray(2));
	GLCALL(glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, normalsVertexDataStride,
		GL_PTR_OFFSET(normalsVertexDataOffset)));

	GLCALL(glEnableVertexAttribArray(3));
	GLCALL(glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, colorsVertexDataStride,
		GL_PTR_OFFSET(colorsVertexDataOffset)));


	// Maps the index dynamic buffer and update it.
	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer3D.dynamicIBO));
	const ozz::sample::Mesh::TriangleIndices& indices = ozzMesh.triangle_indices;
	GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(ozz::sample::Mesh::TriangleIndices::value_type),
		array_begin(indices), GL_STREAM_DRAW));

	// Draws the mesh.
	static_assert(sizeof(ozz::sample::Mesh::TriangleIndices::value_type) == 2,
		"Expects 2 bytes indices.");
	GLCALL(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()),
		GL_UNSIGNED_SHORT, 0));

	// Unbinds.
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

	GLCALL(glDisableVertexAttribArray(0));
	GLCALL(glDisableVertexAttribArray(1));
	GLCALL(glDisableVertexAttribArray(2));
	GLCALL(glDisableVertexAttribArray(3));

	GLCALL(glBindVertexArray(0));

	material->UnBind();
}

void Renderer3D::GeometryPass(RenderTarget target)
{
	FrameBuffer* gBuffer = target.GetFrameBuffer("gBuffer");

	gBuffer->Bind();
	gBuffer->Clear(ClearBit::All, { 0.0f, 0.0f, 0.0f, 1.0f });

	// Set Render Environment
	Renderer::SetRenderEnvironment();

	// Draw Scene
	GLCALL(glDisable(GL_BLEND));
	for (const InstanceCall& call : renderer3D.instanceCalls)
		DrawInstanced(call);

	for (const SkeletalCall& call : renderer3D.skeletalCalls)
		DrawSkeletal(call, true);

	gBuffer->Unbind();
}

bool Renderer3D::InitPostProcess(RenderTarget target)
{
	FrameBuffer* gBuffer = target.GetFrameBuffer("gBuffer");
	FrameBuffer* postBuffer = target.GetFrameBuffer("postBuffer");

	postBuffer->Bind();
	postBuffer->Clear(ClearBit::All, { 0.0f, 0.0f, 0.0f, 1.0f });

	// Copy gBuffer Depth to postBuffer
	GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer->GetBuffer()));
	GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postBuffer->GetBuffer()));
	int width = postBuffer->GetWidth();
	int height = postBuffer->GetHeight();
	GLCALL(glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST));
	if (!Renderer::Settings()->lights.isEnabled)
		GLCALL(glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST));
	postBuffer->Bind();

	if (!Renderer::Settings()->postProcess.isEnabled)
		return false;

	return true;
}

void Renderer3D::EndPostProcess(RenderTarget target)
{
	FrameBuffer* postBuffer = target.GetFrameBuffer("postBuffer");
	postBuffer->Unbind();
}

void Renderer3D::ShadowPass(RenderTarget target)
{
	std::vector<std::shared_ptr<GameObject>> lights = renderer3D.lights;

	for (int i = 0; i < lights.size(); i++)
	{
		Light* light = lights[i]->GetComponent<Light>();
		if (!light->castShadows) continue;

		// Shadow Buffer
		light->shadowBuffer->Bind();
		light->shadowBuffer->Clear(ClearBit::Depth, { 0.0f, 0.0f, 0.0f, 1.0f });

		// Set Light Camera
		Renderer::SetUniformBufferCamera(lights[i]->GetComponent<Camera>()->viewProjectionMatrix);

		// Draw Scene
		GLCALL(glDisable(GL_BLEND));
		for (const InstanceCall& call : renderer3D.instanceCalls)
			DrawInstanced(call);

		for (const SkeletalCall& call : renderer3D.skeletalCalls)
			DrawSkeletal(call, true);

		light->shadowBuffer->Unbind();
	}
}

void Renderer3D::LightPass(RenderTarget target)
{
	FrameBuffer* gBuffer = target.GetFrameBuffer("gBuffer");
	FrameBuffer* postBuffer = target.GetFrameBuffer("postBuffer");
	postBuffer->Bind();

	uint directionalLightNum = 0;
	uint pointLightNum = 0;
	uint spotLightNum = 0;

	Transform* cameraTransform = target.GetCamera()->GetContainerGO().get()->GetComponent<Transform>();

	Uniform::SamplerData gPositionData;
	gPositionData.tex_id = gBuffer->GetAttachmentTexture("position");
	Uniform::SamplerData gNormalData;
	gNormalData.tex_id = gBuffer->GetAttachmentTexture("normal");
	Uniform::SamplerData gAlbedoSpecData;
	gAlbedoSpecData.tex_id = gBuffer->GetAttachmentTexture("color");

	if (renderer3D.lightingMatID == -1)
	{
		LOG(LogType::LOG_ERROR, "Lighting Pass Failed, material was nullptr.");
		return;
	}

	Material *mat = Resources::GetResourceById<Material>(renderer3D.lightingMatID);
	Shader* shader = mat->getShader();
	shader->Bind();

	mat->SetUniformData("gPosition", gPositionData);
	mat->SetUniformData("gNormal", gNormalData);
	mat->SetUniformData("gAlbedoSpec", gAlbedoSpecData);
	mat->SetUniformData("u_ViewPos", (glm::vec3)cameraTransform->GetGlobalPosition());

	for (int i = 0; i < renderer3D.lights.size(); i++)
	{
		Light* light = renderer3D.lights[i]->GetComponent<Light>();
		Transform* transform = renderer3D.lights[i]->GetComponent<Transform>();

		// Uniform data MUST be float!!!
		switch (light->lightType)
		{
			case LightType::Directional:
			{
				std::string iteration = std::to_string(directionalLightNum);
				mat->SetUniformData("u_DirLight[" + iteration + "].Position", (glm::vec3)transform->GetGlobalPosition());
				mat->SetUniformData("u_DirLight[" + iteration + "].Direction", (glm::vec3)transform->GetGlobalForward());
				mat->SetUniformData("u_DirLight[" + iteration + "].Color", light->color);
				mat->SetUniformData("u_DirLight[" + iteration + "].Intensity", light->intensity);
				directionalLightNum++;
				break;
			}
			case LightType::Point:
			{
				std::string iteration = std::to_string(pointLightNum);
				mat->SetUniformData("u_PointLights[" + iteration + "].Position", (glm::vec3)transform->GetGlobalPosition());
				mat->SetUniformData("u_PointLights[" + iteration + "].Color", light->color);
				mat->SetUniformData("u_PointLights[" + iteration + "].Intensity", light->intensity);
				mat->SetUniformData("u_PointLights[" + iteration + "].Linear", light->linear);
				mat->SetUniformData("u_PointLights[" + iteration + "].Quadratic", light->quadratic);
				mat->SetUniformData("u_PointLights[" + iteration + "].Radius", light->radius);
				pointLightNum++;
				break;
			}
			case LightType::Spot:
			{
				std::string iteration = std::to_string(spotLightNum);
				mat->SetUniformData("u_SpotLights[" + iteration + "].Position", (glm::vec3)transform->GetGlobalPosition());
				mat->SetUniformData("u_SpotLights[" + iteration + "].Direction", (glm::vec3)transform->GetGlobalForward());
				mat->SetUniformData("u_SpotLights[" + iteration + "].Color", light->color);
				mat->SetUniformData("u_SpotLights[" + iteration + "].Intensity", light->intensity);
				mat->SetUniformData("u_SpotLights[" + iteration + "].Linear", light->linear);
				mat->SetUniformData("u_SpotLights[" + iteration + "].Quadratic", light->quadratic);
				mat->SetUniformData("u_SpotLights[" + iteration + "].Radius", light->radius);
				mat->SetUniformData("u_SpotLights[" + iteration + "].CutOff", light->innerCutOff);
				mat->SetUniformData("u_SpotLights[" + iteration + "].OuterCutOff", light->outerCutOff);
				mat->SetUniformData("u_SpotLights[" + iteration + "].ViewProjectionMat", renderer3D.lights[i]->GetComponent<Camera>()->viewProjectionMatrix);
				mat->SetUniformData("u_SpotLights[" + iteration + "].Depth", light->shadowBuffer->GetAttachmentTexture("depth"));
				spotLightNum++;
				break;
			}
		}
	}

	mat->Bind();
	Renderer::DrawScreenQuad();
	mat->UnBind();
}

void Renderer3D::IndexPass(RenderTarget target)
{
	Renderer::SetUniformBufferCamera(target.GetCamera()->viewProjectionMatrix);

	GLCALL(glEnable(GL_BLEND));
	for (const SkeletalCall& call : renderer3D.skeletalCalls)
	{
		if (call.HasEffect())
			DrawSkeletal(call);
	}

	//Renderer2D::UpdateIndexed(BatchType::WORLD, target);
}

void Renderer3D::UIComposition(RenderTarget target)
{
	FrameBuffer* readBuffer;
	FrameBuffer* uiBuffer = target.GetFrameBuffer("uiBuffer");

	if (target.GetMode() == DrawMode::BUILD_DEBUG)
	{
		readBuffer = target.GetFrameBuffer("gBuffer");
	}
	else //(GAME && BUILD_RELEASE)
	{
		readBuffer = target.GetFrameBuffer("postBuffer");
	}

	uiBuffer->Bind();
	uiBuffer->Clear(ClearBit::All, { 0.0f, 0.0f, 0.0f, 1.0f });

	// Copy postBuffer Color to uiBuffer, Upscale to Nearest
	GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, readBuffer->GetBuffer()));
	GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, uiBuffer->GetBuffer()));
	GLCALL(glBlitFramebuffer(
		0, 0, readBuffer->GetWidth(), readBuffer->GetHeight(),
		0, 0, uiBuffer->GetWidth(), uiBuffer->GetHeight(),
		GL_COLOR_BUFFER_BIT, GL_NEAREST));

	uiBuffer->Bind();

	Renderer::SetUniformBufferCamera(cameraUI);
	Renderer2D::Update(BatchType::UI, target);

	uiBuffer->Unbind();
}

void Renderer3D::CRTShader(RenderTarget target)
{
	FrameBuffer* uiBuffer = target.GetFrameBuffer("uiBuffer");
	uiBuffer->Bind();

	Material* crtMat = Resources::GetResourceById<Material>(renderer3D.crtMatID);
	Shader* crtShader = crtMat->getShader();
	crtShader->Bind();


	if (renderer3D.recievedDamage)
	{
		renderer3D.screenBloodIn->Play();
		renderer3D.recievedDamage = false;
	}
	if (renderer3D.screenBloodIn->GetState() == EasingState::END)
	{
		renderer3D.screenBloodIn->Reset();
		renderer3D.screenBloodOut->Play();
	}
	if (renderer3D.screenBloodOut->GetState() == EasingState::END)
	{
		renderer3D.screenBloodOut->Reset();
	}

	float kEasing = 0;
	if (renderer3D.screenBloodIn->GetState() == EasingState::PLAY)
	{
		kEasing = renderer3D.screenBloodIn->Ease(0.0, 1.0, engine->dt, EasingType::EASE_IN_EXP, false);
	}
	if (renderer3D.screenBloodOut->GetState() == EasingState::PLAY)
	{
		kEasing = renderer3D.screenBloodOut->Ease(1.0, 0.0, engine->dt, EasingType::EASE_OUT_SIN, false);
	}
	

	crtMat->SetUniformData("albedo", (Uniform::SamplerData)uiBuffer->GetAttachmentTexture("color_ui"));
	crtMat->SetUniformData("kEasing", kEasing);
	crtMat->SetUniformData("time", (float)engine->upTime);
	crtMat->SetUniformData("warp", glm::vec2(12, 8));

	crtMat->Bind();
	Renderer::DrawScreenQuad();
	crtMat->UnBind();

	uiBuffer->Unbind();
}

void Renderer3D::SetUniformsParticleShader(ResourceId materialID, RenderTarget target)
{
	uint directionalLightNum = 0;
	uint pointLightNum = 0;
	uint spotLightNum = 0;

	Material* mat = Resources::GetResourceById<Material>(Renderer2D::GetParticleMaterialID());
	Shader* shader = mat->getShader();
	shader->Bind();

	for (int i = 0; i < renderer3D.lights.size(); i++)
	{
		Light* light = renderer3D.lights[i]->GetComponent<Light>();
		Transform* transform = renderer3D.lights[i]->GetComponent<Transform>();

		Transform* cameraTransform = target.GetCamera()->GetContainerGO().get()->GetComponent<Transform>();
		mat->SetUniformData("camPos", (glm::vec3)cameraTransform->GetGlobalPosition());

		// Uniform data MUST be float!!!
		switch (light->lightType)
		{
			case LightType::Directional:
			{
				std::string iteration = std::to_string(directionalLightNum);
				mat->SetUniformData("u_DirLight[" + iteration + "].Color", light->color);
				mat->SetUniformData("u_DirLight[" + iteration + "].Intensity", light->intensity);
				directionalLightNum++;
				break;
			}
			case LightType::Point:
			{
				std::string iteration = std::to_string(pointLightNum);
				mat->SetUniformData("u_PointLights[" + iteration + "].Color", light->color);
				mat->SetUniformData("u_PointLights[" + iteration + "].Intensity", light->intensity);
				mat->SetUniformData("u_PointLights[" + iteration + "].Position", (glm::vec3)transform->GetGlobalPosition());
				mat->SetUniformData("u_PointLights[" + iteration + "].Radius", light->radius);
				mat->SetUniformData("u_PointLights[" + iteration + "].Linear", light->linear);
				mat->SetUniformData("u_PointLights[" + iteration + "].Quadratic", light->quadratic);
				pointLightNum++;
				break;
			}
			case LightType::Spot:
			{
				std::string iteration = std::to_string(spotLightNum);
				mat->SetUniformData("u_SpotLights[" + iteration + "].Color", light->color);
				mat->SetUniformData("u_SpotLights[" + iteration + "].Intensity", light->intensity);
				mat->SetUniformData("u_SpotLights[" + iteration + "].Position", (glm::vec3)transform->GetGlobalPosition());
				mat->SetUniformData("u_SpotLights[" + iteration + "].Direction", (glm::vec3)transform->GetGlobalForward());
				mat->SetUniformData("u_SpotLights[" + iteration + "].Radius", light->radius);
				mat->SetUniformData("u_SpotLights[" + iteration + "].Linear", light->linear);
				mat->SetUniformData("u_SpotLights[" + iteration + "].Quadratic", light->quadratic);
				spotLightNum++;
				break;
			}
		}
	}

	mat->Bind();
}

// Init Shaders
void Renderer3D::InitPreLightingShader()
{
	ResourceId textShaderId = Resources::Load<Shader>("Assets/Shaders/MeshTexture");
	Shader* textShader = Resources::GetResourceById<Shader>(textShaderId);
	textShader->Compile("Assets/Shaders/MeshTexture");
	textShader->addUniform("diffuse", UniformType::Sampler2D);
	Resources::Import<Shader>("MeshTexture", textShader);

	ResourceId skeletalTextShaderId = Resources::Load<Shader>("Assets/Shaders/MeshTextureAnimated");
	Shader* skeletalTextShader = Resources::GetResourceById<Shader>(skeletalTextShaderId);
	skeletalTextShader->Compile("Assets/Shaders/MeshTextureAnimated");
	skeletalTextShader->addUniform("diffuse", UniformType::Sampler2D);
	Resources::Import<Shader>("MeshTextureAnimated", skeletalTextShader);

	ResourceId colorShaderId = Resources::Load<Shader>("Assets/Shaders/MeshColor");
	Shader* colorShader = Resources::GetResourceById<Shader>(colorShaderId);
	colorShader->Compile("Assets/Shaders/MeshColor");
	colorShader->addUniform("u_Color", UniformType::fVec4);
	Resources::Import<Shader>("MeshColor", colorShader);
}

void Renderer3D::InitPostLightingShader()
{
	ResourceId textShaderId = Resources::Load<Shader>("Assets/Shaders/PostLightingShader");
	Shader* textShader = Resources::GetResourceById<Shader>(textShaderId);
	textShader->Compile("Assets/Shaders/PostLightingShader");
	textShader->addUniform("gPosition", UniformType::Sampler2D);
	textShader->addUniform("gNormal", UniformType::Sampler2D);
	textShader->addUniform("gAlbedoSpec", UniformType::Sampler2D);
	textShader->addUniform("u_ViewPos", UniformType::fVec3);
	textShader->addUniform("u_TotalLightsNum", UniformType::Int);

	for (uint i = 0; i < 4; i++)
	{
		std::string iteration = std::to_string(i);
		textShader->addUniform("u_DirLight[" + iteration + "].Position", UniformType::fVec3);
		textShader->addUniform("u_DirLight[" + iteration + "].Color", UniformType::fVec3);
		textShader->addUniform("u_DirLight[" + iteration + "].Intensity", UniformType::Float);
		textShader->addUniform("u_DirLight[" + iteration + "].Direction", UniformType::fVec3);
		textShader->addUniform("u_DirLight[" + iteration + "].ViewProjectionMat;", UniformType::Mat4);
	}

	for (uint i = 0; i < 32; i++)
	{
		std::string iteration = std::to_string(i);
		textShader->addUniform("u_PointLights[" + iteration + "].Position", UniformType::fVec3);
		textShader->addUniform("u_PointLights[" + iteration + "].Color", UniformType::fVec3);
		textShader->addUniform("u_PointLights[" + iteration + "].Intensity", UniformType::Float);
		textShader->addUniform("u_PointLights[" + iteration + "].Linear", UniformType::Float);
		textShader->addUniform("u_PointLights[" + iteration + "].Quadratic", UniformType::Float);
		textShader->addUniform("u_PointLights[" + iteration + "].Radius", UniformType::Float);
	}

	for (uint i = 0; i < 16; i++)
	{
		std::string iteration = std::to_string(i);
		textShader->addUniform("u_SpotLights[" + iteration + "].Position", UniformType::fVec3);
		textShader->addUniform("u_SpotLights[" + iteration + "].Color", UniformType::fVec3);
		textShader->addUniform("u_SpotLights[" + iteration + "].Intensity", UniformType::Float);
		textShader->addUniform("u_SpotLights[" + iteration + "].Direction", UniformType::fVec3);
		textShader->addUniform("u_SpotLights[" + iteration + "].Linear", UniformType::Float);
		textShader->addUniform("u_SpotLights[" + iteration + "].Quadratic", UniformType::Float);
		textShader->addUniform("u_SpotLights[" + iteration + "].Radius", UniformType::Float);
		textShader->addUniform("u_SpotLights[" + iteration + "].CutOff", UniformType::Float);
		textShader->addUniform("u_SpotLights[" + iteration + "].OuterCutOff", UniformType::Float);
		textShader->addUniform("u_SpotLights[" + iteration + "].ViewProjectionMat", UniformType::Mat4);
		textShader->addUniform("u_SpotLights[" + iteration + "].Depth", UniformType::Sampler2D);
	}

	Resources::Import<Shader>("PostLightingShader", textShader);

	Material lightinProcessMat(textShader);
	lightinProcessMat.setShader(textShader, textShader->getPath());
	std::string lightingProcessPath = Resources::PathToLibrary<Material>() + "lightingProcess.toematerial";
	Resources::Import<Material>(lightingProcessPath, &lightinProcessMat);
	renderer3D.lightingMatID = Resources::LoadFromLibrary<Material>(lightingProcessPath);
}

void Renderer3D::InitIndexShaders()
{
	renderer3D.skeletalShaderID = Resources::Load<Shader>("Assets/Shaders/MeshTextureAnimatedIndex");
	Shader* skeletalTextShader = Resources::GetResourceById<Shader>(renderer3D.skeletalShaderID);
	skeletalTextShader->Compile("Assets/Shaders/MeshTextureAnimatedIndex");

	Resources::Import<Shader>("MeshTextureAnimatedIndex", skeletalTextShader);
}

void Renderer3D::InitCRTShader()
{
	ResourceId crtShaderId = Resources::Load<Shader>("Assets/Shaders/CRTShader");
	Shader* crtShader = Resources::GetResourceById<Shader>(crtShaderId);
	crtShader->Compile("Assets/Shaders/CRTShader");
	crtShader->addUniform("albedo", UniformType::Sampler2D);
	crtShader->addUniform("kEasing", UniformType::Float);
	crtShader->addUniform("time", UniformType::Float);
	crtShader->addUniform("warp", UniformType::fVec2);
	Resources::Import<Shader>("CRTShader", crtShader);

	Material crtMat(crtShader);
	crtMat.setShader(crtShader, crtShader->getPath());
	std::string CRTPath = Resources::PathToLibrary<Material>() + "CRTShader.toematerial";
	Resources::Import<Material>(CRTPath, &crtMat);
	renderer3D.crtMatID = Resources::LoadFromLibrary<Material>(CRTPath);
}
