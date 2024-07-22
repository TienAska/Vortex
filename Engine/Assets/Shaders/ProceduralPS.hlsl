#include "Procedural.hlsli"
#include "ShaderUniforms.h"

ConstantBuffer<GlobalParameters> global : register(b0);

Texture2D<float> t_noise : register(t0);

SamplerState s_point : register(s0);
SamplerState s_linear : register(s1);

float4 main(VertexAttributes input) : SV_TARGET
{
    // Maze
//     float value = t_noise.Sample(s_point, input.uv0);
//     float2 tile = truchetPattern(frac(32 * (input.uv0)), value);
//     float color = smoothstep(tile.x-0.3,tile.x,tile.y)-
//             smoothstep(tile.x,tile.x+0.3,tile.y);

    //int3 index = int3(floor(4 * input.uv0), 0);
    //
    //float valueA = t_noise.Load(index);
    //float valueB = t_noise.Load(index + int3(1, 0, 0));
    //float valueC = t_noise.Load(index + int3(0, 1, 0));
    //float valueD = t_noise.Load(index + int3(1, 1, 0));
    //
    //float2 smoothUV = smoothstep(0.0, 1.0, frac(32 * input.uv0));
    //float value = lerp(lerp(valueA, valueB, smoothUV.x), lerp(valueC, valueD, smoothUV.x), smoothUV.y);

    //value = valueA;
    //smoothUV = (smoothstep(0.0, 1.0, frac(32 * (input.uv0))) + floor(32 * input.uv0)) / 32;
    //value = t_noise.Sample(s_linear, smoothUV);
    float2 flipedUV = float2(input.uv0.x - global.time, 1 - input.uv0.y);
    float value = t_noise.Sample(s_linear, flipedUV);
        

    return float4(input.uv0, 0.0, 1.0);
}