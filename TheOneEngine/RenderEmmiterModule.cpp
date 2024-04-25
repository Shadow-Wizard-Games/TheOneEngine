#include "RenderEmmiterModule.h"
#include "Emmiter.h"
#include "ParticleSystem.h"
#include "Transform.h"
#include "Billboard.h"
#include "GL/glew.h"

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

    const float* viewProjectionMatrix = glm::value_ptr(camera->viewProjectionMatrix);

    glPushMatrix();
    //glLoadIdentity();
    glLoadMatrixf(viewProjectionMatrix);

    vec3 cameraPosition;
    cameraPosition = camera->GetContainerGO()->GetComponent<Transform>()->GetPosition();

    vec3 particlePosition = particle->position;

    if (!owner->isGlobal) {
        mat4 worldTransform = owner->owner->GetTransform()->GetGlobalTransform();

        glm::dmat3 worldRotation = worldTransform;

        vec3 worldPosition = worldTransform[3];

        particlePosition = (worldRotation * particle->position) + worldPosition;
    }

    Billboard::BeginSphericalBillboard(particlePosition, cameraPosition);

    // render
    glBegin(GL_TRIANGLES);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4ub(particle->color.r, particle->color.g, particle->color.b, particle->color.a);

    glVertex3f(- (1 * particle->scale.x), + (1 * particle->scale.y), 0);
    glVertex3f(- (1 * particle->scale.x), - (1 * particle->scale.y), 0);
    glVertex3f(+ (1 * particle->scale.x), + (1 * particle->scale.y), 0);
             
    glVertex3f(- (1 * particle->scale.x), - (1 * particle->scale.y), 0);
    glVertex3f(+ (1 * particle->scale.x), - (1 * particle->scale.y), 0);
    glVertex3f(+ (1 * particle->scale.x), + (1 * particle->scale.y), 0);

    glEnd();

    // particle->Render();

    Billboard::EndBillboard();

    //glPopMatrix();

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
