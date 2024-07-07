#include "Procedural.h"

#include "ShaderUniforms.h"

ConstantBuffer<GlobalParameters> global : register(b0);

[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void main(in payload Payload payload, out indices uint3 indices[1], out vertices MeshToPixel vertices[3])
{
    SetMeshOutputCounts(3, 1);

    indices[0] = uint3(0, 1, 2);

    vertices[0].PositionHS = float4(0, 0.25, 0, 1);
    vertices[0].Color = float4(1, 0, 0, 1);

    vertices[1].PositionHS = float4(0.25, -0.25, 0, 1);
    vertices[1].Color = float4(0, 1, 0, 1);

    vertices[2].PositionHS = float4(-0.25, -0.25, 0, 1);
    vertices[2].Color = float4(0, 0, 1, 1);
}