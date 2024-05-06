#include "RenderEmmiterModule.h"
#include "Emmiter.h"
#include "ParticleSystem.h"
#include "Transform.h"
#include "Billboard.h"
#include "GL/glew.h"
#include "Renderer2D.h"

BillboardRender::BillboardRender(Emmiter* owner)
{
    type = BILLBOARD;
    this->owner = owner;

    billboardType = SCREEN_ALIGNED;
}

BillboardRender::BillboardRender(Emmiter* owner, BillboardRender* ref)
{
    type = BILLBOARD;
    this->owner = owner;

    billboardType = ref->billboardType;
}

void BillboardRender::Update(Particle* particle, Camera* camera)
{
    glDisable(GL_CULL_FACE);

    vec3 cameraPosition = camera->GetContainerGO()->GetComponent<Transform>()->GetPosition();

    vec3 particlePosition = particle->position;

    if (!owner->isGlobal) {
        mat4 worldTransform = owner->owner->GetTransform()->GetGlobalTransform();

        glm::dmat3 worldRotation = worldTransform;

        vec3 worldPosition = worldTransform[3];

        particlePosition = (worldRotation * particle->position) + worldPosition;
    }

    mat4 transform = glm::translate(mat4(1.0f), particlePosition)
        * Billboard::CalculateSphericalBillboardRotationMatrix(particlePosition, cameraPosition)
        * glm::scale(mat4(1.0f), particle->scale);

    if(textureID != -1)
        Renderer2D::DrawQuad(transform, textureID);
    else
        Renderer2D::DrawQuad(transform, glm::vec4(particle->color));
        
    glEnable(GL_CULL_FACE);

}

json BillboardRender::SaveModule()
{
    json moduleJSON;

    moduleJSON["Type"] = type;

    moduleJSON["BillboardType"] = billboardType;

    moduleJSON["TexturePath"] = texturePath;

    return moduleJSON;
}

void BillboardRender::LoadModule(const json& moduleJSON)
{
    if (moduleJSON.contains("Type"))
    {
        type = moduleJSON["Type"];
    }

    if (moduleJSON.contains("BillboardType"))
    {
        billboardType = moduleJSON["BillboardType"];
    }

    if (moduleJSON.contains("TexturePath")) texturePath = moduleJSON["TexturePath"];
    textureID = Resources::Load<Texture>(texturePath);
}

void RenderEmmiterModule::SetTexture(const std::string& filename)
{
    textureID = Resources::Load<Texture>(filename);
    if (textureID != -1)
        texturePath = filename;
}
