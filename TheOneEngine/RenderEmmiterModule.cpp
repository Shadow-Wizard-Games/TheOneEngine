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

    particle->rotation = Billboard::CalculateSphericalBillboardRotation(particlePosition, cameraPosition);


    glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(particlePosition))
        * glm::rotate(glm::mat4(1.0f), float(glm::radians(particle->rotation.w)), glm::vec3(particle->rotation))
        * glm::scale(glm::mat4(1.0f),  glm::vec3(particle->scale));

    Renderer2D::DrawQuad(transform, glm::vec4(particle->color));
    //Debug
    /*Renderer2D::DrawQuad(glm::vec3(particle->position), glm::vec2(particle->scale), glm::vec4(1));
    Renderer2D::DrawQuad({ particle->position.x, particle->position.y }, { particle->scale.x, particle->scale.y }, glm::vec4(1));
    Renderer2D::DrawQuad({ 10.0f, 10.0f }, { 1.0f, 1.0f }, glm::vec4(1));*/

    glEnable(GL_CULL_FACE);

}

json BillboardRender::SaveModule()
{
    json moduleJSON;

    moduleJSON["Type"] = type;

    moduleJSON["BillboardType"] = billboardType;

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
}
