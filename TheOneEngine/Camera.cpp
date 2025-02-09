#include "Camera.h"
#include "Defs.h"

#include "GameObject.h"
#include "Transform.h"
#include "Ray.h"

Camera::Camera(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Camera),
    cameraType(CameraType::PERSPECTIVE),
    aspect(1.777), fov(65), size(200), 
    zNear(0.1), zFar(3000),
    yaw(0), pitch(0),
    viewMatrix(1.0f),
    lookAt(0, 0, 0),
    drawFrustum(true),
    primaryCam(false)
{
    Transform* transform = containerGO.get()->GetComponent<Transform>();

    if (transform)
    {
        lookAt = transform->GetGlobalPosition() + transform->GetGlobalForward();
    }
    else
    {
        LOG(LogType::LOG_ERROR, "GameObject Container invalid!");
    }  
}

Camera::Camera(std::shared_ptr<GameObject> containerGO, Camera* ref) : Component(containerGO, ComponentType::Camera),
    cameraType(ref->cameraType),
    aspect(ref->aspect), fov(ref->fov), size(ref->size),
    zNear(ref->zNear), zFar(ref->zFar),
    yaw(ref->yaw), pitch(ref->pitch),
    viewMatrix(ref->viewMatrix),
    lookAt(ref->lookAt),
    drawFrustum(ref->drawFrustum),
    primaryCam(ref->primaryCam)
{
    frustum = ref->frustum;
    projectionMatrix = ref->projectionMatrix;
    viewProjectionMatrix = ref->viewProjectionMatrix;
}

Camera::~Camera() {}


const mat4f& Camera::getViewMatrix()
{
    return viewMatrix;
}


void Camera::UpdateCamera()
{
    UpdateCameraVectors();
    UpdateViewMatrix();
    UpdateProjectionMatrix();
    UpdateViewProjectionMatrix();
    UpdateFrustum();
}

void Camera::UpdateCameraVectors()
{
    GameObject* GO = this->containerGO.lock().get();

    if (GO)
    {
        Transform* transform = GO->GetComponent<Transform>();
        lookAt = transform->GetGlobalPosition() + transform->GetGlobalForward();
    }
    else
    {
        LOG(LogType::LOG_ERROR, "GameObject Container invalid!");
    }
}

void Camera::UpdateViewMatrix()
{
    GameObject* GO = this->containerGO.lock().get();

    if (GO)
    {
		Transform* transform = GO->GetComponent<Transform>();
        Camera* camera = GO->GetComponent<Camera>();
        glm::vec3 tempPosition = transform->GetGlobalPosition();
        glm::vec3 tempUp = transform->GetGlobalUp();

        viewMatrix = glm::lookAt(tempPosition, camera->lookAt, tempUp);
    }
    else
    {
        LOG(LogType::LOG_ERROR, "GameObject Container invalid!");
    }
}

void Camera::UpdateProjectionMatrix()
{
    switch (cameraType)
    {
    case CameraType::PERSPECTIVE:
        projectionMatrix = glm::perspective(glm::radians(fov), aspect, zNear, zFar);
        break;

    case CameraType::ORTHOGRAPHIC:
    {
        double halfWidth = size * 0.5f;
        double halfHeight = halfWidth / aspect;
        projectionMatrix = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, zNear, zFar);
    }
    break;

    default:
        LOG(LogType::LOG_ERROR, "CameraType invalid!");
        break;
    }
}

void Camera::UpdateViewProjectionMatrix()
{
    viewProjectionMatrix = projectionMatrix * viewMatrix;
}

void Camera::UpdateFrustum()
{
    frustum.Update(viewProjectionMatrix);
}


Ray Camera::ComputeCameraRay(float x, float y)
{
    glm::mat4 viewProjInverse = glm::inverse(viewProjectionMatrix);

    //glm::vec4 worldDirection = viewProjInverse * glm::vec4(x, y, -1.0f, 1.0f);
    glm::vec4 worldDirection = viewProjInverse * glm::vec4(x, y, 1.0f, 1.0f);

	GameObject* GO = this->containerGO.lock().get();

	if (GO)
	{
		Transform* transform = GO->GetComponent<Transform>();
        return Ray(transform->GetGlobalPosition(), glm::normalize(worldDirection));
	}
	else
	{
		LOG(LogType::LOG_ERROR, "GameObject Container invalid!");
        return Ray();
	}
}


json Camera::SaveComponent()
{
    json cameraJSON;

    cameraJSON["Name"] = name;
    cameraJSON["Type"] = type;

    if (auto pGO = containerGO.lock())
        cameraJSON["ParentUID"] = pGO.get()->GetUID();

    cameraJSON["UID"] = UID;
    cameraJSON["FOV"] = fov;
    cameraJSON["Aspect"] = aspect;
    cameraJSON["zNear"] = zNear;
    cameraJSON["zFar"] = zFar;
    cameraJSON["Yaw"] = yaw;
    cameraJSON["Pitch"] = pitch;
    cameraJSON["Size"] = size;
    cameraJSON["CameraType"] = cameraType;
    cameraJSON["PrimaryCamera"] = primaryCam;

    return cameraJSON;
}

void Camera::LoadComponent(const json& cameraJSON)
{
    // Load basic properties
    if (cameraJSON.contains("UID")) UID = cameraJSON["UID"];
    if (cameraJSON.contains("Name")) name = cameraJSON["Name"];

    // Load camera-specific properties
    if (cameraJSON.contains("FOV")) fov = cameraJSON["FOV"];
    if (cameraJSON.contains("Aspect")) aspect = cameraJSON["Aspect"];
    if (cameraJSON.contains("zNear")) zNear = cameraJSON["zNear"];
    if (cameraJSON.contains("zFar")) zFar = cameraJSON["zFar"];
    if (cameraJSON.contains("Yaw")) yaw = cameraJSON["Yaw"];
    if (cameraJSON.contains("Pitch")) pitch = cameraJSON["Pitch"];
    if (cameraJSON.contains("Size")) size = cameraJSON["Size"];

    if (cameraJSON.contains("CameraType")) 
    {
        if (cameraJSON["CameraType"] == 0)
            cameraType = CameraType::PERSPECTIVE;
        else if (cameraJSON["CameraType"] == 1)
            cameraType = CameraType::ORTHOGRAPHIC;
    }

    if (cameraJSON.contains("PrimaryCamera")) primaryCam = cameraJSON["PrimaryCamera"];

    // Optional: Recalculate view and projection matrices based on loaded data
    UpdateCamera();
}