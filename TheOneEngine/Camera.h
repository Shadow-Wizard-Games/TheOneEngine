#ifndef __CAMERA_H__
#define __CAMERA_H__
#pragma once

#include "Defs.h"
#include "GameObject.h"
#include "Ray.h"
#include "Log.h"

#include <memory>

enum class CameraType 
{
    PERSPECTIVE,
    ORTHOGRAPHIC
};

struct Plane
{
    //plane eq -> ax + by + cz + d = 0
    glm::vec3 normal;
    float distance;

    static Plane CreatePlaneFromVec4(glm::vec4 vec)
    {
        Plane plane;
        plane.normal.x = vec.x;
        plane.normal.y = vec.y;
        plane.normal.z = vec.z;
        plane.distance = vec.w;
        return plane;
    }

    void Normalise()
    {
        float magnitude = glm::length(normal);
        normal /= magnitude;
        distance /= magnitude;
    }
};

struct Frustum : public Plane
{
    // hekbas - don't use near and far
    // those are already defined in minwindef.h
    Plane _near; 
    Plane _far;
    Plane left;
    Plane right;
    Plane top;
    Plane bot;

    glm::vec3 vertices[8];

    void Update(const glm::mat4& vpm)
    {
        //glm::mat4 vpm = glm::mat4(1.0f);
       
        left    = CreatePlaneFromVec4({ vpm[0][3] + vpm[0][0], vpm[1][3] + vpm[1][0], vpm[2][3] + vpm[2][0], vpm[3][3] + vpm[3][0] });
        right   = CreatePlaneFromVec4({ vpm[0][3] - vpm[0][0], vpm[1][3] - vpm[1][0], vpm[2][3] - vpm[2][0], vpm[3][3] - vpm[3][0] });
        bot     = CreatePlaneFromVec4({ vpm[0][3] + vpm[0][1], vpm[1][3] + vpm[1][1], vpm[2][3] + vpm[2][1], vpm[3][3] + vpm[3][1] });
        top     = CreatePlaneFromVec4({ vpm[0][3] - vpm[0][1], vpm[1][3] - vpm[1][1], vpm[2][3] - vpm[2][1], vpm[3][3] - vpm[3][1] });
        _near   = CreatePlaneFromVec4({ vpm[0][3] + vpm[0][2], vpm[1][3] + vpm[1][2], vpm[2][3] + vpm[2][2], vpm[3][3] + vpm[3][2] });
        _far    = CreatePlaneFromVec4({ vpm[0][3] - vpm[0][2], vpm[1][3] - vpm[1][2], vpm[2][3] - vpm[2][2], vpm[3][3] - vpm[3][2] });

        left.Normalise();
        right.Normalise();
        bot.Normalise();
        top.Normalise();
        _near.Normalise();
        _far.Normalise();

        CalculateVertices(vpm);
    }

    void CalculateVertices(const glm::mat4& transform)
    {
        static const bool zerotoOne = false;

        glm::mat4 transformInv = glm::inverse(transform);

        vertices[0] = glm::vec4(-1.0f, -1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f);
        vertices[1] = glm::vec4(1.0f, -1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f);
        vertices[2] = glm::vec4(1.0f, 1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f);
        vertices[3] = glm::vec4(-1.0f, 1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f);

        vertices[4] = glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f);
        vertices[5] = glm::vec4(1.0f, -1.0f, 1.0f, 1.0f);
        vertices[6] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        vertices[7] = glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f);

        glm::vec4 temp;
        for (int i = 0; i < 8; i++)
        {
            temp = transformInv * glm::vec4(vertices[i], 1.0f);
            vertices[i] = temp / temp.w;
        }
    }
};

class Camera : public Component
{
public:

    Camera(std::shared_ptr<GameObject> containerGO);
    Camera(std::shared_ptr<GameObject> containerGO, Camera* ref);
    ~Camera();

    const mat4f& getViewMatrix();

    void UpdateCamera();
    void UpdateCameraVectors();
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();
    void UpdateViewProjectionMatrix();
    void UpdateFrustum();

    Ray ComputeCameraRay(float x, float y);

    json SaveComponent();
    void LoadComponent(const json& meshJSON);

public:

    CameraType cameraType;

    //perpective
    double fov;
    double aspect;

    //orthogonal
    double size;

    double zNear;
    double zFar;

    float yaw, pitch;

	vec3f lookAt;

    Frustum frustum;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 viewProjectionMatrix;

    bool drawFrustum;

    bool primaryCam;
};
#endif // !__CAMERA_H__