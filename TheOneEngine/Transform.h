#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__
#pragma once

#include "Defs.h"
#include "Component.h"

#include <memory>

enum class HandleSpace
{
    LOCAL,
    GLOBAL
};

enum class HandlePosition
{
    PIVOT,
    CENTER
};

class GameObject;

class Transform : public Component
{
public:

    Transform(std::shared_ptr<GameObject> containerGO);
    Transform(std::shared_ptr<GameObject> containerGO, mat4 transform);
    Transform(std::shared_ptr<GameObject> containerGO, Transform* ref);
    virtual ~Transform();

    void Update() override;

    // @Transform -------------------------------
    void Translate(const vec3& translation, const HandleSpace& space = HandleSpace::LOCAL);
    void SetPosition(const vec3& newPosition, const HandleSpace& space = HandleSpace::LOCAL);

    void SetRotation(const vec3& eulerAngles);
    void Rotate(const vec3& eulerAngles, const HandleSpace& space = HandleSpace::LOCAL);

    // Testing, do not use in scripting
    void RotateInspector(const vec3& eulerAngles);
    // Testing, do not use in scripting
    void RotateChangeOfBasis(const vec3& eulerAngles, const HandleSpace& space);

    void Scale(const vec3& scaleFactors);
    void SetScale(const vec3& newScale); 


    // @Utils -----------------------------------
    void DecomposeTransform();
    void DecomposeGlobalTransform();
    mat4 CalculateWorldTransform();
    mat4 GetGlobalTransform();
    mat4 WorldToLocalTransform(GameObject* GO, mat4 modifiedWorldTransform);

    void ExtractBasis(const glm::mat4& transformMatrix, glm::mat3& basis);
    glm::vec3 ChangeBasis(const glm::vec3& rotationVectorA, const glm::mat3& basisA, const glm::mat3& basisB);

    void UpdateCameraIfPresent();
    

    // @Get / Set --------------------------------
    vec3 GetGlobalForward() const;
    vec3 GetGlobalUp() const;
    vec3 GetGlobalRight() const;
	vec3 GetForward() const;
	vec3 GetUp() const;
	vec3 GetRight() const;

    void SetRight(vec3 newRight);
    void SetUp(vec3 newUp);
    void SetForward(vec3 newForward);

    vec3 GetGlobalPosition() const;
    quat GetGlobalRotation() const;
    vec3 GetGlobalScale() const;
	vec3 GetPosition() const;
	quat GetRotation() const;
	vec3 GetScale() const;

    mat4 GetTransform() const;
    void SetTransform(mat4 transform);

    vec3 GetRotationEuler() const;
    
public:

    json SaveComponent();
    void LoadComponent(const json& transformJSON);

private:

    mat4 transformMatrix;
    vec3 position;
    quat rotation;
    vec3 scale;

    mat4 globalTransformMatrix;
    vec3 globalPosition;
    quat globalRotation;
    vec3 globalScale;
};

#endif //__TRANSFORM_H__