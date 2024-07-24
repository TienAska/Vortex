#include "Procedural.hlsli"
#include "ShaderUniforms.h"

ConstantBuffer<GlobalParameters> global : register(b0);
ConstantBuffer<MaterialParameters> material : register(b1);

[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void main(uint3 gid : SV_GroupID, in payload Payload payload, out indices uint3 indices[2], out vertices VertexAttributes vertices[4], out primitives PrimitiveAttributes primitives[2])
{
    SetMeshOutputCounts(4, 2);

    indices[0] = uint3(0, 1, 2);
    indices[1] = uint3(0, 2, 3);
    
    float4 positions[4];
    float2 uv[4];
    Quad(positions, uv);

    [unroll] for(int i = 0; i < 4; i++)
    {
        float3 position = (positions[i].xyz * material.scale) + material.offset * gid;
        vertices[i].positionHS = mul(mul(mul(float4(position, 1.0), global.model), global.view), global.projection);
        vertices[i].uv0 = uv[i];
    }
    primitives[0].color = float4(random(gid.xy).xxx, 1.0);
    primitives[1].color = float4(random(gid.xy + 1).xxx, 1.0);
}