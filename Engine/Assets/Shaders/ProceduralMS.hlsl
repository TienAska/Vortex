#include "Procedural.hlsli"
#include "ShaderUniforms.h"

ConstantBuffer<GlobalParameters> global : register(b0);

[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void main(in payload Payload payload, out indices uint3 indices[2], out vertices VertexAttributes vertices[4])
{
    SetMeshOutputCounts(4, 2);

    indices[0] = uint3(0, 1, 2);
    indices[1] = uint3(0, 2, 3);
    
    float4 positions[4];
    float2 uv[4];
    Quad(positions, uv);

    [unroll] for(int i = 0; i < 4; i++)
    {
        vertices[i].position = mul(mul(positions[i], global.view), global.projection);
        vertices[i].uv0 = uv[i];
    }
}