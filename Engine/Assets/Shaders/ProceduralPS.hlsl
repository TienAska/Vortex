#include "Procedural.h"

Texture2D<float4> t_noise : register(t0);

SamplerState s_point
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};

float4 main(OutputToPixel input) : SV_TARGET
{
    float4 texColor = t_noise.Sample(s_point, input.position.xy);
    return input.color;
}