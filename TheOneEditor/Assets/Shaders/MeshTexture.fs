#version 450 core

layout(location = 0) out vec4 gAlbedoSpec;
layout(location = 1) out vec3 gPosition;
layout(location = 2) out vec3 gNormal;

in vec2 TexCoords;
in vec3 fragPos;
in vec3 normal;

uniform sampler2D diffuse;

void main() {
    // store the fragment position vector in the first gbuffer texture
    gPosition = fragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(normal);
    // and the diffuse per-fragment color
    vec4 textureDiffuse = texture(diffuse, TexCoords);
    //Detect if the fragment has texture binded
    if(textureDiffuse.a < 0.1)
    {
        discard;
    }
    else
    {
        gAlbedoSpec = textureDiffuse;
    }
}