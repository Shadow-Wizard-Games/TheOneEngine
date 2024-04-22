#version 450 core

// Outputs colors in RGBA
out vec4 FragColor;

// Inputs the texture coordinates from the Vertex Shader
in vec2 v_Vertex_uv;

// Gets the Texture Unit from the main function
uniform sampler2D u_Tex;


void main()
{
	  FragColor = texture(u_Tex, v_Vertex_uv);
}