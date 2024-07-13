struct Payload
{
    float padding;
};

struct OutputToPixel
{
    float4 position : SV_Position;
    float2 uv0 : TEXCOORD0;
};
