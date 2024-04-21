#version 450 core

// Outputs colors in RGBA
out vec4 FragColor;

uniform vec4 u_Color;

void main() {
	FragColor = u_Color;
}