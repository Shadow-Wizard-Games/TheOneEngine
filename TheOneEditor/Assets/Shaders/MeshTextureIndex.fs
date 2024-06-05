#version 450 core

const vec4 MESH_INDEX_COLOR = vec4(0.298, 0.89, 1.0, 1.0);

layout(location = 0) out vec4 gIndex;

void main() {
    // store the index of the type of 3D element in the gbuffer
    gIndex = MESH_INDEX_COLOR;
}