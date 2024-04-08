#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

// Inputs the texture coordinates from the Vertex Shader
in vec2 texCoord;

// Gets the Texture Unit from the main function
uniform sampler2D tex;


void main()
{
	  FragColor = texture(tex, texCoord);
}