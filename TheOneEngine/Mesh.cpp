#include "Mesh.h"

Mesh::Mesh(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Mesh) {}

Mesh::~Mesh() {}

void Mesh::DrawMesh(MeshLoader::MeshBufferedData mesh)
{
    glColor4ub(255, 255, 255, 255);

    // WIREFRAME MODE, comment the line below for fill mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer_id);
    glEnableClientState(GL_VERTEX_ARRAY);

    switch (mesh.format)
    {
    case MeshLoader::Formats::F_V3:
        glVertexPointer(3, GL_FLOAT, 0, nullptr);
        break;

    case MeshLoader::Formats::F_V3C4:
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(MeshLoader::V3C4), nullptr);
        glColorPointer(4, GL_FLOAT, sizeof(MeshLoader::V3C4), (void*)sizeof(MeshLoader::V3));
        break;

    case MeshLoader::Formats::F_V3T2:
        glEnable(GL_TEXTURE_2D);
        if (texture.get()) texture->bind();
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(MeshLoader::V3T2), nullptr);
        glTexCoordPointer(2, GL_FLOAT, sizeof(MeshLoader::V3T2), (void*)sizeof(MeshLoader::V3));
        break;
    }

    if (mesh.indexs_buffer_id) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexs_buffer_id);
        glDrawElements(GL_TRIANGLES, mesh.numIndexs, GL_UNSIGNED_INT, nullptr);
    }
    else {
        glDrawArrays(GL_TRIANGLES, 0, mesh.numVerts);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
}