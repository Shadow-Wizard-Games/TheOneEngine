#version 450 core

layout(location = 0) in vec2 a_UV;
layout(location = 1) in vec3 a_Pos;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec4 a_color;

out vec2 v_Vertex_UV;
//out vec3 v_world_normal;
//out vec4 v_vertex_color;

//out vec3 Normal;
//out vec3 LocalPos;
//out vec4 LightSpacePos;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

uniform mat4 u_Model;

//uniform mat4 u_LightMVP;

void main() {
	
	gl_Position = u_ViewProjection * u_Model * vec4(a_Pos, 1.0);

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