#version 450 core

const vec4 PARTICLE_INDEX_COLOR = vec4(0.9, 0.9, 0.9, 1.0);

layout(location = 0) out vec4 gIndex;

void main()
{
    // store the index of the type of 3D element in the gbuffer
    gIndex = PARTICLE_INDEX_COLOR;
}
