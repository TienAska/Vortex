#include "Triangle.hlsli"

VertexToPixel main(float4 position : POSITION, float4 color : COLOR)
{
	VertexToPixel output;

	output.position = position;
	output.color = color;

	return output;
}