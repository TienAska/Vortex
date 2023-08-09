struct VertexToPixel
{
	float4 position : SV_Position;
	float4 color : COLOR;
};

VertexToPixel VertexShaderMain(float4 position : POSITION, float4 color : COLOR)
{
	VertexToPixel output;

	output.position = position;
	output.color = color;

	return output;
}

float4 PixelShaderMain(VertexToPixel input) : SV_Target
{
	return input.color;
}
