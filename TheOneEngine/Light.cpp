#include "Light.h"
#include "EngineCore.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "Material.h"
#include "N_SceneManager.h"
#include "Shader.h"
#include "FrameBuffer.h"
#include "Renderer3D.h"


Light::Light(std::shared_ptr<GameObject> containerGO, LightType type)
    : Component(containerGO, ComponentType::Light),
    lightType(type),
    color(1.0f), intensity(1.0f), specular(0.5f),
    radius(60.0f), linear(0.7f), quadratic(1.8f),
    innerCutOff(0.91f), outerCutOff(0.82f)
{
    castShadows = lightType == LightType::Spot ? true : false;

    std::vector<Attachment> shadowBuffAttachments = {
        { Attachment::Type::DEPTH, "depth", "shadowBuffer", 0 }
    };
    shadowBuffer = std::make_shared<FrameBuffer>("shadowBuffer", 1280, 720, shadowBuffAttachments);

    containerGO->AddComponent<Camera>();
    containerGO->GetComponent<Camera>()->zNear = 10.0f;

    if (lightType == Directional)
        containerGO->GetComponent<Camera>()->cameraType = CameraType::ORTHOGRAPHIC;
    
    Renderer3D::AddLight(containerGO);
}

Light::Light(std::shared_ptr<GameObject> containerGO, Light* ref)
    : Component(containerGO, ComponentType::Light),
    lightType(ref->lightType), castShadows(ref->castShadows),
    color(ref->color), intensity(ref->intensity), specular(ref->specular),
    radius(ref->radius), linear(ref->linear), quadratic(ref->quadratic), 
    innerCutOff(ref->innerCutOff), outerCutOff(ref->outerCutOff)
{
    std::vector<Attachment> shadowBuffAttachments = {
        { Attachment::Type::DEPTH, "depth", "shadowBuffer", 0 }
    };
    shadowBuffer = std::make_shared<FrameBuffer>("shadowBuffer", 1280, 720, shadowBuffAttachments);

    containerGO->AddCopiedComponent<Camera>(ref->containerGO.lock()->GetComponent<Camera>());

    Renderer3D::AddLight(containerGO);
}

Light::~Light() {}

void Light::DrawComponent(Camera* camera) {}

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
    lightJSON["Intensity"] = intensity;
    lightJSON["Specular"] = specular;
    lightJSON["Radius"] = radius;
    lightJSON["Linear"] = linear;
    lightJSON["Quadratic"] = quadratic;
    lightJSON["InnerCutOff"] = innerCutOff;
    lightJSON["OuterCutOff"] = outerCutOff;
    lightJSON["ActiveShadows"] = castShadows;

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
    if (meshJSON.contains("Intensity"))
    {
        intensity = meshJSON["Intensity"];
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
    if (meshJSON.contains("ActiveShadows"))
    {
        castShadows = meshJSON["ActiveShadows"];
    }
}