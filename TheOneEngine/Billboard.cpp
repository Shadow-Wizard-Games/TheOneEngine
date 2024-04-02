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

// ideally you'd want to have a model matrix with just the camera lookup set
void Billboard::BeginSphericalBillboard(vec3 objectPosition, vec3 cameraPosition)
{
	// from http://www.lighthouse3d.com/opengl/billboarding/billboardingtut.pdf

	glTranslated(objectPosition.x, objectPosition.y, objectPosition.z);

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
		glRotated(acos(angleCosine) * 180 / 3.14, upAux[0], upAux[1], upAux[2]);

	// so far it is just like the cylindrical billboard. The code for the
	// second rotation comes now
	// The second part tilts the object so that it faces the camera
	// objToCam is the vector in world coordinates from
	// the local origin to the camera
	vec3 objToCam;
	objToCam.x = cameraPosition.x - objectPosition.x;
	objToCam.y = cameraPosition.y - objectPosition.y;
	objToCam.z = cameraPosition.z - objectPosition.z;
	// Normalize to get the cosine afterwards
	glm::normalize(objToCam);
	// Compute the angle between objToCamProj and objToCam,
	//i.e. compute the required angle for the lookup vector
	angleCosine = glm::dot(objToCamProj, objToCam) / (glm::l2Norm(objToCamProj) * glm::l2Norm(objToCam));
	// Tilt the object. The test is done to prevent instability
	// when objToCam and objToCamProj have a very small
	// angle between them
	if ((angleCosine < 0.99999) && (angleCosine > -0.99999))
		if (objToCam[1] < 0)
			glRotated(acos(angleCosine) * 180 / 3.14, 1, 0, 0);
		else
			glRotated(acos(angleCosine) * 180 / 3.14, -1, 0, 0);
}

void Billboard::EndBillboard()
{
	glPopMatrix();

}
