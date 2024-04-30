#include "Light.h"
#include "EngineCore.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "Material.h"
#include "N_SceneManager.h"
#include "Shader.h"

Light::Light(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Light), lightType(LightType::Point),
color(1.0), specular(0.5), linear(0.3f), quadratic(0.2f)
{
    if (lightType == LightType::Point)
    {
        engine->N_sceneManager->currentScene->pointLights.push_back(this);
    }
}

Light::Light(std::shared_ptr<GameObject> containerGO, Light* ref) : Component(containerGO, ComponentType::Light)
{
    
}

Light::~Light() 
{
    //Destory in scene list
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
    //lightJSON["ModeType"] = modeType;
    /*lightJSON["Shadow"] = 
    {
        {"ShadowType", shadows.shadowType},
        {"Strength", shadows.strength},
        {"Bias", shadows.bias},
        {"NormalBias", shadows.normalBias},
        {"NearPlane", shadows.nearPlane},
        {"BakedShadowRadius", shadows.bakedShadowRadius},
    };*/
    lightJSON["UID"] = UID;
    //lightJSON["Range"] = range;
    //lightJSON["SpotAngle"] = spotAngle;
    lightJSON["Color"] = { color.r, color.g, color.b};
    lightJSON["Specular"] = specular;

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
    
   /* if (meshJSON.contains("ModeType"))
    {
        modeType = meshJSON["ModeType"];
    }
    
    if (meshJSON.contains("Shadow"))
    {
        if (meshJSON.contains("ShadowType"))
        {
            shadows.shadowType = meshJSON["ShadowType"];
        }
        if (meshJSON.contains("Strength"))
        {
            shadows.strength = meshJSON["Strength"];
        }
        if (meshJSON.contains("Bias"))
        {
            shadows.bias = meshJSON["Bias"];
        }
        if (meshJSON.contains("NormalBias"))
        {
            shadows.normalBias = meshJSON["NormalBias"];
        }
        if (meshJSON.contains("NearPlane"))
        {
            shadows.nearPlane = meshJSON["NearPlane"];
        }
        if (meshJSON.contains("BakedShadowRadius"))
        {
            shadows.bakedShadowRadius = meshJSON["BakedShadowRadius"];
        }
    }*/
    if (meshJSON.contains("UID"))
    {
        UID = meshJSON["UID"];
    }
    /*if (meshJSON.contains("Range"))
    {
        range = meshJSON["Range"];
    }*/
    /*if (meshJSON.contains("SpotAngle"))
    {
        spotAngle = meshJSON["SpotAngle"];
    }*/
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
}
