#include "Transform.h"
#include "GameObject.h"
#include "Camera.h"

Transform::Transform(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Transform),
    transformMatrix(1.0f),
    position(0.0f),
    rotation(1, 0, 0, 0),
    scale(1.0f)
{}

Transform::Transform(std::shared_ptr<GameObject> containerGO, mat4 transform) : Component(containerGO, ComponentType::Transform),
    transformMatrix(transform)
{
    DecomposeTransform();
}

Transform::~Transform() {}


// Transform ----------------------------------------------------

// To Implement on our Function
//void Transform::Move(vec3 displacement, Space referenceFrame)
//{
//    glm::mat3 referenceFrameMat = (glm::mat4)_transformationMatrix;
//
//    vec3 vecInRefFrame = referenceFrame == Space::GLOBAL ? displacement * referenceFrameMat : displacement;
//
//    _transformationMatrix = glm::translate(_transformationMatrix, vecInRefFrame);
//}

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

//void Transform::Translate(const vec3& translation, const HandleSpace& space)
//{
//    if (space == HandleSpace::GLOBAL)
//        transformMatrix = CalculateWorldTransform();
//
//    transformMatrix = glm::translate(transformMatrix, translation);
//    position = transformMatrix[3];
//
//    UpdateCameraIfPresent();
//}

// void Transform::SetPosition(const vec3& newPosition, const HandleSpace& space)
// {
// 	if (space == HandleSpace::GLOBAL)
// 		transformMatrix;
//
// 	transformMatrix[3] = glm::vec4(newPosition, 1.0f);
// 	position = transformMatrix[3];
//
// 	UpdateCameraIfPresent();
// }

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
}

//AQUI
//void Transform::RotateTo(double angle, vec3 axis)
//{
//	vec3 normalizedVec = glm::normalize(axis);
//
//	_right = vec3(1 * _scale.x, 0, 0);
//	_up = vec3(0, 1 * _scale.y, 0);
//	_forward = vec3(0, 0, 1 * _scale.z);
//
//	_transformationMatrix = glm::rotate(_transformationMatrix, glm::radians(angle), normalizedVec);
//}

//void Transform::RotateTo(vec3 rotVector)
//{
//	_right = vec3(1, 0, 0);
//	_up = vec3(0, 1, 0);
//	_forward = vec3(0, 0, 1);
//
//	_transformationMatrix = glm::rotate(_transformationMatrix, glm::radians(rotVector.x), vec3(1, 0, 0));
//	_transformationMatrix = glm::rotate(_transformationMatrix, glm::radians(rotVector.y), vec3(0, 1, 0));
//	_transformationMatrix = glm::rotate(_transformationMatrix, glm::radians(rotVector.z), vec3(0, 0, 1));
//
//	_right *= _scale.x;
//	_up *= _scale.y;
//	_forward *= _scale.z;
//}

//void Transform::Rotate(double angle, vec3 axis, Space referenceFrame)
//{
//	glm::mat3 referenceFrameMat = (glm::mat4)_transformationMatrix;
//
//	vec3 vecInRefFrame = glm::normalize(axis);
//	if (referenceFrame == Space::GLOBAL)
//		vecInRefFrame = axis * referenceFrameMat;
//
//	_transformationMatrix = glm::rotate(_transformationMatrix, glm::radians(angle), vecInRefFrame);
//}

//void Transform::SetRotation(const vec3& eulerAngles)
//{
//    transformMatrix[0] = vec4(1, 0, 0, transformMatrix[0][3]);
//    transformMatrix[1] = vec4(0, 1, 0, transformMatrix[1][3]);
//    transformMatrix[2] = vec4(0, 0, 1, transformMatrix[2][3]);
//
//    transformMatrix = glm::rotate(transformMatrix, eulerAngles.x, vec3(1, 0, 0));
//    transformMatrix = glm::rotate(transformMatrix, eulerAngles.y, vec3(0, 1, 0));
//    transformMatrix = glm::rotate(transformMatrix, eulerAngles.z, vec3(0, 0, 1));
//
//    transformMatrix[0][0] *= scale.x;
//    transformMatrix[0][1] *= scale.x;
//    transformMatrix[0][2] *= scale.x;
//
//    transformMatrix[1][0] *= scale.y;
//    transformMatrix[1][1] *= scale.y;
//    transformMatrix[1][2] *= scale.y;
//
//    transformMatrix[2][0] *= scale.z;
//    transformMatrix[2][1] *= scale.z;
//    transformMatrix[2][2] *= scale.z;
//
//    DecomposeTransform();
//}

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


//void Transform::SetRotation()
//{
//    glm::quat newRotation = ;
//
//	transformMatrix = glm::translate(
//		glm::mat4(1.0f), position) *
//		glm::mat4_cast(newRotation) *
//		scale;
//}

//hekbas, kiko - Need to fix
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


// Utils --------------------------------------------------------

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

mat4 Transform::CalculateWorldTransform()
{
    mat4 worldTransform = transformMatrix;
    GameObject* parent = containerGO.lock().get()->parent.lock().get();

    while (parent)
    {
        worldTransform = parent->GetComponent<Transform>()->transformMatrix * worldTransform;
        parent = parent->parent.lock().get();
    }

    return worldTransform;
}

mat4 Transform::WorldToLocalTransform(GameObject* GO, mat4 modifiedWorldTransform)
{
    return glm::inverse(GO->parent.lock().get()->GetComponent<Transform>()->CalculateWorldTransform()) * modifiedWorldTransform;
}

void Transform::RotateOMEGALUL(const vec3& eulerAngles, const HandleSpace& space)
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

void Transform::ExtractBasis(const glm::mat4& transformMatrix, glm::mat3& basis)
{
    basis[0] = glm::vec3(transformMatrix[0][0], transformMatrix[0][1], transformMatrix[0][2]);
    basis[1] = glm::vec3(transformMatrix[1][0], transformMatrix[1][1], transformMatrix[1][2]);
    basis[2] = glm::vec3(transformMatrix[2][0], transformMatrix[2][1], transformMatrix[2][2]);
}

// Function to change a rotation vector from basis A to basis B
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




// Get/Set ------------------------------------------------------

vec3 Transform::GetRight() const
{
    return glm::normalize(transformMatrix[0]);
}

vec3 Transform::GetUp() const
{
	return glm::normalize(transformMatrix[1]);
}

vec3 Transform::GetForward() const
{
	return glm::normalize(transformMatrix[2]);
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
    this->transformMatrix = transform;
}

vec3 Transform::GetRotationEuler() const
{
    return glm::eulerAngles(rotation);
}

#pragma region OLD_CODE
//void Transform::rotate(const vec3& axis, double angle, bool local)
//{
//    glm::quat rotationQuat = glm::angleAxis(glm::radians(angle), axis);
//
//    if (local) {
//        localRotation = rotationQuat;
//        //localRotation = glm::normalize(localRotation);
//        localEulerAngles = glm::eulerAngles(localRotation);
//    }
//    else {
//        rotation = rotationQuat;
//        //rotation = glm::normalize(rotation);
//        this->eulerAngles = glm::eulerAngles(rotation);
//    }
//}
//
//void Transform::rotate(const vec3& eulerAngles, bool local)
//{
//    glm::quat rotationQuat = glm::quat(glm::radians(eulerAngles));
//
//    if (local) {
//        localRotation = rotationQuat;
//        //localRotation = glm::normalize(localRotation);
//        localEulerAngles = glm::eulerAngles(localRotation);
//    }
//    else {
//        rotation = rotationQuat;
//        //rotation = glm::normalize(rotation);
//        this->eulerAngles = glm::eulerAngles(rotation);
//    }
//}
//
//void Transform::scaleBy(const vec3& scaling, bool local) 
//{
//    if (local) {
//        localScale *= scaling;
//    }
//    else {
//        scale *= scaling;
//    }
//}
//
//// Get / Set ----------------------------------------------------
//vec3 Transform::getForward() 
//{
//    updateMatrix();
//    return glm::normalize(transformMatrix[2]);
//}
//
//vec3 Transform::getUp() 
//{
//    updateMatrix();
//    return glm::normalize(transformMatrix[1]);
//}
//
//vec3 Transform::getRight() 
//{
//    updateMatrix();
//    return glm::normalize(transformMatrix[0]);
//}
//
//mat4 Transform::getMatrix() 
//{
//    updateMatrix();
//    return transformMatrix;
//}
//
//void Transform::updateMatrix()
//{
//    transformMatrix = mat4(1.0f);
//    transformMatrix = glm::translate(transformMatrix, position);
//    transformMatrix *= glm::mat4_cast(rotation * localRotation);
//    transformMatrix = glm::scale(transformMatrix, localScale);
//    transformMatrix = glm::scale(transformMatrix, scale);
//}
//
//vec3 Transform::getPosition() const
//{
//    return position;
//}
//
//void Transform::setPosition(const vec3& newPosition) 
//{
//    position = newPosition;
//}
//
//quat Transform::getRotation() const
//{
//    return rotation;
//}
//
//quat Transform::getLocalRotation() const
//{
//    return localRotation;
//}
//
//vec3 Transform::getEulerAngles() const
//{
//    vec3 eulerAngles = glm::eulerAngles(rotation);
//    return eulerAngles;
//}
//
//vec3 Transform::getLocalEulerAngles() const
//{
//    vec3 eulerAngles = glm::eulerAngles(localRotation);
//    return eulerAngles;
//}
//
//void Transform::setRotation(const vec3& newRotation)
//{
//    eulerAngles = newRotation;
//    rotation = EulerAnglesToQuaternion(eulerAngles);
//}
//
//vec3 Transform::getScale() const
//{
//    return scale;
//}
//
//void Transform::setScale(const vec3& newScale)
//{
//    scale = newScale;
//}
//
//
//quat Transform::EulerAnglesToQuaternion(const vec3& eulerAngles)
//{
//    quat quaternion;
//    quaternion = glm::angleAxis(eulerAngles.z, vec3(0, 0, 1));     // Rotate around the Z-axis (yaw)
//    quaternion *= glm::angleAxis(eulerAngles.y, vec3(0, 1, 0));    // Rotate around the Y-axis (pitch)
//    quaternion *= glm::angleAxis(eulerAngles.x, vec3(1, 0, 0));    // Rotate around the X-axis (roll)
//    return quaternion;
//}
#pragma endregion OLD_CODE

json Transform::SaveComponent()
{
    json transformJSON;

    /*transformJSON["Name"] = name;
    transformJSON["Type"] = type;
    if (auto pGO = containerGO.lock())
    {
        transformJSON["ParentUID"] = pGO.get()->GetUID();
    }
    transformJSON["UID"] = UID;
    transformJSON["Position"] = { position.x, position.y, position.z };
    transformJSON["Rotation"] = { rotation.w, rotation.x, rotation.y, rotation.z };
    transformJSON["LocalRotation"] = { localRotation.w, localRotation.x, localRotation.y, localRotation.z };
    transformJSON["Scale"] = { scale.x, scale.y, scale.z };
    transformJSON["LocalScale"] = { localScale.x, localScale.y, localScale.z };*/

    return transformJSON;
}

void Transform::LoadComponent(const json& transformJSON)
{
    //// Load basic properties
    //if (transformJSON.contains("UID"))
    //{
    //    UID = transformJSON["UID"];
    //}

    //if (transformJSON.contains("Name"))
    //{
    //    name = transformJSON["Name"];
    //}

    //// Load parent UID and set parent
    ///*if (transformJSON.contains("ParentUID"))
    //{
    //    uint32_t parentUID = transformJSON["ParentUID"];

    //    if (auto parentGameObject = SceneManager::GetInstance().FindGOByUID(parentUID))
    //    {
    //        containerGO = parentGameObject;
    //    }
    //}*/

    //// Load transformation properties
    //if (transformJSON.contains("Position"))
    //{
    //    const auto& positionArray = transformJSON["Position"];
    //    position = { positionArray[0], positionArray[1], positionArray[2] };
    //}

    //if (transformJSON.contains("Rotation"))
    //{
    //    const auto& rotationArray = transformJSON["Rotation"];
    //    rotation = quat(rotationArray[1], rotationArray[2], rotationArray[3], rotationArray[0]);
    //}

    //if (transformJSON.contains("LocalRotation"))
    //{
    //    const auto& localRotationArray = transformJSON["LocalRotation"];
    //    localRotation = quat(localRotationArray[1], localRotationArray[2], localRotationArray[3], localRotationArray[0]);
    //}

    //if (transformJSON.contains("Scale"))
    //{
    //    const auto& scaleArray = transformJSON["Scale"];
    //    scale = { scaleArray[0], scaleArray[1], scaleArray[2] };
    //}

    //if (transformJSON.contains("LocalScale"))
    //{
    //    const auto& localScaleArray = transformJSON["LocalScale"];
    //    localScale = { localScaleArray[0], localScaleArray[1], localScaleArray[2] };
    //}

    //// Update the transformation matrix
    //updateMatrix();
}
