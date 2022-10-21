struct Constants
{
	float4x4 World;
	float4x4 WorldView;
	float4x4 WorldViewProj;
	uint     DrawMeshlets;
};

struct VertexOut
{
	float4 PositionHS : SV_Position;
	float3 PositionVS : POSITION0;
	float3 Normal	  : NORMAL0;
	uint MeshletIndex : COLOR0;
};

ConstantBuffer<Constants> Globals : register(b0);

const float4 vertices[3] =
{
	float4(-1, -1, 0, 1),
	float4( 0,  1, 0, 1),
	float4( 1, -1, 0, 1)
};

[NumThreads(1, 1, 1)]
[OutputTopology("triangle")]
void main(
	uint gtid : SV_GroupThreadID,
    uint gid : SV_GroupID,
    out indices uint3 tris[1],
    out vertices VertexOut verts[3])
{
	tris[0] = (0, 1, 2);
	
	verts[0].PositionHS = vertices[0];
	verts[1].PositionHS = vertices[1];
	verts[2].PositionHS = vertices[2];
}