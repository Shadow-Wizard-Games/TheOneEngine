#pragma once

//#include <Wiwa/core/Core.h>
//#include <Wiwa/core/Renderer3D.h>

#include "../Defs.h"
#include "animations/OzzAnimationPartialBlending.h"

#include <string>
#include <vector>
#include <unordered_map>

typedef int ResourceId;
class Camera;


class OzzAnimator
{
public:
	struct AnimationData
	{
		OzzAnimation* animation;
		std::string name;
	};

private:
	enum UpdateState
	{
		US_BLEND,
		US_UPDATE,
		US_LAST
	};

public:
	OzzAnimator();
	~OzzAnimator();

	// Load mesh function
	bool LoadMesh(const std::string& path);

	// Check if mesh was loaded
	bool IsMeshLoaded() { return m_LoadedMesh; }

	// Get loaded mesh path
	std::string getMeshPath() { return m_MeshPath; }

	// Load skeleton function
	bool LoadSkeleton(const std::string& path);

	// Check if skeleton was loaded
	bool IsSkeletonLoaded() { return m_LoadedSkeleton; }

	// Load material
	bool LoadMaterial(const std::string& path);

	// Check if material was loaded
	bool IsMaterialLoaded() { return m_LoadedMaterial; }

	// Get loaded material path
	std::string getMaterialPath() { return m_MaterialPath; }

	// Get loaded skeleton path
	std::string getSkeletonPath() { return m_SkeletonPath; }

	// Get skeleton bone names
	ozz::span<const char* const> getSkeletonBoneNames();

	// Create simple animation
	size_t CreateSimpleAnimation(const std::string& name);

	// Create partial animation
	size_t CreatePartialAnimation(const std::string& name);

	// Remove animation at index
	void RemoveAnimationAt(size_t index);

	// Check if animation name exists
	bool HasAnimation(const std::string& name);

	// Set transition blend bool
	void setBlendOnTransition(bool blend) { m_BlendOnTransition = blend; }

	// Get transition blend bool
	bool getBlendOnTransition() { return m_BlendOnTransition; }

	// Set transition time for blending
	void setTransitionTime(float time) { m_TransitionTime = time; };

	// Get transition time for blending
	float getTransitionTime() { return m_TransitionTime; }

	// Play animation by name
	AnimationData* PlayAnimation(const std::string& name, float time_ratio = 0.0f);

	// Play animation by id
	AnimationData* PlayAnimation(size_t anim_id, float time_ratio = 0.0f);

	// Stop animation
	void StopAnimation(bool unbind = true);

	// Get total animation count (includes removed animations)
	size_t getAnimationCount() { return m_AnimationList.size(); }

	// Get animation data at index
	AnimationData& getAnimationAt(size_t index) { return m_AnimationList[index]; }

	// Get animation by name
	OzzAnimation* getAnimationByName(const std::string& name);

	// Get animation index by name
	size_t getAnimationIndex(const std::string& str);

	// Get active animation ID
	size_t getActiveAnimationId() { return m_ActiveAnimationId; }

	// Get active animation
	OzzAnimation* getActiveAnimation();

	// Get active animation name
	std::string getActiveAnimationName() { return m_ActiveAnimationName; }

	// Set upper animation playback speed
	void SetUpperPlaybackSpeed(size_t anim_id, float playback_speed);

	// Set lower animation playback speed
	void SetLowerPlaybackSpeed(size_t anim_id, float playback_speed);

	// Initialize
	void Init();

	// Update
	bool Update(float _dt);

	// Render
	bool Render(Camera* camera, mat4 transform);

	// Static save function
	static void SaveAnimator(OzzAnimator* animator, const char* filepath);

	// Static load function
	static OzzAnimator* LoadAnimator(const char* filepath);

	// Get animator material
	ResourceId GetMaterial();

private:
	size_t _create_anim_impl();
	bool _update(float _dt);
	bool _update_blend(float _dt);

private:
	UpdateState m_UpdateState;

	bool m_BlendOnTransition;
	float m_BlendThreshold;
	float m_TransitionTime;
	float m_TransitionTimer;

	std::string m_MeshPath;
	ozz::sample::Mesh m_Mesh;
	bool m_LoadedMesh;

	std::string m_SkeletonPath;
	ozz::animation::Skeleton m_Skeleton;
	bool m_LoadedSkeleton;

	std::string m_MaterialPath;
	ResourceId m_MaterialId;
	bool m_LoadedMaterial;

	std::string m_ActiveAnimationName;
	size_t m_ActiveAnimationId;
	size_t m_PrevAnimationId;

	// Buffer of model space matrices. These are computed by the local-to-model
	// job after the blending stage.
	ozz::vector<ozz::math::Float4x4> models_;

	// Buffer of skinning matrices, result of the joint multiplication of the
	// inverse bind pose with the model space matrix.
	ozz::vector<ozz::math::Float4x4> skinning_matrices_;

	// Buffer of local transforms which stores the blending result.
	ozz::vector<ozz::math::SoaTransform> blended_locals_;

	std::vector<AnimationData> m_AnimationList;
	std::unordered_map<std::string, size_t> m_AnimationsIndex;

	std::vector<size_t> m_RemovedAnimationsIndex;
};