//------------------------
// Smol CRT Shader ~hekbas
//------------------------

#version 450 core
precision highp float;

uniform sampler2D albedo;
uniform float kEasing;
uniform float time;
uniform vec2 warp;

in vec2 fragCoord;
layout(location = 1) out vec4 fragColor;

vec2 kWarp = vec2(1.0 / warp.x, 1.0 / warp.y);

vec2 WarpUV(vec2 uv)
{
	uv = uv * 2.0 - 1.0;
	uv *= vec2(
		1.0 + (uv.y * uv.y) * kWarp.x,
		1.0 + (uv.x * uv.x) * kWarp.y
	);
	return uv * 0.5 + 0.5;
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

void ScreenBlood(inout vec3 color, vec2 uv)
{
    float vignette = uv.x * uv.y * (1.0 - uv.x) * (1.0 - uv.y);
    vignette = clamp(pow(42.0 * vignette, 0.25), 0.0, 1.0); //val, min, max
    vec3 bloodColor = vec3(1.0, 0.0, 0.0);
    color =  mix(color, bloodColor, (1.0 - vignette) * kEasing); //start, end, interpolation
}

void main()
{
    vec2 uv = fragCoord.xy;
    vec2 crtUV = WarpUV(uv);

    vec3 color = texture(albedo, crtUV).rgb;

    ScreenBlood(color, crtUV);

    if(crtUV.x < 0.0 || crtUV.x > 1.0 || crtUV.y < 0.0 || crtUV.y > 1.0)
    {
        color = vec3(0.0, 0.0, 0.0);
    }

    DrawVignette(color, crtUV);

    DrawScanLine(color, crtUV);  

    fragColor.xyz = color;
    fragColor.w = 1.0;
}