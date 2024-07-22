struct Payload
{
    float padding;
};

struct VertexAttributes
{
    float4 position : SV_Position;
    float2 uv0 : TEXCOORD0;
};

void Quad(out float4 position[4], out float2 uv[4])
{
    position[0] = float4(-1.0, -1.0, 0.0, 1.0);
    position[1] = float4( 1.0, -1.0, 0.0, 1.0);
    position[2] = float4( 1.0,  1.0, 0.0, 1.0);
    position[3] = float4(-1.0,  1.0, 0.0, 1.0);

    uv[0] = float2(0.0, 0.0);
    uv[1] = float2(1.0, 0.0);
    uv[2] = float2(1.0, 1.0);
    uv[3] = float2(0.0, 1.0);
}

void FullscreenTriangle(out float4 position[3], out float2 uv[3])
{
    position[0] = float4(-1.0,  1.0, 0.0, 1.0);
    position[1] = float4( 3.0,  1.0, 0.0, 1.0);
    position[2] = float4(-1.0, -3.0, 0.0, 1.0);

    uv[0] = float2(0.0, 0.0);
    uv[1] = float2(2.0, 0.0);
    uv[2] = float2(0.0, 2.0);
}

float2 truchetPattern(float2 uv, float value)
{
    value = frac(((value - 0.5) * 2.0));
    if (value > 0.75)
    {
        uv = float2(1.0, 1.0) - uv;
    }
    else if (value > 0.5)
    {
        uv = float2(1.0 - uv.x, uv.y);
    }
    else if (value > 0.25)
    {
        uv = 1.0 - float2(1.0 - uv.x, uv.y);
    }
    return uv;
}

float random(float2 uv) {
    return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453123);
}

float valueNoise(uint2 cellCoord, float2 uv)
{
    float topLeft = random(cellCoord.xy);
    float topRight = random(cellCoord.xy + int2(1, 0));
    float bottomLeft = random(cellCoord.xy + int2(0, 1));
    float buttomRight = random(cellCoord.xy + int2(1, 1));

    float2 smoothUV = uv * uv * (3.0 - 2.0 * uv);

    return lerp(lerp(topLeft, topRight, smoothUV.x), lerp(bottomLeft, buttomRight, smoothUV.x), smoothUV.y);
}

float perlinNoise(uint2 cellCoord, float2 uv)
{
    float topLeft = random(cellCoord.xy);
    float topRight = random(cellCoord.xy + int2(1, 0));
    float bottomLeft = random(cellCoord.xy + int2(0, 1));
    float buttomRight = random(cellCoord.xy + int2(1, 1));

    float2 smoothUV = uv * uv * uv * (uv * (uv * 6.0 - 15.0) + 10.0);

    return lerp(lerp(topLeft, topRight, smoothUV.x), lerp(bottomLeft, buttomRight, smoothUV.x), smoothUV.y);
}