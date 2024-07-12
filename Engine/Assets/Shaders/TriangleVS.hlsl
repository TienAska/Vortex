#include "Triangle.hlsli"

struct GlobalMatrix
{
	float4x4 model;
};

ConstantBuffer<GlobalMatrix> global : register(b0);

VertexToPixel main(float4 position : POSITION, float4 color : COLOR)
{
	VertexToPixel output;

	output.position = mul(position, global.model);
	output.color = color;

	return output;
}