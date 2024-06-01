#include "Transform.h"
#include "GameObject.h"
#include "Camera.h"

Transform::Transform(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Transform),
    transformMatrix(1.0f),
    position(0.0f),
    rotation(1, 0, 0, 0),
    scale(1.0f),
    globalTransformMatrix(1.0f)
{}

Transform::Transform(std::shared_ptr<GameObject> containerGO, Transform* ref)
    : Component(containerGO, ComponentType::Transform),
    transformMatrix(ref->transformMatrix),
    position(ref->position), rotation(ref->rotation), scale(ref->scale), 
    globalTransformMatrix(ref->globalTransformMatrix)
{}

Transform::Transform(std::shared_ptr<GameObject> containerGO, mat4 transform) : Component(containerGO, ComponentType::Transform),
    transformMatrix(transform), globalTransformMatrix(transform)
{
    DecomposeTransform();
}

Transform::~Transform() {}

void Transform::Update()
{
    CalculateWorldTransform();
    DecomposeGlobalTransform();
    UpdateCameraIfPresent();
}


// @Transform ----------------------------------------------------
void Transform::Translate(const vec3& translation, const HandleSpace& space)
{
    if (space == HandleSpace::GLOBAL)
    {
        mat4 newTransform = CalculateWorldTransform();
        newTransform = glm::translate(newTransform, translation);

        transformMatrix = WorldToLocalTransform(containerGO.lock().get(), newTransform);
    }
    else
    {
        mat4 newTransform = transformMatrix;
        transformMatrix[3] += vec4(translation, 1);
    }

    position = transformMatrix[3];
    UpdateCameraIfPresent();
}

void Transform::SetPosition(const vec3& newPosition, const HandleSpace& space)
{
    mat4 newTransform = space == HandleSpace::GLOBAL ? CalculateWorldTransform() : transformMatrix;
    newTransform[3] = glm::vec4(newPosition, 1.0f);

    transformMatrix = space == HandleSpace::GLOBAL ? WorldToLocalTransform(containerGO.lock().get(), newTransform) : newTransform;
    position = transformMatrix[3];

    UpdateCameraIfPresent();
}

void Transform::Rotate(const vec3& eulerAngles, const HandleSpace& space)
{
	glm::mat3x3 referenceFrameMat = glm::mat3x3(
		vec3(transformMatrix[0][0], transformMatrix[0][1], transformMatrix[0][2]),
		vec3(transformMatrix[1][0], transformMatrix[1][1], transformMatrix[1][2]),
		vec3(transformMatrix[2][0], transformMatrix[2][1], transformMatrix[2][2]));

	glm::vec3 vecInRefFrame = eulerAngles;
	if (space == HandleSpace::LOCAL)
		vecInRefFrame = referenceFrameMat * eulerAngles;

	//Generate the rotation matrix that corresponds to the rotation
	glm::mat3x3 rotX = glm::mat3x3(1, 0, 0,
		0, glm::cos(glm::radians(vecInRefFrame.x)), -glm::sin(glm::radians(vecInRefFrame.x)),
		0, glm::sin(glm::radians(vecInRefFrame.x)), glm::cos(glm::radians(vecInRefFrame.x)));

	glm::mat3x3 rotY = glm::mat3x3(glm::cos(glm::radians(vecInRefFrame.y)), 0, glm::sin(glm::radians(vecInRefFrame.y)),
		0, 1, 0,
		-glm::sin(glm::radians(vecInRefFrame.y)), 0, glm::cos(glm::radians(vecInRefFrame.y)));

	glm::mat3x3 rotZ = glm::mat3x3(glm::cos(glm::radians(vecInRefFrame.z)), -glm::sin(glm::radians(vecInRefFrame.z)), 0,
		glm::sin(glm::radians(vecInRefFrame.z)), glm::cos(glm::radians(vecInRefFrame.z)), 0,
		0, 0, 1);

	glm::mat3x3 rotMatrix = rotZ * rotY * rotX;

	//Apply the generated rotation matrix to the existing director vectors
	vec3 temp = rotMatrix * vec3(transformMatrix[2][0], transformMatrix[2][1], transformMatrix[2][2]);
	transformMatrix[2] = vec4(temp, 0.0f);
	temp = rotMatrix * vec3(transformMatrix[0][0], transformMatrix[0][1], transformMatrix[0][2]);
	transformMatrix[0] = vec4(temp, 0.0f);
	temp = rotMatrix * vec3(transformMatrix[1][0], transformMatrix[1][1], transformMatrix[1][2]);
	transformMatrix[1] = vec4(temp, 0.0f);

    UpdateCameraIfPresent();
}

void Transform::SetRotation(const vec3& eulerAngles)
{
	quat quaternion = glm::angleAxis(eulerAngles.x, vec3(1, 0, 0));
    //rotation = glm::normalize(quaternion);

	quaternion *= glm::angleAxis(eulerAngles.y, vec3(0, 1, 0));
	//rotation = glm::normalize(quaternion * rotation);

	quaternion *= glm::angleAxis(eulerAngles.z, vec3(0, 0, 1));
	//rotation = glm::normalize(quaternion * rotation);

    rotation = quaternion;

    transformMatrix = glm::translate(mat4(1.0f), position) * glm::mat4_cast(rotation) * glm::scale(mat4(1.0f), scale);

    UpdateCameraIfPresent();
}

void Transform::RotateInspector(const vec3& eulerAngles)
{
    // ROTATE LOCAL
    // Convert Euler angles to quaternion
    quat quaternion = quat(eulerAngles);

    // Convert quaternion to rotation matrix
    mat4 rotationMatrix4 = mat4_cast(quaternion);

    // Apply rotation to the original transform matrix
    transformMatrix = transformMatrix * rotationMatrix4;

    // Extract the rotation part of the transformation matrix
    glm::mat3 rotationMatrix3 = glm::mat3(transformMatrix);

    // Create quaternion from rotation matrix
    rotation = glm::quat_cast(rotationMatrix3);


    // ROTATE WORLD
    //// Convert Euler angles to quaternion
    //quat quaternion = quat(eulerAngles);

    //// Convert quaternion to rotation matrix
    //mat4 rotationMatrix4 = glm::mat4_cast(quaternion);

    //// Apply rotation to the original transform matrix (global rotation)
    //transformMatrix = rotationMatrix4 * transformMatrix;

    //// Extract the rotation part of the transformation matrix
    //glm::mat3 rotationMatrix3 = glm::mat3(transformMatrix);

    //// Create quaternion from rotation matrix
    //rotation = glm::quat_cast(rotationMatrix3);


    // SET ROTATION (overwrites scale?)
    //// Convert Euler angles to quaternion
    //quat quaternion = quat(eulerAngles);

    //// Convert quaternion to rotation matrix
    //mat4 rotationMatrix = glm::mat4_cast(quaternion);

    //// Update the rotation part of the transformMatrix
    //transformMatrix[0] = rotationMatrix[0];
    //transformMatrix[1] = rotationMatrix[1];
    //transformMatrix[2] = rotationMatrix[2];

    //// Update the rotation member variable
    //rotation = quaternion;
}

void Transform::RotateChangeOfBasis(const vec3& eulerAngles, const HandleSpace& space)
{
    glm::mat3 identity = glm::mat3(1);
    glm::mat3 newBasis;

    ExtractBasis(transformMatrix, newBasis);

    vec3 rotVectorNewBasis = ChangeBasis(eulerAngles, newBasis, identity);

    // Convert Euler angles to quaternion
    quat quaternion = quat(rotVectorNewBasis);

    // Convert quaternion to rotation matrix
    mat4 rotationMatrix4 = mat4_cast(quaternion);

    // Apply rotation to the original transform matrix
    transformMatrix = transformMatrix * rotationMatrix4;

    // Extract the rotation part of the transformation matrix
    glm::mat3 rotationMatrix3 = glm::mat3(transformMatrix);

    // Create quaternion from rotation matrix
    rotation = glm::quat_cast(rotationMatrix3);
}

// hekbas, kiko - Need to fix
void Transform::Scale(const vec3& scaleFactors)
{
    transformMatrix = glm::scale(transformMatrix, scaleFactors);
    scale *= scaleFactors;
}

void Transform::SetScale(const vec3& newScale)
{
	mat4 newScaleMatrix = glm::scale(mat4(1.0f), vec3(newScale.x, newScale.y, newScale.z));
    transformMatrix = glm::translate(mat4(1.0f), position) * glm::mat4_cast(rotation) * newScaleMatrix;

    scale = newScale;
}


// @Utils --------------------------------------------------------
void Transform::DecomposeTransform()
{
    position = transformMatrix[3];

    for (int i = 0; i < 3; i++)
        scale[i] = glm::length(vec3(transformMatrix[i]));

    const glm::mat3 rotMtx(
        vec3(transformMatrix[0]) / scale[0],
        vec3(transformMatrix[1]) / scale[1],
        vec3(transformMatrix[2]) / scale[2]);

    rotation = glm::quat_cast(rotMtx);
}

void Transform::DecomposeGlobalTransform()
{
    globalPosition = globalTransformMatrix[3];

    for (int i = 0; i < 3; i++)
        globalScale[i] = glm::length(vec3(globalTransformMatrix[i]));

    const glm::mat3 rotMtx(
        vec3(globalTransformMatrix[0]) / globalScale[0],
        vec3(globalTransformMatrix[1]) / globalScale[1],
        vec3(globalTransformMatrix[2]) / globalScale[2]);

    globalRotation = glm::quat_cast(rotMtx);
}

mat4 Transform::CalculateWorldTransform()
{
    mat4 worldTransform = transformMatrix;
    GameObject* parent = containerGO.lock().get()->parent.lock().get();

    while (parent)
    {
        worldTransform = parent->GetComponent<Transform>()->transformMatrix * worldTransform;
        parent = parent->parent.lock().get();
    }

    globalTransformMatrix = worldTransform;

    return worldTransform;
}

mat4 Transform::GetGlobalTransform()
{
    return globalTransformMatrix;
}

mat4 Transform::WorldToLocalTransform(GameObject* GO, mat4 modifiedWorldTransform)
{
    return glm::inverse(GO->parent.lock().get()->GetComponent<Transform>()->CalculateWorldTransform()) * modifiedWorldTransform;
}

void Transform::ExtractBasis(const glm::mat4& transformMatrix, glm::mat3& basis)
{
    basis[0] = glm::vec3(transformMatrix[0][0], transformMatrix[0][1], transformMatrix[0][2]);
    basis[1] = glm::vec3(transformMatrix[1][0], transformMatrix[1][1], transformMatrix[1][2]);
    basis[2] = glm::vec3(transformMatrix[2][0], transformMatrix[2][1], transformMatrix[2][2]);
}

glm::vec3 Transform::ChangeBasis(const glm::vec3& rotationVectorA, const glm::mat3& basisA, const glm::mat3& basisB)
{
    // Calculate the rotation matrix from basis A to basis B
    glm::mat3 rotationMatrix = basisA * glm::inverse(basisB);

    // Apply the rotation matrix to the rotation vector from basis A
    return rotationMatrix * rotationVectorA;
}

void Transform::UpdateCameraIfPresent()
{
    Camera* camera = containerGO.lock().get()->GetComponent<Camera>();
    if (camera) { camera->UpdateCamera(); }
}



// @Get/Set ------------------------------------------------------
vec3 Transform::GetRight() const
{
    return glm::normalize(transformMatrix[0]);
}

void Transform::SetRight(vec3 newRight)
{
    transformMatrix[0] = vec4(newRight, 0.0f);
}

vec3 Transform::GetUp() const
{
	return glm::normalize(transformMatrix[1]);
}

void Transform::SetUp(vec3 newUp)
{
    transformMatrix[1] = vec4(newUp, 0.0f);
}

vec3 Transform::GetGlobalForward() const
{
    return glm::normalize(globalTransformMatrix[2]);
}

vec3 Transform::GetGlobalUp() const
{
    return glm::normalize(globalTransformMatrix[1]);
}

vec3 Transform::GetGlobalRight() const
{
    return glm::normalize(globalTransformMatrix[0]);
}

vec3 Transform::GetForward() const
{
	return glm::normalize(transformMatrix[2]);
}

void Transform::SetForward(vec3 newForward)
{
    transformMatrix[2] = vec4(newForward, 0.0f);
}

vec3 Transform::GetGlobalPosition() const
{
    return globalPosition;
}

quat Transform::GetGlobalRotation() const
{
    return globalRotation;
}

vec3 Transform::GetGlobalScale() const
{
    return globalScale;
}

vec3 Transform::GetPosition() const
{
	return position;
}

quat Transform::GetRotation() const
{
	return rotation;
}

vec3 Transform::GetScale() const
{
	return scale;
}

mat4 Transform::GetTransform() const
{
	return transformMatrix;
}

void Transform::SetTransform(mat4 transform)
{
    SetPosition({ transform[3][0], transform[3][1], transform[3][2] });
    this->transformMatrix = transform;
    DecomposeTransform();
}

vec3 Transform::GetRotationEuler() const
{
    return glm::eulerAngles(rotation);
}

json Transform::SaveComponent()
{
    json transformJSON;

    transformJSON["Name"] = name;
    transformJSON["Type"] = type;
    
    /*if (auto pGO = containerGO.lock())
        transformJSON["ParentUID"] = pGO.get()->GetUID();*/

    transformJSON["UID"] = UID;

    transformJSON["Transformation Matrix"] = { transformMatrix[0][0], transformMatrix[0][1], transformMatrix[0][2], transformMatrix[0][3],
                                                transformMatrix[1][0], transformMatrix[1][1], transformMatrix[1][2], transformMatrix[1][3],
                                                transformMatrix[2][0], transformMatrix[2][1], transformMatrix[2][2], transformMatrix[2][3],
                                                transformMatrix[3][0], transformMatrix[3][1], transformMatrix[3][2], transformMatrix[3][3] };

    return transformJSON;
}

// hekbas - Fix this
void Transform::LoadComponent(const json& transformJSON)
{
    // Load basic properties
    if (transformJSON.contains("UID"))
    {
        UID = transformJSON["UID"];
    }

    if (transformJSON.contains("Name"))
    {
        name = transformJSON["Name"];
    }

    // Load parent UID and set parent
    /*if (transformJSON.contains("ParentUID"))
    {
        uint32_t parentUID = transformJSON["ParentUID"];

        if (auto parentGameObject = SceneManager::GetInstance().FindGOByUID(parentUID))
        {
            containerGO = parentGameObject;
        }
    }*/

    // Load transformation properties
    if (transformJSON.contains("Transformation Matrix"))
    {
        mat4 temp;
        int it = 0;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                temp[i][j] = transformJSON["Transformation Matrix"][it];
                it++;
            }
        }

        SetTransform(temp);
        globalTransformMatrix = transformMatrix;
        CalculateWorldTransform();
        UpdateCameraIfPresent(); //Check if first creates camera component transform
    }
}
