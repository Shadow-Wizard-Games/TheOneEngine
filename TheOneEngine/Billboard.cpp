#include "Billboard.h"
#include "Types.h"
#include "GL/glew.h"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

// ideally you'd want to have a model matrix with just the camera lookup set
void Billboard::BeginCylindricBillboard(vec3 objectPosition, vec3 cameraPosition)
{
	// from http://www.lighthouse3d.com/opengl/billboarding/billboardingtut.pdf

	vec3 lookAt, objToCamProj, upAux;
	float angleCosine;
	//glPushMatrix();
	// glLoadIdentity();
	// objToCamProj is the vector in world coordinates from the
	// local origin to the camera projected in the XZ plane
	objToCamProj.x = cameraPosition.x - objectPosition.x;
	objToCamProj.y = 0;
	objToCamProj.z = cameraPosition.z - objectPosition.z;
	// This is the original lookAt vector for the object
	// in world coordinates
	lookAt.x = 0;
	lookAt.y = 0;
	lookAt.z = 1;

	// normalize both vectors to get the cosine directly afterwards
	glm::normalize(objToCamProj);
	// easy fix to determine wether the angle is negative or positive
	// for positive angles upAux will be a vector pointing in the
	// positive y direction, otherwise upAux will point downwards
	// effectively reversing the rotation.
	upAux = glm::cross(lookAt, objToCamProj);

	// compute the angle
	angleCosine = glm::dot(lookAt, objToCamProj) / (glm::l2Norm(lookAt) * glm::l2Norm(objToCamProj));
	// perform the rotation. The if statement is used for stability reasons
		// if the lookAt and objToCamProj vectors are too close together then
		// |angleCosine| could be bigger than 1 due to lack of precision
	if ((angleCosine < 0.99999) && (angleCosine > -0.99999))
		glRotatef(acos(angleCosine) * 180 / 3.14, upAux[0], upAux[1], upAux[2]);
}

mat4 Billboard::CalculateSphericalBillboardRotationMatrix(const vec3& objectPosition, const vec3& cameraPosition) {
	vec3 lookAt(0.0f, 0.0f, 1.0f);
	vec3 objToCamProj(cameraPosition.x - objectPosition.x, 0, cameraPosition.z - objectPosition.z);
	objToCamProj = glm::normalize(objToCamProj);

	vec3 upAux = glm::cross(lookAt, objToCamProj);
	double angleCosine = glm::dot(lookAt, objToCamProj);

	vec4 verticalRotation(1.0);
	if (std::abs(angleCosine) < 0.99999f) {
		double angle = acos(angleCosine) * 180 / glm::pi<float>();
		verticalRotation = vec4(upAux, angle);
	}

	vec3 objToCam = glm::normalize(cameraPosition - objectPosition);

	vec4 tiltRotation(1.0);
	angleCosine = glm::dot(objToCamProj, objToCam);
	if (std::abs(angleCosine) < 0.99999f) {
		double tiltAngle = acos(angleCosine) * 180 / glm::pi<float>();
		vec3 tiltAxis((objToCam.y < 0) ? 1 : -1, 0, 0);
		tiltRotation = vec4(tiltAxis, tiltAngle);
	}

	mat4 vertical = glm::rotate(mat4(1.0), glm::radians(verticalRotation.w), vec3(verticalRotation));
	mat4 tilt = glm::rotate(mat4(1.0), glm::radians(tiltRotation.w), vec3(tiltRotation));

	return vertical * tilt;
}
