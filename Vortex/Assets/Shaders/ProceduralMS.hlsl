#include "Procedural.hlsli"

[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void main(
    uint gtid : SV_GroupThreadID,
    uint gid : SV_GroupID,
    out vertices MeshToPixel vertices[3],
    out indices uint3 indices[1])
{
    SetMeshOutputCounts(3, 1);

    vertices[0].PositionHS = float4(0, 0.25, 0, 1);
    vertices[0].Color = float4(1, 0, 0, 1);

    vertices[1].PositionHS = float4(0.25, -0.25, 0, 1);
    vertices[1].Color = float4(1, 0, 0, 1);

    vertices[2].PositionHS = float4(-0.25, -0.25, 0, 1);
    vertices[2].Color = float4(1, 0, 0, 1);

    indices[0] = uint3(0, 1, 2);
}