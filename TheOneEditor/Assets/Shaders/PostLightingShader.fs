#version 450 core

const int MAX_POINT_LIGHTS = 32;
const int MAX_SPOT_LIGHTS = 12;

// Outputs colors in RGBA
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct DirLight {
    vec3 Position;
    vec3 Color;
    vec3 Direction;
};

struct PointLight {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
    float Radius;
}; 

struct SpotLight {
    vec3 Position;
    vec3 Color;
    vec3 Direction;

    float Linear;
    float Quadratic;
    float Radius;

    float CutOff;
    float OuterCutOff;
};

uniform DirLight u_DirLight;
uniform int u_PointLightsNum;
uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform int u_SpotLightsNum;
uniform SpotLight u_SpotLights[MAX_SPOT_LIGHTS];

uniform vec3 u_ViewPos;

const float PI = 3.14159265359;

// vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
//     vec3 lightDir = normalize(-light.direction);
//     // diffuse shading
//     float diff = max(dot(normal, lightDir), 0.0);
//     // specular shading
//     vec3 reflectDir = reflect(-lightDir, normal);
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
//     // combine results
//     vec3 ambient = light.ambient * vec3(texture(u_Material.diffuse, v_Vertex_UV));
//     vec3 diffuse = light.diffuse * diff * vec3(texture(u_Material.diffuse, v_Vertex_UV));
//     vec3 specular = light.specular * spec * u_Material.specular;
//     return (ambient + diffuse + specular);
// }

vec3 CalcPointLight(PointLight light, vec3 Diffuse, float Specular, vec3 Normal, vec3 FragPos, vec3 ViewDir) {
    float distance = length(light.Position - FragPos);
    if(distance < light.Radius)
    {
        // diffuse
        vec3 lightDir = normalize(light.Position - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.Color;
        // specular
        vec3 halfwayDir = normalize(lightDir + ViewDir);  
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
        vec3 specular = light.Color * spec * Specular;
        // attenuation
        float attenuation = 1.0 / (1.0 + light.Linear * distance + light.Quadratic * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;
        return (diffuse + specular);
    }
}

vec3 CalcSpotLight(SpotLight light, vec3 Diffuse, float Specular, vec3 Normal, vec3 FragPos, vec3 ViewDir) {
    float distance = length(light.Position - FragPos);
    if(distance < light.Radius)
    {
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
        diffuse  *= intensity;
        specular *= intensity;

        // attenuation
        float attenuation = 1.0 / (1.0 + light.Linear * distance + light.Quadratic * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;
        return (diffuse + specular);
    }
}

void main()
{
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    vec3 ViewDir  = normalize(u_ViewPos - FragPos);

    // phase 1: Directional lighting
    //vec3 result = CalcDirLight(u_DirLight, norm, viewDir);

    // phase 2: Point lights
    vec3 result = Diffuse * 0.1;
    for(int i = 0; i < u_PointLightsNum; i++)
    result += CalcPointLight(u_PointLights[i], Diffuse, Specular, Normal, FragPos, ViewDir);

    // phase 3: Spot lights
    // for(int i = 0; i < u_SpotLightsNum; i++)
    // result += CalcSpotLight(u_SpotLights[i], Diffuse, Specular, Normal, FragPos, ViewDir);

    FragColor = vec4(result, 1.0);
}