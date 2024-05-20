#version 450 core

const int MAX_DIRECTIONAL_LIGHTS = 5;
const int MAX_POINT_LIGHTS = 32;
const int MAX_SPOT_LIGHTS = 32;

// Outputs colors in RGBA
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct DirLight {
    vec3 Position;
    vec3 Color;
    float Intensity;
    vec3 Direction;

    float NearPlane;
    float FarPlane;

    sampler2D Depth;
};

struct PointLight {
    vec3 Position;
    vec3 Color;
    float Intensity;
    
    float Linear;
    float Quadratic;
    float Radius;

    float NearPlane;
    float FarPlane;

    sampler2D Depth;
}; 

struct SpotLight {
    vec3 Position;
    vec3 Color;
    float Intensity;
    vec3 Direction;

    float Linear;
    float Quadratic;
    float Radius;

    float CutOff;
    float OuterCutOff;

    float NearPlane;
    float FarPlane;

    sampler2D Depth;
};

uniform int u_TotalLightsNum;
uniform DirLight u_DirLight[MAX_DIRECTIONAL_LIGHTS];
uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform SpotLight u_SpotLights[MAX_SPOT_LIGHTS];

uniform vec3 u_ViewPos;

const float PI = 3.14159265359;

vec3 CalcDirLight(DirLight light, vec3 Diffuse, float Specular, vec3 Normal, vec3 FragPos, vec3 ViewDir)
{
    // diffuse
    vec3 lightDir = normalize(-light.Direction);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.Color;
    // specular
    vec3 halfwayDir = normalize(lightDir + ViewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
    vec3 specular = light.Color * spec * Specular;

    float depthValue = texture(light.Depth, TexCoords).r;
    

    return ((diffuse + specular) * (light.Intensity * 200.0));
}

vec3 CalcPointLight(PointLight light, vec3 Diffuse, float Specular, vec3 Normal, vec3 FragPos, vec3 ViewDir)
{
    float distance = length(light.Position - FragPos);
    if(distance > light.Radius) return vec3(0.0);

    // diffuse
    vec3 lightDir = normalize(light.Position - FragPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.Color;
    // specular
    vec3 halfwayDir = normalize(lightDir + ViewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
    vec3 specular = light.Color * spec * Specular;
    // attenuation
    float attenuation = 1.0 / (1.0 + light.Linear * distance + light.Quadratic * distance * distance);

    return ((diffuse + specular) * attenuation * (light.Intensity * 200.0));
}

vec3 CalcSpotLight(SpotLight light, vec3 Diffuse, float Specular, vec3 Normal, vec3 FragPos, vec3 ViewDir)
{
    float distance = length(light.Position - FragPos);
    if(distance > light.Radius) return vec3(0.0);

    // diffuse
    vec3 lightDir = normalize(light.Position - FragPos);
    float r = length(lightDir);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.Color;
    
    // specular
    vec3 halfwayDir = normalize(lightDir + ViewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
    vec3 specular = light.Color * spec * Specular;

    // spotlight (soft edges)
    float theta = dot(lightDir, normalize(-light.Direction));
    float epsilon = (light.CutOff - light.OuterCutOff);
    float intensity = clamp((theta - light.OuterCutOff) / epsilon, 0.0, 1.0);

    // attenuation
    float attenuation = 1.0 / (1.0 + light.Linear * distance + light.Quadratic * distance * distance);

    float depthValue = texture(light.Depth, TexCoords).r;


    return ((diffuse + specular) * intensity * attenuation * (light.Intensity * 200.0));
}

void main()
{
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    vec3 ViewDir  = normalize(u_ViewPos - FragPos);

    vec3 result = Diffuse * 0.3;

    // phase 1: Directional lighting
    for(int i = 0; i < MAX_DIRECTIONAL_LIGHTS; i++)
    result += CalcDirLight(u_DirLight[i], Diffuse, Specular, Normal, FragPos, ViewDir);

    // phase 2: Point lights
    for(int i = 0; i < MAX_POINT_LIGHTS; i++)
    result += CalcPointLight(u_PointLights[i], Diffuse, Specular, Normal, FragPos, ViewDir);

    // phase 3: Spot lights
    for(int i = 0; i < MAX_SPOT_LIGHTS; i++)
    result += CalcSpotLight(u_SpotLights[i], Diffuse, Specular, Normal, FragPos, ViewDir);

    FragColor = vec4(result, 1.0);
}