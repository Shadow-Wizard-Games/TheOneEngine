#include "SkeletalModel.h"
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <filesystem>
#include <fstream>
#include "Resources.h"

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

#include "Animation/animations/OzzAnimationSimple.h"


SkeletalModel::SkeletalModel(const std::string& filename) :
    m_ActiveAnimationId(INVALID_INDEX),
    m_PrevAnimationId(INVALID_INDEX),
    m_LoadedMesh(false),
    m_LoadedSkeleton(false),
    m_BlendOnTransition(true),
    m_BlendThreshold(ozz::animation::BlendingJob().threshold),
    m_TransitionTime(1.0f),
    m_TransitionTimer(0.0f),
    m_UpdateState(US_UPDATE)
{
    if (filename.ends_with(".fbx") || filename.ends_with(".FBX"))
    {
        LoadMesh(filename);
    }
    else if (filename.ends_with(".animator"))
    {
        path = filename;
        LoadAnimator(filename);
    }
}


void SkeletalModel::LoadMesh(const std::string& path)
{
    const aiScene* scene = aiImportFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_ForceGenNormals);

    std::string fileName = scene->GetShortFilename(path.c_str());
    std::string sceneName = fileName.substr(fileName.find_last_of("\\/") + 1, fileName.find_last_of('.') - fileName.find_last_of("\\/") - 1);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOG(LogType::LOG_ERROR, "Failed to load mesh from: %s", path.data());
        return;
    }

    for (size_t m = 0; m < scene->mNumMeshes; ++m)
    {
        aiMesh* mesh = scene->mMeshes[m];

        if (!mesh->HasBones())
            continue;

        // Basic Data
        std::string standarizedName = mesh->mName.C_Str();
        size_t index = 0;
        while ((index = standarizedName.find('.', index)) != std::string::npos || (index = standarizedName.find(':', index)) != std::string::npos) {
            standarizedName.replace(index, 1, "_");
            index++;
        }
        meshName = standarizedName;

        // Transform
        aiMatrix4x4 transform;
        glm::mat4 mTransform(1);
        if (mesh->mName != (aiString)"Scene")
        {
            aiNode* meshNode = scene->mRootNode->FindNode(mesh->mName);
            if (meshNode)
            {
                transform = meshNode->mTransformation;
                mTransform = glm::transpose(glm::make_mat4(&transform.a1));
            }
        }
        meshTransform = mTransform;

        // Process Animated Mesh materials
        if (scene->HasMaterials())
        {
            for (unsigned int i = 0; i < scene->mNumMaterials; i++)
            {
                aiMaterial* mat = scene->mMaterials[i];

                aiString name;
                aiGetMaterialString(mat, AI_MATKEY_NAME, &name);

                aiString texture_diffuse;
                aiGetMaterialString(mat, AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), &texture_diffuse);

                aiColor4D diffuse(1.0f, 1.0f, 1.0f, 1.0f);
                aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &diffuse);

                aiColor4D specular(1.0f, 1.0f, 1.0f, 1.0f);
                aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &specular);

                float shininess = 0.1f;
                aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &shininess);

                std::filesystem::path matPath = Resources::PathToLibrary<Material>(sceneName);
                matPath += name.C_Str();
                matPath += ".toematerial";

                ResourceId matID = Resources::LoadFromLibrary<Material>(matPath.string().c_str());

                materials.push_back(matPath.string());

                if (matID != -1)
                    continue;

                // Import material
                Material material;
                size_t id;

                if (texture_diffuse.length > 0)
                {
                    // TODO: this only loads texture paths if they are in the same folder as the fbx
                    fs::path texPath = fs::path(path).parent_path() / fs::path(texture_diffuse.C_Str()).filename();


                    id = Resources::LoadFromLibrary<Shader>("MeshTextureAnimated");
                    material.setShader(Resources::GetResourceById<Shader>(id), Resources::PathToLibrary<Shader>() + "MeshTextureAnimated.toeshader");
                    bool imported = Resources::Import<Texture>(texPath.string());

                    if (imported) {
                        texPath = Resources::PathToLibrary<Texture>() + texPath.filename().replace_extension(".dds").string();
                        ResourceId imgId = Resources::LoadFromLibrary<Texture>(texPath.string().c_str());
                        Texture* img = Resources::GetResourceById<Texture>(imgId);
                        Uniform::SamplerData data;
                        data.tex_id = img->GetTextureId();
                        data.resource_id = imgId;
                        memcpy(data.tex_path, &texPath.string()[0], texPath.string().size() + 1);
                        material.SetUniformData("u_Tex", data);
                    }
                }
                else
                {
                    id = Resources::LoadFromLibrary<Shader>("MeshColor");
                    material.setShader(Resources::GetResourceById<Shader>(id), Resources::PathToLibrary<Shader>() + "MeshColor.toeshader");

                    material.SetUniformData("u_Color", glm::vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a));
                }


                Resources::Import<Material>(matPath.string().c_str(), &material);
            }
        }
        materialIndex = mesh->mMaterialIndex;

        std::filesystem::path importPath = Resources::PathToLibrary<SkeletalModel>(sceneName) + meshName;
        ImportToOzz(path, importPath);
        LOG(LogType::LOG_INFO, "FBX Loaded is skeletal");
        LoadOzzMesh(ozzMeshPath);
        LoadSkeleton(m_SkeletonPath);

        Resources::Import<SkeletalModel>(Resources::PathToLibrary<SkeletalModel>(sceneName) + meshName, this);
    }
    aiReleaseImport(scene);
}

void SkeletalModel::SaveMesh(SkeletalModel* mesh, const std::string& path)
{
    mesh->SaveAnimator();
}


/*======================================================= OZZ ANIMATIOR MESH =======================================================*/

bool SkeletalModel::LoadOzzMesh(const std::string& filename)
{
    m_LoadedMesh = ozz::sample::LoadMesh(filename.c_str(), &m_Mesh);

    if (m_LoadedMesh)
    {
        size_t num_skinning_matrices = 0;
        num_skinning_matrices = ozz::math::Max(num_skinning_matrices, m_Mesh.joint_remaps.size());

        // Allocates skinning matrices.
        skinning_matrices_.resize(num_skinning_matrices);

        for (size_t i = 0; i < m_Mesh.joint_remaps.size(); ++i)
            skinning_matrices_[i] = ozz::math::Float4x4::identity();
    }

    return m_LoadedMesh;
}

bool SkeletalModel::LoadSkeleton(const std::string& path)
{
    m_LoadedSkeleton = ozz::sample::LoadSkeleton(path.c_str(), &m_Skeleton);

    if (m_LoadedSkeleton)
    {
        // Allocates runtime buffers.
        const int num_joints = m_Skeleton.num_joints();
        const int num_soa_joints = m_Skeleton.num_soa_joints();

        models_.resize(num_joints);
        // Allocates local space runtime buffers of blended data.
        blended_locals_.resize(num_soa_joints);
    }

    return m_LoadedSkeleton;
}


size_t SkeletalModel::_create_anim_impl()
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

bool SkeletalModel::_update(float _dt)
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

bool SkeletalModel::_update_blend(float _dt)
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

ozz::span<const char* const> SkeletalModel::getSkeletonBoneNames()
{
    if (!m_LoadedSkeleton)
        return ozz::span<const char* const>();

    return m_Skeleton.joint_names();
}

size_t SkeletalModel::CreateSimpleAnimation(const std::string& name)
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


size_t SkeletalModel::CreatePartialAnimation(const std::string& name)
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

void SkeletalModel::RemoveAnimationAt(size_t index)
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

bool SkeletalModel::HasAnimation(const std::string& name)
{
    return m_AnimationsIndex.find(name) != m_AnimationsIndex.end();
}

SkeletalModel::AnimationData* SkeletalModel::PlayAnimation(const std::string& name, float time_ratio)
{
    std::unordered_map<std::string, size_t>::iterator it = m_AnimationsIndex.find(name);

    if (it != m_AnimationsIndex.end())
        return PlayAnimation(it->second, time_ratio);

    return nullptr;
}

SkeletalModel::AnimationData* SkeletalModel::PlayAnimation(size_t anim_id, float time_ratio)
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

void SkeletalModel::StopAnimation(bool unbind)
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

OzzAnimation* SkeletalModel::getAnimationByName(const std::string& name)
{
    std::unordered_map<std::string, size_t>::iterator it = m_AnimationsIndex.find(name);

    if (it != m_AnimationsIndex.end())
        return m_AnimationList[it->second].animation;

    return nullptr;
}

size_t SkeletalModel::getAnimationIndex(const std::string& str)
{
    std::unordered_map<std::string, size_t>::iterator it = m_AnimationsIndex.find(str);

    if (it != m_AnimationsIndex.end())
        return it->second;

    return INVALID_INDEX;
}

OzzAnimation* SkeletalModel::getActiveAnimation()
{
    OzzAnimation* anim = nullptr;

    if (m_ActiveAnimationId != INVALID_INDEX)
        anim = m_AnimationList[m_ActiveAnimationId].animation;

    return anim;
}

void SkeletalModel::SetUpperPlaybackSpeed(size_t anim_id, float playback_speed)
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

void SkeletalModel::SetLowerPlaybackSpeed(size_t anim_id, float playback_speed)
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

bool SkeletalModel::UpdateAnim(float _dt)
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

void SkeletalModel::SaveAnimator()
{
    nlohmann::json animator_doc;

    // Save animator name
    animator_doc["name"] = meshName;
    // Save animator mesh
    animator_doc["mesh"] = ozzMeshPath;

    // Save animator material
    std::string matPath = GetMaterialPath();
    animator_doc["material"] = matPath;

    // Save skeleton mesh
    animator_doc["skeleton"] = m_SkeletonPath;

    // Save blending settings
    animator_doc["transition_blend"] = getBlendOnTransition();
    animator_doc["transition_time"] = getTransitionTime();

    // Save animation list
    nlohmann::json animation_list;

    for (const auto& a_data : m_AnimationList)
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

    Resources::SaveJSON(path, animator_doc);
}

void SkeletalModel::LoadAnimator(const std::string& filepath)
{
    nlohmann::json animator_doc = Resources::OpenJSON(filepath);

    if (animator_doc.contains("name"))
    {
        meshName = animator_doc["name"].get<std::string>();
    }

    if (animator_doc.contains("mesh"))
    {
        ozzMeshPath = animator_doc["mesh"].get<std::string>();
        LoadOzzMesh(ozzMeshPath);
    }

    if (animator_doc.contains("material"))
    {
        auxMatPath = animator_doc["material"].get<std::string>();
    }

    if (animator_doc.contains("skeleton"))
    {
        m_SkeletonPath = animator_doc["skeleton"].get<std::string>();
        LoadSkeleton(m_SkeletonPath);
    }

    if (animator_doc.contains("transition_blend"))
    {
        bool transition_blend = animator_doc["transition_blend"].get<bool>();
        setBlendOnTransition(transition_blend);
    }

    if (animator_doc.contains("transition_time"))
    {
        float transition_time = animator_doc["transition_time"].get<float>();
        setTransitionTime(transition_time);
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
                size_t index = CreatePartialAnimation(a_data.name);
                animation = getAnimationAt(index).animation;
                OzzAnimationPartialBlending* partial_anim = dynamic_cast<OzzAnimationPartialBlending*>(animation);

                if (anim_obj.contains("lower_body_file")) {
                    const std::string lower_body_file = anim_obj["lower_body_file"].get<std::string>();
                    partial_anim->LoadLowerAnimation(lower_body_file.c_str());
                }

                if (anim_obj.contains("upper_body_file")) {
                    std::string upper_body_file = anim_obj["upper_body_file"].get<std::string>();
                    partial_anim->LoadUpperAnimation(upper_body_file.c_str());
                }

                if (anim_obj.contains("upper_body_root")) {
                    int ubr = anim_obj["upper_body_root"].get<int>();
                    partial_anim->SetUpperBodyRoot(ubr);
                }

            } break;

            case AT_SIMPLE:
            {
                size_t index = CreateSimpleAnimation(a_data.name);
                animation = getAnimationAt(index).animation;
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
}

void SkeletalModel::ImportToOzz(const std::string& file, const std::filesystem::path& importPath)
{
    std::string filename = importPath.filename().string();

    Resources::PreparePath(importPath.parent_path().string());

    // Skeleton output path
    std::filesystem::path skeleton_path = importPath;
    skeleton_path.replace_filename(filename + "_skeleton");
    skeleton_path.replace_extension(".skeleton");
    m_SkeletonPath = skeleton_path.string();
    LOG(LogType::LOG_INFO, "Ozz Skeleton Path: %s", m_SkeletonPath.c_str());

    // Animations output path
    std::filesystem::path anim_out_path = importPath;
    anim_out_path.replace_filename(filename + "_*");
    anim_out_path.replace_extension(".anim");
    LOG(LogType::LOG_INFO, "Ozz Anim Path: %s", anim_out_path.string().c_str());

    // Mesh output path
    std::filesystem::path mesh_out_path = importPath;
    mesh_out_path.replace_filename(filename + "_animatedmesh");
    mesh_out_path.replace_extension(".ozzmesh");
    ozzMeshPath = mesh_out_path.string();
    LOG(LogType::LOG_INFO, "Ozz Mesh Path: %s", ozzMeshPath.c_str());

    // === Setup ozz import json config ===
    json ozzImportJSON;

    // skeleton settings
    json skeletonJSON;
    skeletonJSON["filename"] = skeleton_path.string().c_str();

    // skeleton import settings
    json skeletonImportJSON;
    skeletonImportJSON["enable"] = true;
    skeletonImportJSON["raw"] = false;
    skeletonJSON["import"] = skeletonImportJSON;

    ozzImportJSON["skeleton"] = skeletonJSON;


    // animation settings
    json animationsJSON;
    json animJSON;
    animJSON["clip"] = "*";
    animJSON["filename"] = anim_out_path.string().c_str();
    animationsJSON.push_back(animJSON);

    ozzImportJSON["animations"] = animationsJSON;

    Resources::SaveJSON("Tools\\import_anim.json", ozzImportJSON);

    // === Import ozz ===
    std::string cmd = "Tools\\import_mesh.bat";
    cmd += " \"";
    cmd += file.c_str(); // Path of file to import (.fbx)
    cmd += "\" \"";
    cmd += mesh_out_path.string().c_str(); // Path of output mesh
    cmd += "\" \"";
    cmd += skeleton_path.string().c_str(); // Path of skeleton to import mesh
    cmd += "\"";
    system(cmd.c_str());
}

void SkeletalModel::ImportAnimation(const std::string& file)
{
    CreateConfigOzzJSON(fs::path(path).parent_path().string() + "/" + meshName, fs::path(file).filename().replace_extension().string());

    // === Import ozz ===
    std::string cmd = "Tools\\import_anim.bat";
    cmd += " \"";
    cmd += file.c_str(); // Path of file to import (.fbx)
    cmd += " \"";
    system(cmd.c_str());
}

void SkeletalModel::CreateConfigOzzJSON(const std::filesystem::path& importPath, const std::string& name)
{
    std::string filename = importPath.filename().string();

    Resources::PreparePath(importPath.parent_path().string());

    // Skeleton output path
    std::filesystem::path skeleton_path = importPath;
    skeleton_path.replace_filename(filename + "_skeleton");
    skeleton_path.replace_extension(".skeleton");
    LOG(LogType::LOG_INFO, "Ozz Skeleton Path: %s", skeleton_path.c_str());

    // Animations output path
    std::filesystem::path anim_out_path = importPath;
    anim_out_path.replace_filename(filename + "_*" + name);
    anim_out_path.replace_extension(".anim");
    LOG(LogType::LOG_INFO, "Ozz Anim Path: %s", anim_out_path.string().c_str());

    // Mesh output path
    std::filesystem::path mesh_out_path = importPath;
    mesh_out_path.replace_filename(filename + "_animatedmesh");
    mesh_out_path.replace_extension(".ozzmesh");
    LOG(LogType::LOG_INFO, "Ozz Mesh Path: %s", mesh_out_path.c_str());

    // === Setup ozz import json config ===
    json ozzImportJSON;

    // skeleton settings
    json skeletonJSON;
    skeletonJSON["filename"] = skeleton_path.string().c_str();

    // skeleton import settings
    json skeletonImportJSON;
    skeletonImportJSON["enable"] = true;
    skeletonImportJSON["raw"] = false;
    skeletonJSON["import"] = skeletonImportJSON;

    ozzImportJSON["skeleton"] = skeletonJSON;


    // animation settings
    json animationsJSON;
    json animJSON;
    animJSON["clip"] = "*";
    animJSON["filename"] = anim_out_path.string().c_str();
    animationsJSON.push_back(animJSON);

    ozzImportJSON["animations"] = animationsJSON;

    Resources::SaveJSON("Tools\\import_anim.json", ozzImportJSON);
}