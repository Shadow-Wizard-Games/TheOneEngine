#version 450 core

// Positions/Coordinates
layout (location = 0) in vec3 a_Pos;
// Texture Coordinates
layout (location = 1) in vec2 a_UV;

// Outputs the texture coordinates to the fragment shader
out vec2 v_Vertex_uv;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

uniform mat4 u_Model;

void main()
{
	gl_Position = u_ViewProjection * u_Model * vec4(a_Pos, 1.0);

	// Assigns the texture coordinates from the Vertex Data to "texCoord"
	v_Vertex_uv = a_UV;
}