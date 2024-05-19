#version 450 core

// Positions/Coordinates
layout(location = 0) in vec3 a_Pos;
// Normals
layout(location = 1) in vec3 a_Normal;

out vec3 normal;
out vec3 fragPos;

layout(std140, binding = 0) uniform Camera {
	mat4 u_ViewProjection;
};

uniform mat4 u_Model;

void main() {
	gl_Position = u_ViewProjection * u_Model * vec4(a_Pos, 1.0);
	fragPos = vec3(u_Model * vec4(a_Pos, 1.0));
	//TODO: Compute at the CPU!
	normal = mat3(transpose(inverse(u_Model))) * a_Normal;
}