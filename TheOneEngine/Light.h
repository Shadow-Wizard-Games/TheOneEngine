#ifndef __LIGHT_H__
#define __LIGHT_H__
#pragma once

#include "Component.h"
#include "Resources.h"
#include "Uniform.h"
#include "Log.h"

class Material;
class Shader;

//  For PBR Lighting
struct Shadow
{
    enum ShadowType
    {
        NoShadow = 0,
        Hard,
        Soft,
    };
    ShadowType shadowType;
    float strength;
    float bias;
    float normalBias;
    float nearPlane;
    float bakedShadowRadius;
};

enum LightType
{
    Point = 0,
    Directional,
    Spot,
    Area, //Only in Baked mode? PBR
};

//  For PBR Lighting
enum ModeType
{
    Realtime = 0,
    Mixed,  //PBR?
    Baked,  //PBR?
};

class Light : public Component
{
public:
    Light(std::shared_ptr<GameObject> containerGO);
    Light(std::shared_ptr<GameObject> containerGO, Light* ref);
    virtual ~Light();

    void DrawComponent(Camera* camera);

    json SaveComponent();
    void LoadComponent(const json& meshJSON);

    void SetLightType(LightType type);

public:
    LightType lightType;
    //ModeType modeType;  //PBR
    //Shadow shadows;     //PBR

    //float range;
    //float spotAngle; //PBR
    vec3f color;
    float specular;

    float flux;
    float innerCutOff;
    float outerCutOff;

    std::string lightPresetPath;

    bool recalculate;
};
#endif //__LIGHT_H__