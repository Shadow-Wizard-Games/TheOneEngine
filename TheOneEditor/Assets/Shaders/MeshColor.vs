#version 450 core

layout (location = 0) in vec3 v_pos;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

uniform mat4 model;

void main()
{
    gl_Position = u_ViewProjection * model * vec4(v_pos, 1.0);
}