#version 450 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in mat4 a_InstanceModel;

out vec3 normal;
out vec3 fragPos;
out vec2 TexCoords;

layout(std140, binding = 0) uniform Camera {
	mat4 u_ViewProjection;
};

void main() {
	gl_Position = u_ViewProjection * a_InstanceModel * vec4(a_Pos, 1.0);
	fragPos = vec3(a_InstanceModel * vec4(a_Pos, 1.0));
	//TODO: Compute at the CPU!
	normal = mat3(transpose(inverse(a_InstanceModel))) * a_Normal;
	TexCoords = a_UV;
}