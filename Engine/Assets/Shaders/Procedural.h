struct Payload
{
    float padding;
};

struct OutputToPixel
{
    float4 position : SV_Position;
    float4 color : COLOR0;
};
