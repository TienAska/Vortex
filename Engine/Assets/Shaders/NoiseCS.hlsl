#include "Procedural.hlsli"

#define GROUP_SIZE_X 32
#define GROUP_SIZE_Y 32
#define GROUP_SIZE_Z 1

RWTexture2D<float> noise :register(u0);

[numthreads(GROUP_SIZE_X, GROUP_SIZE_Y, GROUP_SIZE_Z)]
void main(uint3 gid :SV_GroupID, uint3 gtid :SV_GroupThreadID, uint3 dtid :SV_DispatchThreadID)
{
    uint3 cellSize = uint3(GROUP_SIZE_X, GROUP_SIZE_Y, GROUP_SIZE_Z);
    float2 inCellUV = (float2)gtid.xy / (cellSize.xy - 1);
    
    noise[dtid.xy] = abs(perlinNoise(gid, inCellUV) - valueNoise(gid, inCellUV));
    //noise[dtid.xy] = valueNoise(gid, inCellUV);
    noise[dtid.xy] = perlinNoise(gid, inCellUV);
}