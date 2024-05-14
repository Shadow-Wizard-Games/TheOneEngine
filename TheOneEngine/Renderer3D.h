#pragma once
#include "Defs.h"

struct OpenGLMesh
{
	unsigned int meshID;
	unsigned int textureID;
	glm::mat4 model;
};

struct InstanceCall
{
	std::vector<glm::mat4> models;
};

class Renderer3D
{
public:
	static void Update();

	static void AddMesh(const OpenGLMesh& mesh);

	static void AddMeshToQueue();

private:
	static void AddInstanceCall();
};