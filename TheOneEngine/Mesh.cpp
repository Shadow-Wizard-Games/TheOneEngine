#include "Mesh.h"
#include "EngineCore.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "Log.h"
#include "Defs.h"

#include "../TheOneEngine/Animation/samples/shader.h"
#include "../external/ozz/include/geometry/runtime/skinning_job.h"
#include "../external/ozz/include/base/maths/internal/simd_math_config.h"

#include <span>
#include <vector>
#include <array>
#include <cstdio>
#include <cassert>


Mesh::Mesh(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Mesh)
{
    active = true;
    drawNormalsFaces = false;
    drawNormalsVerts = false;
    drawAABB = true;
    drawChecker = false;

    normalLineWidth = 1;
    normalLineLength = 0.1f;
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

Mesh::~Mesh() {}


// Draw
void Mesh::DrawComponent(Camera* camera)
{
    std::shared_ptr<GameObject> containerGO = GetContainerGO();

    if (meshID == -1 || materialID == -1)
        return;

    Model* mesh = Resources::GetResourceById<Model>(meshID);
    Material* mat = Resources::GetResourceById<Material>(materialID);

    Shader* matShader = mat->getShader();
    matShader->Bind();
    matShader->SetModel(containerGO.get()->GetComponent<Transform>()->CalculateWorldTransform());

    mat->Bind();

    if (!active)
        return;

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

    mat->UnBind();

    /*if (drawNormalsVerts) DrawVertexNormals();
    if (drawNormalsFaces) DrawFaceNormals();*/
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


bool Mesh::RenderOzzSkinnedMesh(Camera* camera, const ozz::sample::Mesh& _mesh, Material* material, const ozz::span<ozz::math::Float4x4> _skinning_matrices, const ozz::math::Float4x4& _transform)
{
	const int vertex_count = _mesh.vertex_count();

	// Positions and normals are interleaved to improve caching while executing
	// skinning job.

	const GLsizei positions_offset = 0;
	const GLsizei positions_stride = sizeof(float) * 3;
	const GLsizei normals_offset = vertex_count * positions_stride;
	const GLsizei normals_stride = sizeof(float) * 3;
	const GLsizei tangents_offset =
		normals_offset + vertex_count * normals_stride;
	const GLsizei tangents_stride = sizeof(float) * 3;
	const GLsizei skinned_data_size =
		tangents_offset + vertex_count * tangents_stride;

	// Colors and uvs are contiguous. They aren't transformed, so they can be
	// directly copied from source mesh which is non-interleaved as-well.
	// Colors will be filled with white if _options.colors is false.
	// UVs will be skipped if _options.textured is false.
	const GLsizei colors_offset = skinned_data_size;
	const GLsizei colors_stride = sizeof(uint8_t) * 4;
	const GLsizei colors_size = vertex_count * colors_stride;
	const GLsizei uvs_offset = colors_offset + colors_size;
	const GLsizei uvs_stride = sizeof(float) * 2;
	const GLsizei uvs_size = vertex_count * uvs_stride;
	const GLsizei fixed_data_size = colors_size + uvs_size;

	// Reallocate vertex buffer.
	const GLsizei vbo_size = skinned_data_size + fixed_data_size;

	void* vbo_map = engine->scratch_buffer_.Resize(vbo_size);

	// Iterate mesh parts and fills vbo.
	// Runs a skinning job per mesh part. Triangle indices are shared
	// across parts.
	size_t processed_vertex_count = 0;
	for (size_t i = 0; i < _mesh.parts.size(); ++i) {
		const ozz::sample::Mesh::Part& part = _mesh.parts[i];

		// Skip this iteration if no vertex.
		const size_t part_vertex_count = part.positions.size() / 3;
		if (part_vertex_count == 0) {
			continue;
		}

		// Fills the job.
		ozz::geometry::SkinningJob skinning_job;
		skinning_job.vertex_count = static_cast<int>(part_vertex_count);
		const int part_influences_count = part.influences_count();

		// Clamps joints influence count according to the option.
		skinning_job.influences_count = part_influences_count;

		// Setup skinning matrices, that came from the animation stage before being
		// multiplied by inverse model-space bind-pose.
		skinning_job.joint_matrices = _skinning_matrices;

		// Setup joint's indices.
		skinning_job.joint_indices = make_span(part.joint_indices);
		skinning_job.joint_indices_stride =
			sizeof(uint16_t) * part_influences_count;

		// Setup joint's weights.
		if (part_influences_count > 1) {
			skinning_job.joint_weights = make_span(part.joint_weights);
			skinning_job.joint_weights_stride =
				sizeof(float) * (part_influences_count - 1);
		}

		// Setup input positions, coming from the loaded mesh.
		skinning_job.in_positions = make_span(part.positions);
		skinning_job.in_positions_stride =
			sizeof(float) * ozz::sample::Mesh::Part::kPositionsCpnts;

		// Setup output positions, coming from the rendering output mesh buffers.
		// We need to offset the buffer every loop.
		float* out_positions_begin = reinterpret_cast<float*>(ozz::PointerStride(
			vbo_map, positions_offset + processed_vertex_count * positions_stride));
		float* out_positions_end = ozz::PointerStride(
			out_positions_begin, part_vertex_count * positions_stride);
		skinning_job.out_positions = { out_positions_begin, out_positions_end };
		skinning_job.out_positions_stride = positions_stride;

		// Setup normals if input are provided.
		float* out_normal_begin = reinterpret_cast<float*>(ozz::PointerStride(
			vbo_map, normals_offset + processed_vertex_count * normals_stride));
		float* out_normal_end = ozz::PointerStride(
			out_normal_begin, part_vertex_count * normals_stride);

		if (part.normals.size() / ozz::sample::Mesh::Part::kNormalsCpnts ==
			part_vertex_count) {
			// Setup input normals, coming from the loaded mesh.
			skinning_job.in_normals = make_span(part.normals);
			skinning_job.in_normals_stride =
				sizeof(float) * ozz::sample::Mesh::Part::kNormalsCpnts;

			// Setup output normals, coming from the rendering output mesh buffers.
			// We need to offset the buffer every loop.
			skinning_job.out_normals = { out_normal_begin, out_normal_end };
			skinning_job.out_normals_stride = normals_stride;
		}
		else {
			// Fills output with default normals.
			for (float* normal = out_normal_begin; normal < out_normal_end;
				normal = ozz::PointerStride(normal, normals_stride)) {
				normal[0] = 0.f;
				normal[1] = 1.f;
				normal[2] = 0.f;
			}
		}

		// Setup tangents if input are provided.
		float* out_tangent_begin = reinterpret_cast<float*>(ozz::PointerStride(
			vbo_map, tangents_offset + processed_vertex_count * tangents_stride));
		float* out_tangent_end = ozz::PointerStride(
			out_tangent_begin, part_vertex_count * tangents_stride);

		if (part.tangents.size() / ozz::sample::Mesh::Part::kTangentsCpnts ==
			part_vertex_count) {
			// Setup input tangents, coming from the loaded mesh.
			skinning_job.in_tangents = make_span(part.tangents);
			skinning_job.in_tangents_stride =
				sizeof(float) * ozz::sample::Mesh::Part::kTangentsCpnts;

			// Setup output tangents, coming from the rendering output mesh buffers.
			// We need to offset the buffer every loop.
			skinning_job.out_tangents = { out_tangent_begin, out_tangent_end };
			skinning_job.out_tangents_stride = tangents_stride;
		}
		else {
			// Fills output with default tangents.
			for (float* tangent = out_tangent_begin; tangent < out_tangent_end;
				tangent = ozz::PointerStride(tangent, tangents_stride)) {
				tangent[0] = 1.f;
				tangent[1] = 0.f;
				tangent[2] = 0.f;
			}
		}

		// Execute the job, which should succeed unless a parameter is invalid.
		if (!skinning_job.Run()) {
			return false;
		}

		// OZZ COLORS
		// Un-optimal path used when the right number of colors is not provided.
		static_assert(sizeof(kDefaultColorsArray[0]) == colors_stride,
			"Stride mismatch");

		for (size_t j = 0; j < part_vertex_count;
			j += OZZ_ARRAY_SIZE(kDefaultColorsArray)) {
			const size_t this_loop_count = ozz::math::Min(
				OZZ_ARRAY_SIZE(kDefaultColorsArray), part_vertex_count - j);
			memcpy(ozz::PointerStride(
				vbo_map, colors_offset +
				(processed_vertex_count + j) * colors_stride),
				kDefaultColorsArray, colors_stride * this_loop_count);
		}

		if (part_vertex_count ==
			part.uvs.size() / ozz::sample::Mesh::Part::kUVsCpnts) {
			// Optimal path used when the right number of uvs is provided.
			memcpy(ozz::PointerStride(
				vbo_map, uvs_offset + processed_vertex_count * uvs_stride),
				array_begin(part.uvs), part_vertex_count * uvs_stride);
		}
		else {
			// Un-optimal path used when the right number of uvs is not provided.
			assert(sizeof(kDefaultUVsArray[0]) == uvs_stride);
			for (size_t j = 0; j < part_vertex_count;
				j += OZZ_ARRAY_SIZE(kDefaultUVsArray)) {
				const size_t this_loop_count = ozz::math::Min(
					OZZ_ARRAY_SIZE(kDefaultUVsArray), part_vertex_count - j);
				memcpy(ozz::PointerStride(
					vbo_map,
					uvs_offset + (processed_vertex_count + j) * uvs_stride),
					kDefaultUVsArray, uvs_stride * this_loop_count);
			}
		}

		// Some more vertices were processed.
		processed_vertex_count += part_vertex_count;
	}


	// ========================= RENDERING =====================================

	// Build mvp for object
	glm::mat4 glm_mvp = camera->projectionMatrix * camera->viewMatrix;
	glm::mat4 transform_mat = glm::mat4(1.f);

	ozz::math::Float4x4 ozz_mvp;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			// m128_f32 for float4x4
			ozz_mvp.cols[i].m128_f32[j] = glm_mvp[i][j];
			transform_mat[i][j] = _transform.cols[i].m128_f32[j];
		}
	}


	Shader* anim_shader = material->getShader();

	/*RenderShadowsOzz(
		camera,
		_mesh,
		anim_shader,
		transform_mat,
		vbo_size,
		vbo_map
	);*/

	//LightManager& lman = Wiwa::SceneManager::getActiveScene()->GetLightManager();

	// After processing everything, render
	GL(Viewport(0, 0, scene->frameBuffer->getWidth(), camera->frameBuffer->getHeight()));

	camera->frameBuffer->Bind(false);
	anim_shader->Bind();
	GL(BindVertexArray(dynamic_vao_));
	// Updates dynamic vertex buffer with skinned data.
	GL(BindBuffer(GL_ARRAY_BUFFER, dynamic_array_bo_));
	GL(BufferData(GL_ARRAY_BUFFER, vbo_size, nullptr, GL_STREAM_DRAW));
	GL(BufferSubData(GL_ARRAY_BUFFER, 0, vbo_size, vbo_map));



	SetUpLight(anim_shader, camera, lman.GetDirectionalLight(), lman.GetPointLights(), lman.GetSpotLights());
	camera->shadowBuffer->BindTexture();

	material->Bind(GL_TEXTURE1);

	GL(EnableVertexAttribArray(0));
	GL(VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, uvs_stride,
		GL_PTR_OFFSET(uvs_offset)));

	GL(EnableVertexAttribArray(1));
	GL(VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, positions_stride,
		GL_PTR_OFFSET(positions_offset)));

	GL(EnableVertexAttribArray(2));
	GL(VertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, normals_stride,
		GL_PTR_OFFSET(normals_offset)));

	GL(EnableVertexAttribArray(3));
	GL(VertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE,
		colors_stride, GL_PTR_OFFSET(colors_offset)));

	// Binds mw uniform
	float values[16];
	const GLint mw_uniform = GL(GetUniformLocation(anim_shader->getID(), "u_mw"));//uniform(0);
	ozz::math::StorePtrU(_transform.cols[0], values + 0);
	ozz::math::StorePtrU(_transform.cols[1], values + 4);
	ozz::math::StorePtrU(_transform.cols[2], values + 8);
	ozz::math::StorePtrU(_transform.cols[3], values + 12);
	GL(UniformMatrix4fv(mw_uniform, 1, false, values));

	// Binds mvp uniform
	const GLint mvp_uniform = GL(GetUniformLocation(anim_shader->getID(), "u_mvp"));//uniform(1);
	ozz::math::StorePtrU(ozz_mvp.cols[0], values + 0);
	ozz::math::StorePtrU(ozz_mvp.cols[1], values + 4);
	ozz::math::StorePtrU(ozz_mvp.cols[2], values + 8);
	ozz::math::StorePtrU(ozz_mvp.cols[3], values + 12);
	GL(UniformMatrix4fv(mvp_uniform, 1, false, values));

	// Maps the index dynamic buffer and update it.
	GL(BindBuffer(GL_ELEMENT_ARRAY_BUFFER, dynamic_index_bo_));
	const ozz::sample::Mesh::TriangleIndices& indices = _mesh.triangle_indices;
	GL(BufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(ozz::sample::Mesh::TriangleIndices::value_type),
		array_begin(indices), GL_STREAM_DRAW));

	// Draws the mesh.
	static_assert(sizeof(ozz::sample::Mesh::TriangleIndices::value_type) == 2,
		"Expects 2 bytes indices.");
	GL(DrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()),
		GL_UNSIGNED_SHORT, 0));

	// Unbinds.
	GL(BindBuffer(GL_ARRAY_BUFFER, 0));
	GL(BindTexture(GL_TEXTURE_2D, 0));
	GL(BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

	GL(DisableVertexAttribArray(0));
	GL(DisableVertexAttribArray(1));
	GL(DisableVertexAttribArray(2));
	GL(DisableVertexAttribArray(3));

	GL(BindVertexArray(0));

	camera->shadowBuffer->UnbindTexture();
	material->UnBind(GL_TEXTURE1);

	camera->frameBuffer->Unbind();

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
    meshJSON["MeshPath"] = mesh->path;
    Material* mat = Resources::GetResourceById<Material>(materialID);
    meshJSON["MaterialPath"] = mat->getPath();

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