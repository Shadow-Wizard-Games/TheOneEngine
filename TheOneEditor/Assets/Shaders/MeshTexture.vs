#version 450 core

// Positions/Coordinates
layout (location = 0) in vec3 pos;
// Texture Coordinates
layout (location = 1) in vec2 texCoords;


// Outputs the color for the Fragment Shader
out vec3 color;
// Outputs the texture coordinates to the fragment shader
out vec2 texCoord;

uniform mat4 model;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};


void main()
{
	gl_Position = u_ViewProjection * model * vec4(pos, 1.0);

	// Assigns the texture coordinates from the Vertex Data to "texCoord"
	texCoord = texCoords;
}