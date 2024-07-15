#include "Procedural.h"

#define GROUP_SIZE_X 32
#define GROUP_SIZE_Y 32
#define GROUP_SIZE_Z 1

RWTexture2D<float> noise : register(u0);

[numthreads(GROUP_SIZE_X, GROUP_SIZE_Y, GROUP_SIZE_Z)]
void main(uint3 gtid : SV_GroupThreadID, uint3 dtid : SV_DispatchThreadID)
{
    uint3 gp_max = uint3(GROUP_SIZE_X, GROUP_SIZE_Y, GROUP_SIZE_Z) - 1;
    float2 uv = (float2)gtid.xy / gp_max.xy;

    noise[dtid.xy] = random(uv);
}