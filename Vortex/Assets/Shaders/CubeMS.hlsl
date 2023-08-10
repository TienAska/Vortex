//#define ROOT_SIG "CBV(b0), \
//                  RootConstants(b1, num32bitconstants=2), \
//                  SRV(t0), \
//                  SRV(t1), \
//                  SRV(t2), \
//                  SRV(t3)"

//struct Constants
//{
//	float4x4 World;
//	float4x4 WorldView;
//	float4x4 WorldViewProj;
//	uint     DrawMeshlets;
//};

//struct VertexOut
//{
//	float4 PositionHS : SV_Position;
//	float3 PositionVS : POSITION0;
//	float3 Normal	  : NORMAL0;
//	uint MeshletIndex : COLOR0;
//	float4 Color : COLOR1;
//};

struct VertexOut
{
	float4 PositionHS : SV_Position;
	float4 Color : COLOR1;
};

//ConstantBuffer<Constants> Globals : register(b0);

//static const float4 vertices[3] =
//{
//	float4(    0,  0.25, 0, 1),
//	float4( 0.25, -0.25, 0, 1),
//	float4(-0.25, -0.25, 0, 1)
//};

//[RootSignature(ROOT_SIG)]
[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void main(
	uint gtid : SV_GroupThreadID,
    uint gid : SV_GroupID,
    out vertices VertexOut verts[3],
    out indices uint3 tris[1])
{
	SetMeshOutputCounts(3, 1);
	
	verts[0].PositionHS = float4(0, 0.25, 0, 1);
	verts[0].Color      = float4(1, 0, 0, 1);
	
	verts[1].PositionHS = float4(0.25, -0.25, 0, 1);
	verts[1].Color      = float4(1, 0, 0, 1);
	
	verts[2].PositionHS = float4(-0.25, -0.25, 0, 1);
	verts[2].Color      = float4(1, 0, 0, 1);
	
	tris[0] = uint3(0, 1, 2);
}