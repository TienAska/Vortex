#include "Procedural.h"

SamplerState MySamp : register(s0);
Texture2D <float4> MyTex : register(t0);

float4 main(MeshToPixel input) : SV_TARGET
{
    float4 texColor = MyTex.Sample(MySamp, input.PositionHS.xy);
    return input.Color * texColor;
}