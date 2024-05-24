#ifndef __LIGHT_H__
#define __LIGHT_H__
#pragma once

#include "Component.h"
#include "Resources.h"
#include "Uniform.h"
#include "Log.h"

class Material;
class Shader;
class FrameBuffer;

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
    Directional = 0,
    Point,
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
    Light(std::shared_ptr<GameObject> containerGO, LightType type = LightType::Point);
    Light(std::shared_ptr<GameObject> containerGO, Light* ref);
    virtual ~Light();

    void DrawComponent(Camera* camera);

    json SaveComponent();
    void LoadComponent(const json& meshJSON);

public:
    LightType lightType;
    //ModeType modeType;  //PBR
    //Shadow shadows;     //PBR

    //float range;
    //float spotAngle; //PBR
    vec3f color;
    float intensity;
    float specular;

    float radius;
    float linear;
    float quadratic;

    //SpotLight Vars
    float innerCutOff;
    float outerCutOff;

    //Shadows Vars
    std::shared_ptr<FrameBuffer> depthBuffer;
    bool castShadows;

    std::string lightPresetPath;
};
#endif //__LIGHT_H__