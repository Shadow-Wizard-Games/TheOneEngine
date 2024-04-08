#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 pos;
// Texture Coordinates
layout (location = 1) in vec2 texCoords;


// Outputs the color for the Fragment Shader
out vec3 color;
// Outputs the texture coordinates to the fragment shader
out vec2 texCoord;


//Necesary to render the model DON'T touch
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;




void main()
{
	gl_Position = projection * view * model * vec4(pos, 1.0);

	// Assigns the texture coordinates from the Vertex Data to "texCoord"
	texCoord = texCoords;
}