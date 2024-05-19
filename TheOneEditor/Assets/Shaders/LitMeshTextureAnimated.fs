#version 450 core

const int MAX_POINT_LIGHTS = 32;
const int MAX_SPOT_LIGHTS = 12;

struct Material {
    sampler2D diffuse;
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
in vec2 v_Vertex_UV;

//Lighting Uniforms
uniform DirLight u_DirLight;
uniform int u_PointLightsNum;
uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform int u_SpotLightsNum;
uniform SpotLight u_SpotLights[MAX_SPOT_LIGHTS];
uniform vec3 u_ViewPos;
uniform Material u_Material;

//Depth Uniforms (Change to uniforms)
float near = 0.1f;
float far = 100.0f;
float steepness = 0.5f; //Controls how fast depth value changes from 0 to 1
float offset = 5.0f;
bool fogActivated = false;

const float PI = 3.14159265359;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec2 flipUV = vec2(v_Vertex_UV.s, 1. - v_Vertex_UV.t);

    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(u_Material.diffuse, flipUV));
    vec3 diffuse = light.diffuse * diff * vec3(texture(u_Material.diffuse, flipUV));
    vec3 specular = light.specular * spec * u_Material.specular;
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec2 flipUV = vec2(v_Vertex_UV.s, 1. - v_Vertex_UV.t);

    vec3 lightDir = normalize(light.position - fragPos);
    float r = length(lightDir);
    // diffuse shading
    float diff = max(dot(lightDir, normal), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(viewDir + lightDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
    
    // combine results
    vec3 ambient = light.ambient * texture(u_Material.diffuse, flipUV).rgb;
    vec3 diffuse = light.diffuse * diff * texture(u_Material.diffuse, flipUV).rgb;
    vec3 specular = light.specular * spec * u_Material.specular;
    //if (diff == 0.0f) specular = vec3(0.0f);

    vec3 radiance = vec3(0.0);
    float irradiance = max(dot(lightDir, normal), 0.0) * light.flux / (4.0 * PI * r * r);
    radiance += (ambient + diffuse + specular) * irradiance;

    return radiance;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec2 flipUV = vec2(v_Vertex_UV.s, 1. - v_Vertex_UV.t);

    vec3 lightDir = normalize(light.position - fragPos);
    float r = length(lightDir);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), u_Material.shininess);
    
    // combine results
    vec3 ambient = light.ambient * texture(u_Material.diffuse, flipUV).rgb;
    vec3 diffuse = light.diffuse * diff * texture(u_Material.diffuse, flipUV).rgb;
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

//Function for fixing depth calculus
float linearizeDepth(float depth){
    return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}


//Function for creating fog effect
float logisticDepth(float depth, float steepness, float offset){
    float zVal = linearizeDepth(depth);
    return (1 / (1 + exp(-steepness * (zVal - offset))));
}

void main() {
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(u_ViewPos - fragPos);

    // phase 1: Directional lighting
    //vec3 result = CalcDirLight(u_DirLight, norm, viewDir);
    // phase 2: Point lights
    vec3 result = {0, 0, 0};
    for(int i = 0; i < u_PointLightsNum; i++) result += CalcPointLight(u_PointLights[i], norm, fragPos, viewDir);

    for(int i = 0; i < u_SpotLightsNum; i++) result += CalcSpotLight(u_SpotLights[i], norm, fragPos, viewDir);

    FragColor = vec4(result, 1.0);
}