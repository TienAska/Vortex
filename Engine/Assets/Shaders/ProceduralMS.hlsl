#include "Procedural.h"
#include "ShaderUniforms.h"

ConstantBuffer<GlobalParameters> global : register(b0);

[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void main(in payload Payload payload, out indices uint3 indices[1], out vertices OutputToPixel vertices[3])
{
    SetMeshOutputCounts(3, 1);

    indices[0] = uint3(0, 1, 2);

    //vertices[0].position = mul(float4(0, 0.25, 0, 1), global.model);
    vertices[0].position = float4(-1.0, 1.0, 0, 1);
    vertices[0].uv0 = float2(0, 0);

    //vertices[1].position = mul(float4(0.25, -0.25, 0, 1), global.model);
    vertices[1].position = float4(3.0, 1.0, 0, 1);
    vertices[1].uv0 = float2(2, 0);

    //vertices[2].position = mul(float4(-0.25, -0.25, 0, 1), global.model);
    vertices[2].position = float4(-1.0, -3.0, 0, 1);
    vertices[2].uv0 = float2(0, 2);
}