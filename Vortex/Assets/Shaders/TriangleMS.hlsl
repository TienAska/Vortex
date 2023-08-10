#define ROOT_SIG "CBV(b0), \
                  RootConstants(b1, num32bitconstants=2), \
                  SRV(t0), \
                  SRV(t1), \
                  SRV(t2), \
                  SRV(t3)"

struct MeshToPixel
{
	float4 PositionHS : SV_Position;
	float4 Color : COLOR1;
};

[rootsignature(ROOT_SIG)]
[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void MeshShaderMain(
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

float4 PixelShaderMain(MeshToPixel input) : SV_TARGET
{
	return input.Color;
}