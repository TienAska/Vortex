#include "Procedural.hlsli"

groupshared Payload s_payload;

[numthreads(1, 1, 1)]
void main(uint3 gid : SV_GroupID)
{
    DispatchMesh(10, 10, 1, s_payload);
}