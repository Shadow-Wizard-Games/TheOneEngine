//#include <wipch.h>
#include "OzzAnimator.h"

#include "animation/runtime/animation.h"
#include "animation/runtime/blending_job.h"
#include "animation/runtime/local_to_model_job.h"
#include "animation/runtime/sampling_job.h"
#include "animation/runtime/skeleton.h"
#include "animation/runtime/skeleton_utils.h"
#include "base/containers/vector.h"
#include "base/log.h"
#include "base/maths/simd_math.h"
#include "base/maths/soa_transform.h"
#include "base/maths/vec_float.h"
#include "options/options.h"

#include "animations/OzzAnimationSimple.h"

#include <Wiwa/utilities/json/JSONDocument.h>
//#include <Wiwa/core/Application.h>

#include "..\Defs.h"


size_t OzzAnimator::_create_anim_impl()
{
	size_t index = 0;

	if (m_RemovedAnimationsIndex.size() > 0)
	{
		index = m_RemovedAnimationsIndex[m_RemovedAnimationsIndex.size() - 1];
		m_RemovedAnimationsIndex.pop_back();
	}
	else
	{
		index = m_AnimationList.size();
		m_AnimationList.emplace_back();
	}

	return index;
}

bool OzzAnimator::_update(float _dt)
{
	AnimationData& a_data = m_AnimationList[m_ActiveAnimationId];

	OzzAnimation* animation = a_data.animation;

	if (!animation->Update(_dt)) return false;

	ozz::vector<ozz::math::SoaTransform>& locals = animation->getLocals();

	// Converts from local space to model space matrices.
	// Gets the output of the blending stage, and converts it to model space.

	// Setup local-to-model conversion job.
	ozz::animation::LocalToModelJob ltm_job;
	ltm_job.skeleton = &m_Skeleton;
	ltm_job.input = make_span(locals);
	ltm_job.output = make_span(models_);

	// Run ltm job.
	if (!ltm_job.Run())
		return false;

	// Build skinning matrices
	for (size_t i = 0; i < m_Mesh.joint_remaps.size(); ++i)
		skinning_matrices_[i] = models_[m_Mesh.joint_remaps[i]] * m_Mesh.inverse_bind_poses[i];

	return true;
}

bool OzzAnimator::_update_blend(float _dt)
{
	float weight_ratio = m_TransitionTimer / m_TransitionTime;

	AnimationData& a_data = m_AnimationList[m_ActiveAnimationId];
	AnimationData& pa_data = m_AnimationList[m_PrevAnimationId];

	OzzAnimation* animation = a_data.animation;
	OzzAnimation* prevanimation = pa_data.animation;

	if (!animation->Update(_dt)) return false;
	if (!prevanimation->Update(_dt)) return false;

	ozz::vector<ozz::math::SoaTransform>& locals = animation->getLocals();
	ozz::vector<ozz::math::SoaTransform>& prevlocals = prevanimation->getLocals();

	// Prepares blending layers.
	ozz::animation::BlendingJob::Layer layers[2];
	layers[0].transform = make_span(locals);
	layers[0].weight = weight_ratio;

	layers[1].transform = make_span(prevlocals);
	layers[1].weight = 1.0f - weight_ratio;

	// Setups blending job.
	ozz::animation::BlendingJob blend_job;
	blend_job.threshold = m_BlendThreshold;
	blend_job.layers = layers;
	blend_job.rest_pose = m_Skeleton.joint_rest_poses();
	blend_job.output = make_span(blended_locals_);

	// Blends.
	if (!blend_job.Run())
		return false;

	// Setup local-to-model conversion job.
	ozz::animation::LocalToModelJob ltm_job;
	ltm_job.skeleton = &m_Skeleton;
	ltm_job.input = make_span(blended_locals_);
	ltm_job.output = make_span(models_);

	// Run ltm job.
	if (!ltm_job.Run())
		return false;

	// Build skinning matrices
	for (size_t i = 0; i < m_Mesh.joint_remaps.size(); ++i)
		skinning_matrices_[i] = models_[m_Mesh.joint_remaps[i]] * m_Mesh.inverse_bind_poses[i];

	m_TransitionTimer += _dt;

	if (m_TransitionTimer >= m_TransitionTime)
	{
		m_UpdateState = US_UPDATE;
		m_TransitionTimer = 0.0f;
	}

	return true;
}

OzzAnimator::OzzAnimator() :
	m_ActiveAnimationId(INVALID_INDEX),
	m_PrevAnimationId(INVALID_INDEX),
	m_LoadedMesh(false),
	m_LoadedSkeleton(false),
	m_LoadedMaterial(false),
	m_MaterialId(INVALID_INDEX),
	m_BlendOnTransition(true),
	m_BlendThreshold(ozz::animation::BlendingJob().threshold),
	m_TransitionTime(1.0f),
	m_TransitionTimer(0.0f),
	m_UpdateState(US_UPDATE)
{}

OzzAnimator::~OzzAnimator()
{
	size_t a_size = m_AnimationList.size();

	for (size_t i = 0; i < a_size; i++)
	{
		OzzAnimation* anim = m_AnimationList[i].animation;
		delete anim;
	}
}

ozz::span<const char* const> OzzAnimator::getSkeletonBoneNames()
{
	if (!m_LoadedSkeleton)
		return ozz::span<const char* const>();

	return m_Skeleton.joint_names();
}

bool OzzAnimator::LoadMesh(const std::string& path)
{
	m_LoadedMesh = ozz::sample::LoadMesh(path.c_str(), &m_Mesh);

	if (m_LoadedMesh)
	{
		m_MeshPath = path;

		size_t num_skinning_matrices = 0;
		num_skinning_matrices = ozz::math::Max(num_skinning_matrices, m_Mesh.joint_remaps.size());

		// Allocates skinning matrices.
		skinning_matrices_.resize(num_skinning_matrices);

		for (size_t i = 0; i < m_Mesh.joint_remaps.size(); ++i)
			skinning_matrices_[i] = ozz::math::Float4x4::identity();
	}

	return m_LoadedMesh;
}

bool OzzAnimator::LoadSkeleton(const std::string& path)
{
	m_LoadedSkeleton = ozz::sample::LoadSkeleton(path.c_str(), &m_Skeleton);

	if (m_LoadedSkeleton)
	{
		m_SkeletonPath = path;

		// Allocates runtime buffers.
		const int num_joints = m_Skeleton.num_joints();
		const int num_soa_joints = m_Skeleton.num_soa_joints();

		models_.resize(num_joints);
		// Allocates local space runtime buffers of blended data.
		blended_locals_.resize(num_soa_joints);
	}

	return m_LoadedSkeleton;
}

bool OzzAnimator::LoadMaterial(const std::string& path)
{
	m_MaterialId = Resources::Load<Material>(path.c_str());

	if (m_MaterialId == INVALID_INDEX) return false;

	m_MaterialPath = path;
	m_LoadedMaterial = true;

	return true;
}

size_t OzzAnimator::CreateSimpleAnimation(const std::string& name)
{
	// Return NULL if name already in list
	if (HasAnimation(name)) return INVALID_INDEX;

	size_t anim_id = _create_anim_impl();

	// Add index to map
	m_AnimationsIndex[name] = anim_id;

	// Create partial animation
	OzzAnimationSimple* simple_anim = new OzzAnimationSimple();
	simple_anim->SetSkeleton(&m_Skeleton);

	AnimationData& a_data = m_AnimationList[anim_id];

	a_data.name = name;
	a_data.animation = simple_anim;

	return anim_id;
}

size_t OzzAnimator::CreatePartialAnimation(const std::string& name)
{
	// Return NULL if name already in list
	if (HasAnimation(name)) return INVALID_INDEX;

	size_t anim_id = _create_anim_impl();

	// Add index to map
	m_AnimationsIndex[name] = anim_id;

	// Create partial animation
	OzzAnimationPartialBlending* partial_anim = new OzzAnimationPartialBlending();
	partial_anim->SetSkeleton(&m_Skeleton);

	AnimationData& a_data = m_AnimationList[anim_id];

	a_data.name = name;
	a_data.animation = partial_anim;

	return anim_id;
}

void OzzAnimator::RemoveAnimationAt(size_t index)
{
	AnimationData& a_data = m_AnimationList[index];

	m_RemovedAnimationsIndex.push_back(index);
	m_AnimationsIndex.erase(a_data.name);

	a_data.name = "Removed";
	delete a_data.animation;
	a_data.animation = nullptr;

	if (m_ActiveAnimationId == index)
	{
		m_ActiveAnimationId = INVALID_INDEX;
		m_ActiveAnimationName = "";
	}
}

bool OzzAnimator::HasAnimation(const std::string& name)
{
	return m_AnimationsIndex.find(name) != m_AnimationsIndex.end();
}

OzzAnimator::AnimationData* OzzAnimator::PlayAnimation(const std::string& name, float time_ratio)
{
	std::unordered_map<std::string, size_t>::iterator it = m_AnimationsIndex.find(name);

	if (it != m_AnimationsIndex.end())
		return PlayAnimation(it->second, time_ratio);

	return nullptr;
}

OzzAnimator::AnimationData* OzzAnimator::PlayAnimation(size_t anim_id, float time_ratio)
{
	if (anim_id >= 0 && anim_id < m_AnimationList.size())
	{
		AnimationData& a_data = m_AnimationList[anim_id];
		a_data.animation->Play();

		if (anim_id == m_ActiveAnimationId)
		{
			if (!a_data.animation->HasFinished() || a_data.animation->getLoop())
				return &a_data;
		}

		a_data.animation->setTimeRatio(time_ratio);

		if (a_data.animation->getStatus() == OzzAnimation::Status::VALID)
		{
			if (m_ActiveAnimationId != INVALID_INDEX)
			{
				if (m_BlendOnTransition)
				{
					m_UpdateState = US_BLEND;
					m_PrevAnimationId = m_ActiveAnimationId;
				}
			}

			m_ActiveAnimationId = anim_id;
			m_ActiveAnimationName = a_data.name;

			return &m_AnimationList[m_ActiveAnimationId];
		}

		return nullptr;
	}

	return nullptr;
}

void OzzAnimator::StopAnimation(bool unbind)
{
	if (m_ActiveAnimationId != INVALID_INDEX)
	{
		AnimationData& a_data = m_AnimationList[m_ActiveAnimationId];

		a_data.animation->Stop();

		if (unbind)
		{
			m_ActiveAnimationId = INVALID_INDEX;
			m_ActiveAnimationName = "";
		}
	}
}

OzzAnimation* OzzAnimator::getAnimationByName(const std::string& name)
{
	std::unordered_map<std::string, size_t>::iterator it = m_AnimationsIndex.find(name);

	if (it != m_AnimationsIndex.end()) 
		return m_AnimationList[it->second].animation;

	return nullptr;
}

size_t OzzAnimator::getAnimationIndex(const std::string& str)
{
	std::unordered_map<std::string, size_t>::iterator it = m_AnimationsIndex.find(str);

	if (it != m_AnimationsIndex.end())
		return it->second;

	return INVALID_INDEX;
}

OzzAnimation* OzzAnimator::getActiveAnimation()
{
	OzzAnimation* anim = nullptr;

	if (m_ActiveAnimationId != INVALID_INDEX)
		anim = m_AnimationList[m_ActiveAnimationId].animation;

	return anim;
}

void OzzAnimator::setUpperPlaybackSpeed(size_t anim_id, float playback_speed)
{
	if (anim_id < 0 || anim_id >= m_AnimationList.size()) return;

	AnimationData& a_data = m_AnimationList[anim_id];

	if (!a_data.animation) return;

	if (a_data.animation->getAnimationType() == AnimationType::AT_PARTIAL_BLEND)
	{
		OzzAnimationPartialBlending* a_partial = (OzzAnimationPartialBlending*)a_data.animation;
		a_partial->setUpperPlaybackSpeed(playback_speed);
	}
}

void OzzAnimator::setLowerPlaybackSpeed(size_t anim_id, float playback_speed)
{
	if (anim_id < 0 || anim_id >= m_AnimationList.size()) return;

	AnimationData& a_data = m_AnimationList[anim_id];

	if (!a_data.animation) return;

	if (a_data.animation->getAnimationType() == AnimationType::AT_PARTIAL_BLEND)
	{
		OzzAnimationPartialBlending* a_partial = (OzzAnimationPartialBlending*)a_data.animation;
		a_partial->setLowerPlaybackSpeed(playback_speed);
	}
}

void OzzAnimator::Init()
{

}

bool OzzAnimator::Update(float _dt)
{
	if (m_ActiveAnimationId != INVALID_INDEX)
	{
		switch (m_UpdateState)
		{
		case US_UPDATE: _update(_dt); break;
		case US_BLEND: _update_blend(_dt); break;
		default: break;
		}
	}

	return true;
}

bool OzzAnimator::Render(Wiwa::Camera* camera, glm::mat4 transform)
{
	if (!m_LoadedMesh || !m_LoadedSkeleton || !m_LoadedMaterial) return false;

	bool success = true;

	Wiwa::Renderer3D& r3d = Wiwa::Application::Get().GetRenderer3D();

	ozz::math::Float4x4 ozz_transform = ozz::math::Float4x4::identity();

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			ozz_transform.cols[i].m128_f32[j] = transform[i][j];
	}

	Material* mat = Wiwa::Resources::GetResourceById<Material>(m_MaterialId);

	success &= r3d.RenderOzzSkinnedMesh(camera, m_Mesh, mat, make_span(skinning_matrices_), ozz_transform);

	return success;
}

void OzzAnimator::SaveAnimator(OzzAnimator* animator, const char* filepath)
{
	size_t a_size = animator->m_AnimationList.size();

	JSONDocument animator_doc;

	// Save animator mesh
	animator_doc.AddMember("mesh", animator->getMeshPath().c_str());

	// Save animator material
	animator_doc.AddMember("material", animator->getMaterialPath().c_str());

	// Save skeleton mesh
	animator_doc.AddMember("skeleton", animator->getSkeletonPath().c_str());

	// Save blending settings
	animator_doc.AddMember("transition_blend", animator->getBlendOnTransition());
	animator_doc.AddMember("transition_time", animator->getTransitionTime());

	// Save animation list
	JSONValue animation_list = animator_doc.AddMemberArray("animation_list");

	for (size_t i = 0; i < a_size; i++)
	{
		// Animation data
		AnimationData& a_data = animator->m_AnimationList[i];

		if (!a_data.animation) continue;

		// Animation
		OzzAnimation* anim = a_data.animation;

		// Animation type
		AnimationType a_type = anim->getAnimationType();

		// JSON Animation object
		JSONValue anim_obj = animation_list.PushBackObject();

		// Base data
		anim_obj.AddMember("name", a_data.name.c_str());
		anim_obj.AddMember("type", (int)a_type);
		anim_obj.AddMember("playback_speed", anim->getPlaybackSpeed());
		anim_obj.AddMember("loop", anim->getLoop());

		// Specific data
		switch (a_type)
		{
		case AT_PARTIAL_BLEND:
		{
			OzzAnimationPartialBlending* partial_anim = (OzzAnimationPartialBlending*)anim;

			anim_obj.AddMember("upper_body_root", partial_anim->GetUpperBodyRoot());
			anim_obj.AddMember("lower_body_file", partial_anim->getLowerBodyFile());
			anim_obj.AddMember("upper_body_file", partial_anim->getUpperBodyFile());
		}break;

		case AT_SIMPLE:
		{
			OzzAnimationSimple* simple_anim = (OzzAnimationSimple*)anim;

			anim_obj.AddMember("animation_file", simple_anim->getAnimationPath());
		}break;

		default:
			break;
		}
	}

	animator_doc.save_file(filepath);
}

OzzAnimator* OzzAnimator::LoadAnimator(const char* filepath)
{
	JSONDocument animator_doc(filepath);

	if (!animator_doc.IsObject()) return nullptr;

	OzzAnimator* animator = new OzzAnimator();

	if (animator_doc.HasMember("mesh"))
	{
		const char* mesh_file = animator_doc["mesh"].as_string();
		animator->LoadMesh(mesh_file);
	}

	if (animator_doc.HasMember("material"))
	{
		const char* mesh_file = animator_doc["material"].as_string();
		animator->LoadMaterial(mesh_file);
	}

	if (animator_doc.HasMember("skeleton"))
	{
		const char* skeleton_file = animator_doc["skeleton"].as_string();
		animator->LoadSkeleton(skeleton_file);
	}

	if (animator_doc.HasMember("transition_blend"))
	{
		bool transition_blend = animator_doc["transition_blend"].as_bool();
		animator->setBlendOnTransition(transition_blend);
	}

	if (animator_doc.HasMember("transition_time"))
	{
		float transition_time = animator_doc["transition_time"].as_float();
		animator->setTransitionTime(transition_time);
	}

	if (animator_doc.HasMember("animation_list"))
	{
		// Get animation list
		JSONValue anim_list = animator_doc["animation_list"];

		size_t a_size = anim_list.Size();

		for (size_t i = 0; i < a_size; i++)
		{
			// JSON Animation object
			JSONValue anim_obj = anim_list[i];

			// Animation data
			AnimationData a_data;

			// Animation type
			AnimationType a_type;

			// Base data
			a_data.name = anim_obj["name"].as_string();
			a_type = (AnimationType)anim_obj["type"].as_int();

			OzzAnimation* animation = nullptr;

			switch (a_type)
			{
			case AT_PARTIAL_BLEND:
			{
				size_t index = animator->CreatePartialAnimation(a_data.name);
				animation = animator->getAnimationAt(index).animation;
				OzzAnimationPartialBlending* partial_anim = (OzzAnimationPartialBlending*)animation;

				if (anim_obj.HasMember("lower_body_file")) {
					const char* lower_body_file = anim_obj["lower_body_file"].as_string();
					partial_anim->LoadLowerAnimation(lower_body_file);
				}

				if (anim_obj.HasMember("upper_body_file")) {
					const char* upper_body_file = anim_obj["upper_body_file"].as_string();
					partial_anim->LoadUpperAnimation(upper_body_file);
				}

				if (anim_obj.HasMember("upper_body_root")) {
					int ubr = anim_obj["upper_body_root"].as_int();

					partial_anim->SetUpperBodyRoot(ubr);
				}
			}break;

			case AT_SIMPLE:
			{
				size_t index = animator->CreateSimpleAnimation(a_data.name);
				animation = animator->getAnimationAt(index).animation;
				OzzAnimationSimple* simple_anim = (OzzAnimationSimple*)animation;

				if (anim_obj.HasMember("animation_file")) {
					const char* anim_file = anim_obj["animation_file"].as_string();

					simple_anim->LoadAnimation(anim_file);
				}
			}break;

			default:
				break;
			}

			if (anim_obj.HasMember("playback_speed"))
			{
				float p_speed = anim_obj["playback_speed"].as_float();
				animation->setPlaybackSpeed(p_speed);
			}

			if (anim_obj.HasMember("loop"))
			{
				bool loop = anim_obj["loop"].as_bool();
				animation->setLoop(loop);
			}
		}
	}

	return animator;
}

ResourceId OzzAnimator::GetMaterial()
{
	return m_MaterialId;
}