#version 330 core

layout (location = 0) in vec3 v_pos;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;

void main()
{
    gl_Position = u_Proj * u_View * u_Model * vec4(v_pos, 1.0);
}