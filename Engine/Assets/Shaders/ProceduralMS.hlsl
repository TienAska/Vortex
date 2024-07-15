#include "Procedural.h"
#include "ShaderUniforms.h"

ConstantBuffer<GlobalParameters> global : register(b0);

[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void main(in payload Payload payload, out indices uint3 indices[1], out vertices OutputToPixel vertices[3])
{
    SetMeshOutputCounts(3, 1);

    indices[0] = uint3(0, 1, 2);
    
    float4 positions[3];
    float2 uv[3];
    FullscreenTriangle(positions, uv);

    [unroll] for(int i = 0; i < 3; i++)
    {
        vertices[i].position = positions[i];
        vertices[i].uv0 = uv[i];
    }
}