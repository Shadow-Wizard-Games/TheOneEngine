//------------------------
// Smol CRT Shader ~hekbas
//------------------------

#version 450 core
precision highp float;

#define RGB(r, g, b) vec3(float(r)/255.0, float(g)/255.0, float(b)/255.0)

uniform sampler2D albedo;
uniform vec2 resolution;
uniform float time;

in vec2 fragCoord;
out vec4 fragColor;


const vec2 kWarp = vec2(1.0 / 12.0, 1.0 / 8.0);

vec2 WarpUV(vec2 uv)
{
	uv = uv * 2.0 - 1.0;
	uv *= vec2(
		1.0 + (uv.y * uv.y) * kWarp.x,
		1.0 + (uv.x * uv.x) * kWarp.y
	);
	return uv * 0.5 + 0.5;
}

vec2 CurveUV(vec2 uv)
{
    uv = uv * 2.0 - 1.0;
    vec2 offset = abs(uv.yx) / vec2(3.0, 2.0);
    uv = uv + uv * offset * offset;
    uv = uv * 0.5 + 0.5;
    return uv;
    //return clamp(uv, 0.0, 1.0);
}

vec2 SmoothWarpUV(vec2 uv)
{
    uv = uv * 2.0 - 1.0;
    uv *= vec2(
        1.0 + (uv.y * uv.y) * kWarp.x,
        1.0 + (uv.x * uv.x) * kWarp.y
    );
    uv = uv * 0.5 + 0.5;
    return clamp(uv, 0.0, 1.0);
}

void DrawVignette(inout vec3 color, vec2 uv)
{
    float vignette = uv.x * uv.y * (1.0 - uv.x) * (1.0 - uv.y);
    vignette = clamp(pow(16.0 * vignette, 0.3), 0.0, 1.0);
    color *= vignette;
}

void DrawScanLine(inout vec3 color, vec2 uv)
{
    float scanline = clamp(0.95 + 0.05 * cos(3.14 * (uv.y + 0.008 * time) * 240.0 * 1.0), 0.0, 1.0);
    float grille = 0.85 + 0.15 * clamp(1.5 * cos(3.14 * uv.x * 640.0 * 1.0), 0.0, 1.0);
    color *= scanline * grille * 1.2;
}

void main()
{
    // Curve UV
    vec2 uv = fragCoord.xy;
    vec2 crtUV = SmoothWarpUV(uv);

    // Color
    vec3 color = texture(albedo, crtUV).rgb;
    //vec3 color = vec3(crtUV, 0.0).rgb;

    // Set color
    if(crtUV.x < 0.0 || crtUV.x > 1.0 || crtUV.y < 0.0 || crtUV.y > 1.0)
    {
        color = vec3(0.0, 0.0, 0.0);
    }

    // Vignette
    DrawVignette(color, crtUV);

    // ScanLine
    DrawScanLine(color, uv);

    // Fragcolor OUT
    fragColor.xyz = color;
    fragColor.w = 1.0;
}