#include "Mesh.h"
#include "EngineCore.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "Light.h"
#include "N_SceneManager.h"
#include "Log.h"
#include "Defs.h"

#include "Animation/OzzAnimation.h"
#include "../TheOneAnimation/samples/framework/internal/shader.h"

#include "ozz/geometry/runtime/skinning_job.h"
#include "ozz/base/maths/internal/simd_math_config.h"
#include "ozz/base/span.h"

#include <span>
#include <vector>
#include <array>
#include <cstdio>
#include <cassert>

static uint dynamicVAO = 0;
static uint dynamicVBO = 0;
static uint dynamicIBO = 0;

const uint8_t kDefaultColorsArray[][4] = {
	{255, 255, 255, 255}, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
	{ 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
	{ 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
	{ 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
	{ 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
	{ 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
	{ 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
	{ 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
	{ 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
	{ 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
	{ 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
	{ 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
	{ 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
	{ 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
	{ 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
	{ 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
	{ 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
	{ 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
	{ 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
	{ 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
	{ 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
	{ 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
	{ 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
	{ 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }
};

const float kDefaultNormalsArray[][3] = {
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f} };

const float kDefaultUVsArray[][2] = {
	{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f},
	{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f},
	{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f},
	{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f},
	{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f},
	{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f},
	{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f},
	{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f},
	{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f},
	{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f},
	{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f} };


Mesh::Mesh(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Mesh)
{
    active = true;
    drawNormalsFaces = false;
    drawNormalsVerts = false;
    drawAABB = true;
    drawChecker = false;

    normalLineWidth = 1;
    normalLineLength = 0.1f;

	if (dynamicVAO == 0) {
		GLCALL(glGenVertexArrays(1, &dynamicVAO));
	}

	if (dynamicVBO == 0) {
		GLCALL(glGenBuffers(1, &dynamicVBO));
	}

	if (dynamicIBO == 0) {
		GLCALL(glGenBuffers(1, &dynamicIBO));
	}
}

Mesh::Mesh(std::shared_ptr<GameObject> containerGO, Mesh* ref) : Component(containerGO, ComponentType::Mesh)
{
    active = ref->active;
    drawNormalsFaces = ref->drawNormalsFaces;
    drawNormalsVerts = ref->drawNormalsVerts;
    drawAABB = ref->drawAABB;
    drawChecker = ref->drawChecker;

    meshID = ref->meshID;
    materialID = ref->materialID;

    normalLineWidth = ref->normalLineWidth;
    normalLineLength = ref->normalLineLength;
}

Mesh::~Mesh()
{
	if (dynamicVAO) {
		GLCALL(glDeleteVertexArrays(1, &dynamicVAO));
		dynamicVAO = 0;
	}

	if (dynamicVBO) {
		GLCALL(glDeleteBuffers(1, &dynamicVBO));
		dynamicVBO = 0;
	}

	if (dynamicIBO) {
		GLCALL(glDeleteBuffers(1, &dynamicIBO));
		dynamicIBO = 0;
	}
}


// Draw
void Mesh::DrawComponent(Camera* camera)
{
    std::shared_ptr<GameObject> containerGO = GetContainerGO();

    if (!active || meshID == -1)
        return;

	Material* mat;
	if(materialID == -1)
		mat = Resources::GetResourceById<Material>(0);
	else
		mat = Resources::GetResourceById<Material>(materialID);

    Model* mesh = Resources::GetResourceById<Model>(meshID);
	mat4 transform = containerGO.get()->GetComponent<Transform>()->CalculateWorldTransform();

	if (mesh->isAnimated())
		RenderOzzSkinnedMesh(mesh, mat, ozz::make_span(mesh->getSkinningMatrices()), transform);
	else
		RenderMesh(mesh, mat, transform);

    /*if (drawNormalsVerts) DrawVertexNormals();
    if (drawNormalsFaces) DrawFaceNormals();*/
}

bool Mesh::RenderMesh(Model* mesh, Material* material, const mat4& transform)
{

	Shader* matShader = material->getShader();
	matShader->Bind();
	matShader->SetModel(transform);

	SetUpLight(material);

	material->Bind();

	if (drawWireframe)
	{
		GLCALL(glDisable(GL_TEXTURE_2D));
		GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
	}
	else
	{
		GLCALL(glEnable(GL_TEXTURE_2D));
		GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
	}

	mesh->Render();

	material->UnBind();
	return true;
}

//void Mesh::DrawVertexNormals()
//{
//    if (meshData.meshVerts.empty() || meshData.meshNorms.empty()) return;
//
//    GLCALL(glLineWidth(normalLineWidth);
//    GLCALL(glColor3f(1.0f, 1.0f, 0.0f);
//    GLCALL(glBegin(GL_LINES);
//
//    for (int i = 0; i < meshData.meshVerts.size(); i++)
//    {
//        GLCALL(glVertex3f(meshData.meshVerts[i].x, meshData.meshVerts[i].y, meshData.meshVerts[i].z);
//        GLCALL(glVertex3f(meshData.meshVerts[i].x + meshData.meshNorms[i].x * normalLineLength,
//            meshData.meshVerts[i].y + meshData.meshNorms[i].y * normalLineLength,
//            meshData.meshVerts[i].z + meshData.meshNorms[i].z * normalLineLength);
//    }
//
//    GLCALL(glColor3f(1.0f, 1.0f, 0.0f);
//    GLCALL(glEnd();
//}

//void Mesh::DrawFaceNormals() 
//{
//    if (meshData.meshFaceCenters.empty() || meshData.meshFaceNorms.empty()) return;
//
//    GLCALL(glLineWidth(normalLineWidth);
//    GLCALL(glColor3f(1.0f, 0.0f, 1.0f);
//    GLCALL(glBegin(GL_LINES);
//
//    for (int i = 0; i < meshData.meshFaceCenters.size(); i++)
//    {
//        glm::vec3 endPoint = meshData.meshFaceCenters[i] + normalLineLength * meshData.meshFaceNorms[i];
//        GLCALL(glVertex3f(meshData.meshFaceCenters[i].x, meshData.meshFaceCenters[i].y, meshData.meshFaceCenters[i].z);
//        GLCALL(glVertex3f(endPoint.x, endPoint.y, endPoint.z);
//    }
//
//    GLCALL(glColor3f(0.0f, 1.0f, 1.0f);
//    GLCALL(glEnd();
//}


bool Mesh::RenderOzzSkinnedMesh(Model* mesh, Material* material, const ozz::span<ozz::math::Float4x4> skinningMatrices, const mat4& transform)
{
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
			return false;
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

	Shader* animShader = material->getShader();

	/*RenderShadowsOzz(
		camera,
		ozzMesh,
		animShader,
		transform,
		vboSize,
		vboMap
	);*/

	//LightManager& lman = Wiwa::SceneManager::getActiveScene()->GetLightManager();

	animShader->Bind();
	animShader->SetModel(transform);

	GLCALL(glBindVertexArray(dynamicVAO));
	// Updates dynamic vertex buffer with skinned data.
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, dynamicVBO));
	GLCALL(glBufferData(GL_ARRAY_BUFFER, vboSize, vboMap, GL_STREAM_DRAW));
	//GLCALL(glBufferSubData(GL_ARRAY_BUFFER, 0, vboSize, vboMap));

	SetUpLight(material);
	/*SetUpLight(animShader, camera, lman.GetDirectionalLight(), lman.GetPointLights(), lman.GetSpotLights());
	camera->shadowBuffer->BindTexture();*/

	material->Bind();

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
	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dynamicIBO));
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

	//camera->shadowBuffer->UnbindTexture();
	material->UnBind();

	return true;
}


// Load/Save
json Mesh::SaveComponent()
{
    json meshJSON;

    meshJSON["Name"] = name;
    meshJSON["Type"] = type;
    if (auto pGO = containerGO.lock())
    {
        meshJSON["ParentUID"] = pGO.get()->GetUID();
    }
    meshJSON["UID"] = UID;
    meshJSON["Active"] = active;
    meshJSON["DrawWireframe"] = drawWireframe;
    meshJSON["DrawAABB"] = drawAABB;
    meshJSON["DrawOBB"] = drawOBB;
    meshJSON["DrawChecker"] = drawChecker;
    meshJSON["DrawNormalsVerts"] = drawNormalsVerts;
    meshJSON["DrawNormalsFaces"] = drawNormalsFaces;

    Model* mesh = Resources::GetResourceById<Model>(meshID);
    meshJSON["MeshPath"] = mesh->GetMeshPath();

    Material* mat = Resources::GetResourceById<Material>(materialID);
    std::string matPath = mat->getPath();
    meshJSON["MaterialPath"] = matPath;

    return meshJSON;
}

void Mesh::LoadComponent(const json& meshJSON)
{
    // Load basic properties
    if (meshJSON.contains("UID"))
    {
        UID = meshJSON["UID"];
    }

    if (meshJSON.contains("Name"))
    {
        name = meshJSON["Name"];
    }

    // Load mesh-specific properties
    if (meshJSON.contains("Active"))
    {
        active = meshJSON["Active"];
    }

    if (meshJSON.contains("DrawWireframe"))
    {
        drawWireframe = meshJSON["DrawWireframe"];
    }

    if (meshJSON.contains("DrawAABB"))
    {
        drawAABB = meshJSON["DrawAABB"];
    }

    if (meshJSON.contains("DrawOBB"))
    {
        drawOBB = meshJSON["DrawOBB"];
    }

    if (meshJSON.contains("DrawChecker"))
    {
        drawChecker = meshJSON["DrawChecker"];
    }

    if (meshJSON.contains("DrawNormalsVerts"))
    {
        drawNormalsVerts = meshJSON["DrawNormalsVerts"];
    }

    if (meshJSON.contains("DrawNormalsFaces"))
    {
        drawNormalsFaces = meshJSON["DrawNormalsFaces"];
    }

    if (meshJSON.contains("Path"))
    {
        std::filesystem::path legacyPath = meshJSON["Path"];
        if (!legacyPath.string().empty())
        {
            std::filesystem::path assetsMesh = Resources::FindFileInAssets(legacyPath.parent_path().filename().string());
            Resources::LoadMultiple<Model>(assetsMesh.replace_extension(".fbx").string());
            meshID = Resources::LoadFromLibrary<Model>(legacyPath.string());
            Model* model = Resources::GetResourceById<Model>(meshID);
            materialID = Resources::LoadFromLibrary<Material>(model->GetMaterialPath());
        }
    }

    if (meshJSON.contains("MeshPath"))
    {
        std::string meshPath = meshJSON["MeshPath"];
        if (Resources::FindFileInLibrary(meshPath).empty())
        {
            std::filesystem::path libraryToAssets = meshPath;
            std::string assetsMesh = Resources::FindFileInAssets(libraryToAssets.parent_path().filename().string());
            Resources::LoadMultiple<Model>(assetsMesh);
            meshID = Resources::LoadFromLibrary<Model>(meshPath);
        }
        else
        {
            meshID = Resources::LoadFromLibrary<Model>(meshPath);
        }
    }

    if (meshJSON.contains("MaterialPath"))
    {
        materialID = Resources::LoadFromLibrary<Material>(meshJSON["MaterialPath"]);
    }
}

bool Mesh::SetUpLight(Material* material)
{
	bool ret = true;

	std::vector<Light*> pointLights = engine->N_sceneManager->currentScene->pointLights;

	Transform* cameraTransform = engine->N_sceneManager->currentScene->currentCamera->GetContainerGO().get()->GetComponent<Transform>();
	
	//Variables need to be float not double
	material->SetUniformData("u_ViewPos", (glm::vec3)cameraTransform->GetPosition());
	material->SetUniformData("u_PointLightsNum", pointLights.size());
	for (int i = 0; i < pointLights.size(); i++)
	{
		string iteration = to_string(i);

		//Variables need to be float not double
		material->SetUniformData("u_PointLights[" + iteration + "].position", (glm::vec3)pointLights[i]->GetContainerGO().get()->GetComponent<Transform>()->GetPosition());
		material->SetUniformData("u_PointLights[" + iteration + "].constant", 1.0f);
		material->SetUniformData("u_PointLights[" + iteration + "].linear", 0.3f);
		material->SetUniformData("u_PointLights[" + iteration + "].quadratic", 0.3f);
		material->SetUniformData("u_PointLights[" + iteration + "].ambient", pointLights[i]->color * 0.1f);
		material->SetUniformData("u_PointLights[" + iteration + "].diffuse", pointLights[i]->color);
		material->SetUniformData("u_PointLights[" + iteration + "].specular", pointLights[i]->specular);
	}

	return ret;
}