#version 450 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normal;

out vec3 normal;
out vec3 fragPos;
out vec2 TexCoords;

layout(std140, binding = 0) uniform Camera {
	mat4 u_ViewProjection;
};

uniform mat4 u_Model;

void main() {
	gl_Position = u_ViewProjection * u_Model * vec4(a_Pos, 1.0);
	fragPos = vec3(u_Model * vec4(a_Pos, 1.0));
	//TODO: Compute at the CPU!
	normal = mat3(transpose(inverse(u_Model))) * a_Normal;
	TexCoords = a_UV;
}