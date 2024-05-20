#include "Light.h"
#include "EngineCore.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "Material.h"
#include "N_SceneManager.h"
#include "Shader.h"
#include "FrameBuffer.h"


Light::Light(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Light), lightType(LightType::Point),
color(1.0f), specular(0.5f), linear(0.7f), quadratic(1.8f), innerCutOff(0.91f), outerCutOff(0.82f)
{
    const float maxBrightness = std::fmaxf(std::fmaxf(color.r, color.g), color.b);
    radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (1.0f - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);

    std::vector<Attachment> depthBuffAttachments = {
            { Attachment::Type::DEPTH, "depth", 0 }
    };

    depthBuffer = std::make_shared<FrameBuffer>(1280, 720, depthBuffAttachments);

    engine->N_sceneManager->currentScene->lights.push_back(this);
}

Light::Light(std::shared_ptr<GameObject> containerGO, Light* ref) : Component(containerGO, ComponentType::Light), lightType(ref->lightType),
color(ref->color), specular(ref->specular), radius(ref->radius), linear(ref->linear), quadratic(ref->quadratic), 
innerCutOff(ref->innerCutOff), outerCutOff(ref->outerCutOff)
{
    engine->N_sceneManager->currentScene->lights.push_back(this);
}

Light::~Light() 
{
}

void Light::DrawComponent(Camera* camera)
{
}

json Light::SaveComponent()
{
    json lightJSON;

    lightJSON["Name"] = name;
    lightJSON["Type"] = type;
    if (auto pGO = containerGO.lock())
    {
        lightJSON["ParentUID"] = pGO.get()->GetUID();
    }
    lightJSON["LightType"] = lightType;
    lightJSON["UID"] = UID;
    lightJSON["Color"] = { color.r, color.g, color.b};
    lightJSON["Specular"] = specular;
    lightJSON["Radius"] = radius;
    lightJSON["Linear"] = linear;
    lightJSON["Quadratic"] = quadratic;
    lightJSON["InnerCutOff"] = innerCutOff;
    lightJSON["OuterCutOff"] = outerCutOff;

    return lightJSON;
}

void Light::LoadComponent(const json& meshJSON)
{
    if (meshJSON.contains("Name"))
    {
        name = meshJSON["Name"];
    }
    if (meshJSON.contains("LightType"))
    {
        lightType = meshJSON["LightType"];
    }
    if (meshJSON.contains("UID"))
    {
        UID = meshJSON["UID"];
    }
    if (meshJSON.contains("Color"))
    {
        color.r = meshJSON["Color"][0];
        color.g = meshJSON["Color"][1];
        color.b = meshJSON["Color"][2];
    }
    if (meshJSON.contains("Specular"))
    {
        specular = meshJSON["Specular"];
    }
    if (meshJSON.contains("Radius"))
    {
        radius = meshJSON["Radius"];
    }
    if (meshJSON.contains("Linear"))
    {
        linear = meshJSON["Linear"];
    }
    if (meshJSON.contains("Quadratic"))
    {
        quadratic = meshJSON["Quadratic"];
    }
    if (meshJSON.contains("InnerCutOff"))
    {
        innerCutOff = meshJSON["InnerCutOff"];
    }
    if (meshJSON.contains("OuterCutOff"))
    {
        outerCutOff = meshJSON["OuterCutOff"];
    }
}

void Light::RemoveLight()
{
    auto it = std::find(engine->N_sceneManager->currentScene->lights.begin(), engine->N_sceneManager->currentScene->lights.end(), this);
    if (!engine->N_sceneManager->currentScene->lights.empty())
    {
        if (it == engine->N_sceneManager->currentScene->lights.end())
        {
            engine->N_sceneManager->currentScene->lights.pop_back();
            return;
        }
        engine->N_sceneManager->currentScene->lights.erase(it);
    }
}

