#pragma once
#include "VertexArray.h"
#include "Animation/animations/OzzAnimationPartialBlending.h"


class Texture;
struct aiMesh;

struct MeshVertex
{
    vec3f position;
    vec2f texCoords;
};

class Model
{
public:
    friend class ModelScene;

    struct AnimationData
    {
        OzzAnimation* animation;
        std::string name;
    };

    Model(){}
    Model(const std::string& path);
    ~Model(){}

    static std::vector<Model*> LoadMeshes(const std::string& path);
    static void SaveMesh(Model* mesh, const std::string& path);

    void Render();

    std::string GetMaterialPath() { return materials[materialIndex]; }
    std::string GetMeshPath() { return path; }

    std::string meshName;
    std::vector<float> vertexData;
    std::vector<unsigned int> indexData;

    glm::mat4 meshTransform;

    BufferLayout format;

    std::shared_ptr<VertexArray> meshVAO;
    std::shared_ptr<VertexBuffer> meshVBO;
    std::shared_ptr<IndexBuffer> meshIBO;

    std::vector<vec3f> meshVerts;
    std::vector<vec3f> meshNorms;
    std::vector<vec3f> meshFaceCenters;
    std::vector<vec3f> meshFaceNorms;

    std::string path;

    void serializeMeshData(const std::string& filename);
    void deserializeMeshData(const std::string& filename);

private:
    void GenBufferData();

    std::vector<std::string> materials;
    uint materialIndex = INVALID_INDEX;


    /*===================================== OZZ MESH =====================================*/

    bool hasBones = false;

    enum UpdateState
    {
        US_BLEND,
        US_UPDATE,
        US_LAST
    };

    UpdateState m_UpdateState;

    bool m_BlendOnTransition;
    float m_BlendThreshold;
    float m_TransitionTime;
    float m_TransitionTimer;

    std::string ozzMeshPath;
    ozz::sample::Mesh m_Mesh;
    bool m_LoadedMesh;

    std::string m_SkeletonPath;
    ozz::animation::Skeleton m_Skeleton;
    bool m_LoadedSkeleton;

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

    size_t _create_anim_impl();
    bool _update(float _dt);
    bool _update_blend(float _dt);
    void ImportToOzz(const std::string& file, const std::filesystem::path& importPath);
public:

    bool isAnimated() { return hasBones; }

    bool LoadOzzMesh(const std::string& path);
	bool IsMeshLoaded() { return m_LoadedMesh; }

	bool LoadSkeleton(const std::string& path);
	bool IsSkeletonLoaded() { return m_LoadedSkeleton; }
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

	// Play animation by name
	AnimationData* PlayAnimation(const std::string& name, float time_ratio = 0.0f);

	// Play animation by id
	AnimationData* PlayAnimation(size_t anim_id, float time_ratio = 0.0f);

	// Stop animation
	void StopAnimation(bool unbind = true);


    // Set transition blend bool
    void setBlendOnTransition(bool blend) { m_BlendOnTransition = blend; }

    // Get transition blend bool
    bool getBlendOnTransition() { return m_BlendOnTransition; }

    // Set transition time for blending
    void setTransitionTime(float time) { m_TransitionTime = time; };

    // Get transition time for blending
    float getTransitionTime() { return m_TransitionTime; }
	// Get total animation count (includes removed animations)
	size_t getAnimationCount() { return m_AnimationList.size(); }
	AnimationData& getAnimationAt(size_t index) { return m_AnimationList[index]; }
	OzzAnimation* getAnimationByName(const std::string& name);
	size_t getAnimationIndex(const std::string& str);
	size_t getActiveAnimationId() { return m_ActiveAnimationId; }
	OzzAnimation* getActiveAnimation();
	std::string getActiveAnimationName() { return m_ActiveAnimationName; }
    const ozz::sample::Mesh& GetOzzMesh() { return m_Mesh; }
    const std::string& GetOzzMeshPath() { return ozzMeshPath; }
    ozz::vector<ozz::math::Float4x4>& getSkinningMatrices() { return skinning_matrices_; }


	// Set upper animation playback speed
	void SetUpperPlaybackSpeed(size_t anim_id, float playback_speed);

	// Set lower animation playback speed
	void SetLowerPlaybackSpeed(size_t anim_id, float playback_speed);

	// Update
	bool UpdateAnim(float _dt);

	// Render
	//bool Render(Camera* camera, mat4 transform);

	// Static save function
	void SaveAnimator(const std::string& filepath);

	// Static load function
	void LoadAnimator(const std::string& filepath);
};
