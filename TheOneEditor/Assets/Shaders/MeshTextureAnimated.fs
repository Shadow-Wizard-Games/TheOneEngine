#version 450 core
// const int MAX_POINT_LIGHTS = 32;
// const int MAX_SPOT_LIGHTS = 32;

// struct BaseLight
// {
//     vec3 Color;
//     float AmbientIntensity;
//     float DiffuseIntensity;
// };

// struct DirectionalLight
// {
//     BaseLight Base;
//     vec3 Direction;
// };

// struct Attenuation
// {
//     float Constant;
//     float Linear;
//     float Exp;
// };

// struct PointLight
// {
//     BaseLight Base;
//     vec3 LocalPos;
//     Attenuation Atten;
// };

// struct SpotLight
// {
//     PointLight Base;
//     vec3 Direction;
//     float Cutoff;
// };

// in vec3 v_world_normal;
// in vec4 v_vertex_color;
in vec2 v_Vertex_UV;

// in vec3 Normal;
// in vec3 LocalPos;
// in vec4 LightSpacePos;

// // Outline percentage
// uniform vec2 u_OutlineSmoothRange = vec2(0.4,0.4);
// // Outline color
// uniform vec4 u_OutlineColor = vec4(0,0,0,1);

// uniform vec4 u_MatAmbientColor;
// uniform vec4 u_MatDiffuseColor;
// uniform vec4 u_MatSpecularColor;
// uniform int u_ToonLevels = 4;
// uniform float u_RimLightPower = 4.0;

// float ToonScaleFactor = 1.0f / u_ToonLevels;

// uniform DirectionalLight u_DirectionalLight;
// uniform int u_NumPointLights;
// uniform PointLight[MAX_POINT_LIGHTS] u_PointLights;
// uniform int u_NumSpotLights;
// uniform SpotLight[MAX_SPOT_LIGHTS] u_SpotLights;
// uniform vec4 u_Color = vec4(0.0,0.0,0.0,0.0);
// uniform int u_Hit = 0;
// uniform sampler2D u_ShadowMap;
uniform sampler2D u_Tex;
// uniform float u_SpecularValue;
// uniform vec3 u_CameraPosition;
// uniform vec3 u_LightPos;

out vec4 FragColor;

// float CalcShadowFactor(vec3 position,vec3 direction)
// {
//      vec3 projCoords = LightSpacePos.xyz / LightSpacePos.w;
//     // transform to [0,1] range
//     projCoords = projCoords * 0.5 + 0.5;
//     // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
//     float closestDepth = texture(u_ShadowMap, projCoords.xy).r; 
//     // get depth of current fragment from light's perspective
//     float currentDepth = projCoords.z;
//     // calculate bias (based on depth map resolution and slope)
//     vec3 normal = normalize(Normal);
//     vec3 lightDir = normalize(position - LocalPos);
//     float bias = 0.005;
//     // check whether current frag pos is in shadow
//     // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
//     // PCF
//     float shadow = 0.0;
//     vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
//     for(int x = -1; x <= 1; ++x)
//     {
//         for(int y = -1; y <= 1; ++y)
//         {
//             float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
//             shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
//         }    
//     }
//     shadow /= 9.0;

//    if(projCoords.z > 1.0)
//         shadow = 0.0;
    
//     return shadow;
// }

// float CalcRimLightFactor(vec3 pixelToCamera, vec3 normal)
// {
//     float rimFactor = dot(pixelToCamera, normal);
//     rimFactor = 1.0 - rimFactor;
//     rimFactor = max(0.0, rimFactor);
//     rimFactor = pow(rimFactor, u_RimLightPower);
//     return rimFactor;
// }

// vec4 CalcLightInternal(BaseLight light, vec3 direction, vec3 normal, float shadowFactor)
// {
//     vec4 ambientColor = vec4(light.Color, 1.0f) * light.AmbientIntensity * u_MatAmbientColor;

//     float diffuseFactor = dot(normal, -direction);

//     vec4 diffuseColor = vec4(0, 0, 0, 0);
//     vec4 specularColor = vec4(0, 0, 0, 0);
//     vec4 rimColor = vec4(0, 0, 0, 0);

//     if(diffuseFactor > 0) 
//     {
//         diffuseFactor = floor(diffuseFactor * u_ToonLevels) * ToonScaleFactor;

//         diffuseColor = vec4(light.Color, 1.0f) * light.DiffuseIntensity * u_MatDiffuseColor * diffuseFactor;

//         vec3 pixelToCamera = normalize(u_CameraPosition - LocalPos);

//         float rimFactor = CalcRimLightFactor(pixelToCamera, normal);
//         rimColor = diffuseColor * rimFactor;
//     }

//     return (ambientColor + (1 - shadowFactor)  * (diffuseColor + specularColor + rimColor));
// }

// vec4 CalcDirectionalLight(vec3 normal)
// {
    
//     float shadowFactor = CalcShadowFactor(u_LightPos, u_DirectionalLight.Direction);

//     return CalcLightInternal(u_DirectionalLight.Base, u_DirectionalLight.Direction, normal, shadowFactor);
// }

// vec4 CalcPointLight(PointLight light, vec3 normal)
// {
//     float shadowFactor = 1.0;

//     vec3 lightDirection = LocalPos - light.LocalPos;
//     float distance = length(lightDirection);
//     lightDirection = normalize(lightDirection);

//     vec4 color = CalcLightInternal(light.Base, lightDirection, normal, shadowFactor);

//     float attenuation = light.Atten.Constant +
//                         light.Atten.Linear * distance *
//                         light.Atten.Exp * distance * distance;

//     return color / attenuation;
// }
// vec4 CalcSpotLight(SpotLight light, vec3 normal)
// {
//     vec3 lightToPixel = normalize(LocalPos - light.Base.LocalPos);
//     float spotFactor = dot(lightToPixel, light.Direction);

//     if(spotFactor > light.Cutoff)
//     {
//         vec4 color = CalcPointLight(light.Base, normal);
//         float spotLightIntensity = (1.0 - (1.0 - spotFactor) / (1.0 - light.Cutoff));
//         return color * spotLightIntensity;
//     }
// }

void main() {
  

	//vec3 normal = normalize(Normal);

	//vec4 totalLight = CalcDirectionalLight(normal);
	
	// for(int i = 0; i < u_NumPointLights; i++)
    // {
    //     totalLight += CalcPointLight(u_PointLights[i], normal);
    // }

	// vec3 dir = u_CameraPosition - LocalPos;
	
	// float dotEyeNormal = dot(normalize(dir),normalize(Normal));
    // dotEyeNormal = abs(dotEyeNormal);
	
	vec2 flipuv = vec2(v_Vertex_UV.s, 1. - v_Vertex_UV.t);
	FragColor = texture(u_Tex, flipuv);
	//flipuv);//ambient * v_vertex_color * 
	
	// vec4 colorLight  = color * totalLight;
	// if(u_Hit == 1)
    // {
    //     FragColor = u_Color;
    // }
    // else{
	//     FragColor = mix(u_OutlineColor, colorLight, smoothstep(u_OutlineSmoothRange.x,u_OutlineSmoothRange.y,dotEyeNormal));
    // }
}