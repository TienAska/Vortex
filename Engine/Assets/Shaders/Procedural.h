struct Payload
{
    float padding;
};

struct OutputToPixel
{
    float4 position : SV_Position;
    float2 uv0 : TEXCOORD0;
};

void FullscreenTriangle(out float4 position[3], out float2 uv[3])
{
    position[0] = float4(-1.0,  1.0, 0.0, 1.0);
    position[1] = float4( 3.0,  1.0, 0.0, 1.0);
    position[2] = float4(-1.0, -3.0, 0.0, 1.0);

    uv[0] = float2(0.0, 0.0);
    uv[1] = float2(2.0, 0.0);
    uv[2] = float2(0.0, 2.0);
}

float random(float2 uv) {
    return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453123);
}