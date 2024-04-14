//#include <wipch.h>
#include "OzzAnimator.h"

#include "ozz/animation/runtime/animation.h"
#include "ozz/animation/runtime/blending_job.h"
#include "ozz/animation/runtime/local_to_model_job.h"
#include "ozz/animation/runtime/sampling_job.h"
#include "ozz/animation/runtime/skeleton.h"
#include "ozz/animation/runtime/skeleton_utils.h"
#include "ozz/base/containers/vector.h"
#include "ozz/base/log.h"
#include "ozz/base/maths/simd_math.h"
#include "ozz/base/maths/soa_transform.h"
#include "ozz/base/maths/vec_float.h"
#include "ozz/options/options.h"

#include "animations/OzzAnimationSimple.h"

//#include <Wiwa/utilities/json/JSONDocument.h>
//#include <Wiwa/core/Application.h>

#include "../Defs.h"
#include "../Resources.h"
#include "nlohmann/json.hpp"

#include <fstream>


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
	//m_MaterialId(INVALID_INDEX),
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

// hekbas: TODO OZZ
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

		a_data.animation->SetTimeRatio(time_ratio);

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

void OzzAnimator::SetUpperPlaybackSpeed(size_t anim_id, float playback_speed)
{
	if (anim_id < 0 || anim_id >= m_AnimationList.size()) return;

	AnimationData& a_data = m_AnimationList[anim_id];

	if (!a_data.animation) return;

	if (a_data.animation->getAnimationType() == AnimationType::AT_PARTIAL_BLEND)
	{
		OzzAnimationPartialBlending* a_partial = (OzzAnimationPartialBlending*)a_data.animation;
		a_partial->SetUpperPlaybackSpeed(playback_speed);
	}
}

void OzzAnimator::SetLowerPlaybackSpeed(size_t anim_id, float playback_speed)
{
	if (anim_id < 0 || anim_id >= m_AnimationList.size()) return;

	AnimationData& a_data = m_AnimationList[anim_id];

	if (!a_data.animation) return;

	if (a_data.animation->getAnimationType() == AnimationType::AT_PARTIAL_BLEND)
	{
		OzzAnimationPartialBlending* a_partial = (OzzAnimationPartialBlending*)a_data.animation;
		a_partial->SetLowerPlaybackSpeed(playback_speed);
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

// hekbas: TODO OZZ
bool OzzAnimator::Render(Camera* camera, mat4 transform)
{
	/*if (!m_LoadedMesh || !m_LoadedSkeleton || !m_LoadedMaterial) return false;

	bool success = true;

	Wiwa::Renderer3D& r3d = Wiwa::Application::Get().GetRenderer3D();

	ozz::math::Float4x4 ozz_transform = ozz::math::Float4x4::identity();

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			ozz_transform.cols[i].m128_f32[j] = transform[i][j];
	}

	Material* mat = Resources::GetResourceById<Material>(m_MaterialId);

	success &= r3d.RenderOzzSkinnedMesh(camera, m_Mesh, mat, make_span(skinning_matrices_), ozz_transform);

	return success;*/

	return true; //DELETE THIS WHEN FIX
}

void OzzAnimator::SaveAnimator(OzzAnimator* animator, const char* filepath)
{
	nlohmann::json animator_doc;

	// Save animator mesh
	animator_doc["mesh"] = animator->getMeshPath();

	// Save animator material
	animator_doc["material"] = animator->getMaterialPath();

	// Save skeleton mesh
	animator_doc["skeleton"] = animator->getSkeletonPath();

	// Save blending settings
	animator_doc["transition_blend"] = animator->getBlendOnTransition();
	animator_doc["transition_time"] = animator->getTransitionTime();

	// Save animation list
	nlohmann::json animation_list;

	for (const auto& a_data : animator->m_AnimationList)
	{
		if (!a_data.animation) continue;

		// Animation
		OzzAnimation* anim = a_data.animation;

		// Animation type
		AnimationType a_type = anim->getAnimationType();

		// JSON Animation object
		nlohmann::json anim_obj;

		// Base data
		anim_obj["name"] = a_data.name;
		anim_obj["type"] = static_cast<int>(a_type);
		anim_obj["playback_speed"] = anim->getPlaybackSpeed();
		anim_obj["loop"] = anim->getLoop();

		// Specific data
		switch (a_type)
		{
		case AT_PARTIAL_BLEND:
		{
			OzzAnimationPartialBlending* partial_anim = dynamic_cast<OzzAnimationPartialBlending*>(anim);
			if (partial_anim)
			{
				anim_obj["upper_body_root"] = partial_anim->GetUpperBodyRoot();
				anim_obj["lower_body_file"] = partial_anim->GetLowerBodyFile();
				anim_obj["upper_body_file"] = partial_anim->GetUpperBodyFile();
			}
		} break;

		case AT_SIMPLE:
		{
			OzzAnimationSimple* simple_anim = dynamic_cast<OzzAnimationSimple*>(anim);
			if (simple_anim)
			{
				anim_obj["animation_file"] = simple_anim->getAnimationPath();
			}
		} break;

		default:
			break;
		}

		animation_list.push_back(anim_obj);
	}

	animator_doc["animation_list"] = animation_list;

	std::ofstream ofs(filepath);
	ofs << animator_doc.dump(4); // Pretty printing with indentation
	ofs.close();
}

OzzAnimator* OzzAnimator::LoadAnimator(const char* filepath)
{
	std::ifstream ifs(filepath);
	if (!ifs.is_open()) {
		// Handle file not found or other error
		return nullptr;
	}

	nlohmann::json animator_doc;
	ifs >> animator_doc;
	ifs.close();

	if (!animator_doc.is_object()) return nullptr;

	OzzAnimator* animator = new OzzAnimator();

	if (animator_doc.contains("mesh"))
	{
		const std::string mesh_file = animator_doc["mesh"].get<std::string>();
		animator->LoadMesh(mesh_file.c_str());
	}

	// Similarly handle other members...

	if (animator_doc.contains("animation_list"))
	{
		for (const auto& anim_obj : animator_doc["animation_list"])
		{
			AnimationData a_data;

			a_data.name = anim_obj["name"].get<std::string>();
			AnimationType a_type = static_cast<AnimationType>(anim_obj["type"].get<int>());

			OzzAnimation* animation = nullptr;

			switch (a_type)
			{
			case AT_PARTIAL_BLEND:
			{
				size_t index = animator->CreatePartialAnimation(a_data.name);
				animation = animator->getAnimationAt(index).animation;
				OzzAnimationPartialBlending* partial_anim = dynamic_cast<OzzAnimationPartialBlending*>(animation);

				if (anim_obj.contains("lower_body_file")) {
					const std::string lower_body_file = anim_obj["lower_body_file"].get<std::string>();
					partial_anim->LoadLowerAnimation(lower_body_file.c_str());
				}

				if (animator_doc.contains("material"))
				{
					const std::string material_file = animator_doc["material"].get<std::string>();
					animator->LoadMaterial(material_file.c_str());
				}

				if (animator_doc.contains("skeleton"))
				{
					const std::string skeleton_file = animator_doc["skeleton"].get<std::string>();
					animator->LoadSkeleton(skeleton_file.c_str());
				}

				if (animator_doc.contains("transition_blend"))
				{
					bool transition_blend = animator_doc["transition_blend"].get<bool>();
					animator->setBlendOnTransition(transition_blend);
				}

				if (animator_doc.contains("transition_time"))
				{
					float transition_time = animator_doc["transition_time"].get<float>();
					animator->setTransitionTime(transition_time);
				}

			} break;

			case AT_SIMPLE:
			{
				size_t index = animator->CreateSimpleAnimation(a_data.name);
				animation = animator->getAnimationAt(index).animation;
				OzzAnimationSimple* simple_anim = dynamic_cast<OzzAnimationSimple*>(animation);

				if (anim_obj.contains("animation_file")) {
					const std::string anim_file = anim_obj["animation_file"].get<std::string>();
					simple_anim->LoadAnimation(anim_file.c_str());
				}
			} break;

			default:
				break;
			}

			if (animation)
			{
				if (anim_obj.contains("playback_speed"))
				{
					float p_speed = anim_obj["playback_speed"].get<float>();
					animation->setPlaybackSpeed(p_speed);
				}

				if (anim_obj.contains("loop"))
				{
					bool loop = anim_obj["loop"].get<bool>();
					animation->setLoop(loop);
				}
			}
		}
	}

	return animator;
}

// hekbas: TODO OZZ
ResourceId OzzAnimator::GetMaterial()
{
	return m_MaterialId;
}