#include "Skybox.hlsli"

float4 main(VertexAttributes vertexInput) : SV_TARGET
{
    return float4(vertexInput.uvw, 1.0);
}