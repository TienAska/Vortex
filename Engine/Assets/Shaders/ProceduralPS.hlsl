#include "Procedural.h"

Texture2D<float> t_noise : register(t0);

SamplerState s_point : register(s0);

float4 main(OutputToPixel input) : SV_TARGET
{
    float texColor = t_noise.Sample(s_point, input.uv0);
    return texColor;
}