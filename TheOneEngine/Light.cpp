#include "Light.h"
#include "EngineCore.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "Material.h"
#include "N_SceneManager.h"
#include "Shader.h"

Light::Light(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Light), lightType(LightType::Point),
color(1.0f), specular(0.5f), linear(0.7f), quadratic(1.8f), innerCutOff(0.91f), outerCutOff(0.82f), recalculate(true)
{
    const float maxBrightness = std::fmaxf(std::fmaxf(color.r, color.g), color.b);
    radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (1.0f - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);

    if (lightType == LightType::Point)
    {
        engine->N_sceneManager->currentScene->pointLights.push_back(this);
    }
}

Light::Light(std::shared_ptr<GameObject> containerGO, Light* ref) : Component(containerGO, ComponentType::Light), lightType(ref->lightType),
color(ref->color), specular(ref->specular), radius(ref->radius), linear(ref->linear), quadratic(ref->quadratic), 
innerCutOff(ref->innerCutOff), outerCutOff(ref->outerCutOff), recalculate(true)
{
    switch (lightType)
    {
    case Point:
        engine->N_sceneManager->currentScene->pointLights.push_back(this);
        break;
    case Directional:
        engine->N_sceneManager->currentScene->directionalLight = this;
        break;
    case Spot:
        engine->N_sceneManager->currentScene->spotLights.push_back(this);
        break;
    case Area:
        break;
    default:
        break;
    }
}

Light::~Light() 
{
    //Destory in scene vector
    switch (lightType)
    {
    case LightType::Point:
    {
        auto it = std::find(engine->N_sceneManager->currentScene->pointLights.begin(), engine->N_sceneManager->currentScene->pointLights.end(),
            this);
        if (it != engine->N_sceneManager->currentScene->pointLights.end()) {
            engine->N_sceneManager->currentScene->pointLights.erase(it);
        }
    }
    break;
    case LightType::Directional:
    {
        delete engine->N_sceneManager->currentScene->directionalLight;
        engine->N_sceneManager->currentScene->directionalLight = nullptr;
    }
    break;
    case LightType::Spot:
    {
        auto it = std::find(engine->N_sceneManager->currentScene->spotLights.begin(), engine->N_sceneManager->currentScene->spotLights.end(),
            this);
        if (it != engine->N_sceneManager->currentScene->spotLights.end()) {
            engine->N_sceneManager->currentScene->spotLights.erase(it);
        }
    }
    break;
    case LightType::Area:
        break;
    default:
        break;
    }
}

void Light::DrawComponent(Camera* camera)
{
    //Stop shaders calculus to not be each frame doing it
    std::vector<Light*> pointLights = engine->N_sceneManager->currentScene->pointLights;

    for (uint i = 0; i < pointLights.size(); i++)
    {
        if (pointLights[i]->recalculate) pointLights[i]->recalculate = false;
    }
    pointLights.clear();
    
    std::vector<Light*> spotLights = engine->N_sceneManager->currentScene->spotLights;

    for (uint i = 0; i < spotLights.size(); i++)
    {
        if (spotLights[i]->recalculate) spotLights[i]->recalculate = false;
    }
    spotLights.clear();

    if (engine->N_sceneManager->currentScene->directionalLight != nullptr && engine->N_sceneManager->currentScene->directionalLight->recalculate)
        engine->N_sceneManager->currentScene->directionalLight->recalculate = false;
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
    if (meshJSON.contains("InnerCutOff"))
    {
        innerCutOff = meshJSON["InnerCutOff"];
    }
    if (meshJSON.contains("OuterCutOff"))
    {
        outerCutOff = meshJSON["OuterCutOff"];
    }
}

void Light::SetLightType(LightType type)
{
    bool changedToDirectional = false;
    if (type != lightType)
    {
        switch (type)
        {
        case LightType::Point:
        {
            engine->N_sceneManager->currentScene->pointLights.push_back(this);
        }
        break;
        case LightType::Directional:
        {   
            if (engine->N_sceneManager->currentScene->directionalLight == nullptr)
            {
                engine->N_sceneManager->currentScene->directionalLight = this;
                changedToDirectional = true;
            }
            else
            {
                LOG(LogType::LOG_ERROR, "Already exists a Directional Light in scene");
            }
        }
        break;
        case LightType::Spot:
        {
            engine->N_sceneManager->currentScene->spotLights.push_back(this);
        }
        break;
        case LightType::Area:
            break;
        default:
            break;
        }

        //Destory in scene vector
        switch (lightType)
        {
        case LightType::Point:
        {
            if (type == LightType::Directional && !changedToDirectional) break;
            auto it = std::find(engine->N_sceneManager->currentScene->pointLights.begin(), engine->N_sceneManager->currentScene->pointLights.end(),
                this);
            if (it != engine->N_sceneManager->currentScene->pointLights.end()) {
                engine->N_sceneManager->currentScene->pointLights.erase(it);
            }
        }
            break;
        case LightType::Directional:
        {
            delete engine->N_sceneManager->currentScene->directionalLight;
            engine->N_sceneManager->currentScene->directionalLight = nullptr;
        }
            break;
        case LightType::Spot:
        {
            if (type == LightType::Directional && !changedToDirectional) break;
            auto it = std::find(engine->N_sceneManager->currentScene->spotLights.begin(), engine->N_sceneManager->currentScene->spotLights.end(),
                this);
            if (it != engine->N_sceneManager->currentScene->spotLights.end()) {
                engine->N_sceneManager->currentScene->spotLights.erase(it);
            }
        }
            break;
        case LightType::Area:
            break;
        default:
            break;
        }
        this->lightType = type;
        this->recalculate = true;
    }
}
