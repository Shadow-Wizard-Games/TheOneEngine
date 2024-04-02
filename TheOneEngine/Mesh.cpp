#include "Mesh.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"

#include "Log.h"

#include <span>
#include <vector>
#include <array>
#include <cstdio>
#include <cassert>


Mesh::Mesh(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Mesh)
{
    active = true;
    drawNormalsFaces = false;
    drawNormalsVerts = false;
    drawAABB = true;
    drawChecker = false;

    normalLineWidth = 1;
    normalLineLength = 0.1f;
    meshLoader = new MeshLoader();
    //GenerateAABB();
}

Mesh::Mesh(std::shared_ptr<GameObject> containerGO, Mesh* ref) : Component(containerGO, ComponentType::Mesh)
{
    active = ref->active;
    drawNormalsFaces = ref->drawNormalsFaces;
    drawNormalsVerts = ref->drawNormalsVerts;
    drawAABB = ref->drawAABB;
    drawChecker = ref->drawChecker;

    mesh = ref->mesh;
    mesh.texture = ref->mesh.texture;

    meshData = ref->meshData;
    meshLoader = ref->meshLoader;

    normalLineWidth = ref->normalLineWidth;
    normalLineLength = ref->normalLineLength;
    meshLoader = new MeshLoader();
    //GenerateAABB();
}

Mesh::~Mesh()
{
    RELEASE(meshLoader);
    /*if (_vertex_buffer_id) glDeleteBuffers(1, &_vertex_buffer_id);
    if (_indexs_buffer_id) glDeleteBuffers(1, &_indexs_buffer_id);*/
}


// Draw
void Mesh::DrawComponent(Camera* camera)
{
    glLineWidth(1);
    glColor4ub(255, 255, 255, 255); 

    glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer_id);
    glEnableClientState(GL_VERTEX_ARRAY);

    std::shared_ptr<GameObject> containerGO = GetContainerGO();
    glPushMatrix();
    glMultMatrixd(&containerGO.get()->GetComponent<Transform>()->CalculateWorldTransform()[0].x);

    ConfigureVertexFormat();

    if (active)
    {
        if (drawWireframe)
        {
            glDisable(GL_TEXTURE_2D);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glEnable(GL_TEXTURE_2D);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        if (mesh.indexs_buffer_id)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexs_buffer_id);
            glDrawElements(GL_TRIANGLES, mesh.numIndexs, GL_UNSIGNED_INT, nullptr);
        }
        else
            glDrawArrays(GL_TRIANGLES, 0, mesh.numVerts);

        if (drawNormalsVerts) DrawVertexNormals();
        if (drawNormalsFaces) DrawFaceNormals();


        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);
    }
    
    GLenum error = glGetError();

    glPopMatrix();
}

void Mesh::ConfigureVertexFormat()
{
    switch (mesh.format)
    {
    case Formats::F_V3:
        glVertexPointer(3, GL_FLOAT, 0, nullptr);
        break;

    case Formats::F_V3C4:
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(V3C4), nullptr);
        glColorPointer(4, GL_FLOAT, sizeof(V3C4), (void*)sizeof(V3));
        break;

    case Formats::F_V3T2:
        glEnable(GL_TEXTURE_2D);
        if (mesh.texture.get() && !drawChecker) mesh.texture->bind();
        //else mesh.checkboard.get()->bind();

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(V3T2), nullptr);
        glTexCoordPointer(2, GL_FLOAT, sizeof(V3T2), (void*)sizeof(V3));
        break;
    }
}

void Mesh::DrawVertexNormals()
{
    if (meshData.meshVerts.empty() || meshData.meshNorms.empty()) return;

    glLineWidth(normalLineWidth);
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);

    for (int i = 0; i < meshData.meshVerts.size(); i++)
    {
        glVertex3f(meshData.meshVerts[i].x, meshData.meshVerts[i].y, meshData.meshVerts[i].z);
        glVertex3f(meshData.meshVerts[i].x + meshData.meshNorms[i].x * normalLineLength,
            meshData.meshVerts[i].y + meshData.meshNorms[i].y * normalLineLength,
            meshData.meshVerts[i].z + meshData.meshNorms[i].z * normalLineLength);
    }

    glColor3f(1.0f, 1.0f, 0.0f);
    glEnd();
}

void Mesh::DrawFaceNormals() 
{
    if (meshData.meshFaceCenters.empty() || meshData.meshFaceNorms.empty()) return;

    glLineWidth(normalLineWidth);
    glColor3f(1.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);

    for (int i = 0; i < meshData.meshFaceCenters.size(); i++)
    {
        glm::vec3 endPoint = meshData.meshFaceCenters[i] + normalLineLength * meshData.meshFaceNorms[i];
        glVertex3f(meshData.meshFaceCenters[i].x, meshData.meshFaceCenters[i].y, meshData.meshFaceCenters[i].z);
        glVertex3f(endPoint.x, endPoint.y, endPoint.z);
    }

    glColor3f(0.0f, 1.0f, 1.0f);
    glEnd();
}


// Load/Save
json Mesh::SaveComponent()
{
    json meshJSON;

    meshJSON["Name"] = name;
    meshJSON["Type"] = type;
    if (auto pGO = containerGO.lock())
    {
        meshJSON["ParentUID"] = pGO.get()->GetUID();
    }
    meshJSON["UID"] = UID;
    meshJSON["Active"] = active;
    meshJSON["DrawWireframe"] = drawWireframe;
    meshJSON["DrawAABB"] = drawAABB;
    meshJSON["DrawOBB"] = drawOBB;
    meshJSON["DrawChecker"] = drawChecker;
    meshJSON["DrawNormalsVerts"] = drawNormalsVerts;
    meshJSON["DrawNormalsFaces"] = drawNormalsFaces;
    meshJSON["Path"] = path;

    //MeshData && MeshBufferedData are already serialized in .mesh files

    return meshJSON;
}

void Mesh::LoadComponent(const json& meshJSON)
{
    // Load basic properties
    if (meshJSON.contains("UID"))
    {
        UID = meshJSON["UID"];
    }

    if (meshJSON.contains("Name"))
    {
        name = meshJSON["Name"];
    }

    // Load mesh-specific properties
    if (meshJSON.contains("Active"))
    {
        active = meshJSON["Active"];
    }

    if (meshJSON.contains("DrawWireframe"))
    {
        drawWireframe = meshJSON["DrawWireframe"];
    }

    if (meshJSON.contains("DrawAABB"))
    {
        drawAABB = meshJSON["DrawAABB"];
    }

    if (meshJSON.contains("DrawOBB"))
    {
        drawOBB = meshJSON["DrawOBB"];
    }

    if (meshJSON.contains("DrawChecker"))
    {
        drawChecker = meshJSON["DrawChecker"];
    }

    if (meshJSON.contains("DrawNormalsVerts"))
    {
        drawNormalsVerts = meshJSON["DrawNormalsVerts"];
    }

    if (meshJSON.contains("DrawNormalsFaces"))
    {
        drawNormalsFaces = meshJSON["DrawNormalsFaces"];
    }

    if (meshJSON.contains("Path"))
    {
        path = meshJSON["Path"];
    }

    // Implement additional logic to handle other mesh-specific properties as needed
    // ...

    //Reinitialize or update the mesh based on the loaded data

    if (!path.empty())
    {
        meshData = meshLoader->deserializeMeshData(path);
        meshLoader->BufferData(meshData);
        mesh = meshLoader->GetBufferData();
        if(!meshData.texturePath.empty())
            mesh.texture = std::make_shared<Texture>(meshData.texturePath);
    }

}