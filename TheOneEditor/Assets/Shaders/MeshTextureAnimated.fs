#version 450 core

const vec4 SKELETAL_INDEX_COLOR = vec4(0.537, 1.0, 0.298, 1.0);

layout(location = 0) out vec4 gAlbedoSpec;
layout(location = 1) out vec3 gPosition;
layout(location = 2) out vec3 gNormal;
layout(location = 3) out vec4 gIndex;

in vec2 TexCoords;
in vec3 fragPos;
in vec3 normal;

uniform sampler2D diffuse;

void main() {
    // store the index of the type of 3D element in the gbuffer
    gIndex = SKELETAL_INDEX_COLOR;
    // store the fragment position vector in the first gbuffer texture
    gPosition = fragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(normal);
    // and the diffuse per-fragment color
    vec4 textureDiffuse = texture(diffuse, vec2(TexCoords.s, 1. - TexCoords.t));
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