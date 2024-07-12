RWTexture2D<float> noise : register(u0);

[numthreads(32, 32, 1)]
void main(uint3 gtid : SV_GroupThreadID, uint3 dtid : SV_DispatchThreadID)
{
    noise[dtid.xy] = dot(gtid.xy, 0.5f) / 32;
}