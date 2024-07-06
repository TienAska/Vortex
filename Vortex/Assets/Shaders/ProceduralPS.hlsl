#include "Procedural.hlsli"

float4 main(MeshToPixel input) : SV_TARGET
{
    return input.Color;
}