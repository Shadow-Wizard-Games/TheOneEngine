#version 330 core

layout (location = 0) in vec3 v_pos;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

uniform mat4 u_Model;

void main()
{
    gl_Position = u_ViewProjection * u_Model * vec4(v_pos, 1.0);
}