#include "Skybox.hlsli"

[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void main(uint3 gid : SV_GroupID, out indices uint3 indices[12], out vertices VertexAttributes vertices[8])
{
    SetMeshOutputCounts(8, 12);
    
    float4 positions[8];
    Cube(positions, indices);

    [unroll] for(int i = 0; i < 8; i++)
    {
        //vertices[i].positionHS = mul(mul(mul(float4(position, 1.0), global.model), global.view), global.projection);
        vertices[i].positionHS = positions[i];
        vertices[i].uvw = positions[i].xyz;
    }
}