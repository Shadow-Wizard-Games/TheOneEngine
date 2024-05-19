#version 450 core

const int MAX_POINT_LIGHTS = 32;
const int MAX_SPOT_LIGHTS = 12;

struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float flux;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;  
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float flux;
};

// Outputs colors in RGBA
out vec4 FragColor;

in vec3 normal;
in vec3 fragPos;

uniform DirLight u_DirLight;
uniform int u_PointLightsNum;
uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform int u_SpotLightsNum;
uniform SpotLight u_SpotLights[MAX_SPOT_LIGHTS];
uniform vec3 u_ViewPos;
uniform Material u_Material;

const float PI = 3.14159265359;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
    // combine results
    vec3 ambient = light.ambient * u_Material.diffuse;
    vec3 diffuse = light.diffuse * diff * u_Material.diffuse;
    vec3 specular = light.specular * spec * u_Material.specular;
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float r = length(lightDir);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
    
    vec3 ambient = light.ambient * u_Material.diffuse;
    vec3 diffuse = light.diffuse * diff * u_Material.diffuse;
    vec3 specular = light.specular * spec * u_Material.specular;

    vec3 radiance = vec3(0.0);
    float irradiance = max(dot(lightDir, normal), 0.0) * light.flux / (4.0 * PI * r * r);
    radiance += (ambient + diffuse + specular) * irradiance;

    return radiance;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float r = length(lightDir);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
    
    // combine results
    vec3 ambient = light.ambient * u_Material.diffuse;
    vec3 diffuse = light.diffuse * diff * u_Material.diffuse;
    vec3 specular = light.specular * spec * u_Material.specular;

    // spotlight (soft edges)
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;

    vec3 radiance = vec3(0.0);
    float irradiance = max(dot(lightDir, normal), 0.0) * light.flux / (4.0 * PI * r * r);
    radiance += (ambient + diffuse + specular) * irradiance;

    return radiance;
}

void main() {
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(u_ViewPos - fragPos);

    // phase 1: Directional lighting
    //vec3 result = CalcDirLight(u_DirLight, norm, viewDir);
    // phase 2: Point lights
    vec3 result = {0,0,0};
    for(int i = 0; i < u_PointLightsNum; i++) result += CalcPointLight(u_PointLights[i], norm, fragPos, viewDir);
    
    for(int i = 0; i < u_SpotLightsNum; i++) result += CalcSpotLight(u_SpotLights[i], norm, fragPos, viewDir);

    FragColor = vec4(result, 1.0);
}