#include "Skybox.hlsli"
#include "ShaderUniforms.h"

ConstantBuffer<GlobalParameters> global : register(b0);

[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void main(uint3 gid : SV_GroupID, out indices uint3 indices[12], out vertices VertexAttributes vertices[8])
{
    SetMeshOutputCounts(8, 12);
    
    float4 positions[8];
    Cube(positions, indices);

    [unroll] for(int i = 0; i < 8; i++)
    {
        vertices[i].positionHS = mul(mul(positions[i], global.view), global.projection);
        vertices[i].uvw = positions[i].xyz;
    }
}