#include "Procedural.h"

Texture2D<float> t_noise : register(t0);

SamplerState s_point : register(s0);
SamplerState s_linear : register(s1);

float2 truchetPattern(float2 uv, float value){
    value = frac(((value-0.5)*2.0));
    if (value > 0.75) {
        uv = float2(1.0, 1.0) - uv;
    } else if (value > 0.5) {
        uv = float2(1.0-uv.x,uv.y);
    } else if (value > 0.25) {
        uv = 1.0-float2(1.0-uv.x,uv.y);
    }
    return uv;
}

float4 main(OutputToPixel input) : SV_TARGET
{
    // Maze
//     float value = t_noise.Sample(s_point, input.uv0);
//     float2 tile = truchetPattern(frac(32 * (input.uv0)), value);
//     float color = smoothstep(tile.x-0.3,tile.x,tile.y)-
//             smoothstep(tile.x,tile.x+0.3,tile.y);

    int3 index = int3(floor(32 * input.uv0), 0);
    
    float valueA = t_noise.Load(index);
    float valueB = t_noise.Load(index + int3(1, 0, 0));
    float valueC = t_noise.Load(index + int3(0, 1, 0));
    float valueD = t_noise.Load(index + int3(1, 1, 0));
    
    float2 smoothUV = (smoothstep(0.0, 1.0, frac(32 * (input.uv0))) + floor(32 * input.uv0)) / 32;
    smoothUV = smoothstep(0.0, 1.0, frac(32 * input.uv0));
    
    float value = lerp(lerp(valueA, valueB, smoothUV.x), lerp(valueC, valueD, smoothUV.x), smoothUV.y);

    return value;
}