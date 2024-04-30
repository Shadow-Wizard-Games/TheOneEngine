#version 450 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in vec4 a_Color;

//out vec3 v_world_normal;
//out vec4 v_vertex_color;

out vec3 normal;
out vec3 fragPos;
out vec2 v_Vertex_UV;

layout(std140, binding = 0) uniform Camera {
	mat4 u_ViewProjection;
};

uniform mat4 u_Model;

void main() {

	gl_Position = u_ViewProjection * u_Model * vec4(a_Pos, 1.0);
	fragPos = vec3(u_Model * vec4(a_Pos, 1.0));
	//TODO: Compute at the CPU!
	normal = mat3(transpose(inverse(u_Model))) * a_Normal;
	v_Vertex_UV = a_UV;

	//mat3 cross_matrix = mat3(
	//	cross(u_Model[1].xyz, u_Model[2].xyz),
	//	cross(u_Model[2].xyz, u_Model[0].xyz),
	//	cross(u_Model[0].xyz, u_Model[1].xyz));

	//float invdet = 1.0 / dot(cross_matrix[2], u_Model[2].xyz);
	//mat3 normal_matrix = cross_matrix * invdet;

	//v_world_normal = normal_matrix * a_normal;

	//v_vertex_color = a_color;

	//Normal =(world_matrix * vec4(a_normal, 0)).xyz;

	//LocalPos = vec3(world_matrix * vec4(a_Pos, 1.0f));

	//LightSpacePos = u_LightMVP * vec4(LocalPos, 1.0);
}