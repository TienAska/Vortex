struct VertexAttributes
{
    float4 positionHS : SV_Position;
    float3 uvw : TEXCOORD0;
};

struct PrimitiveAttributes
{
    float4 color : COLOR0;
};


void Cube(out float4 vertices[8], out uint3 indices[12])
{
    vertices[0] = float4(-1.0, -1.0, -1.0, 1.0);
    vertices[1] = float4(1.0, -1.0, -1.0, 1.0);
    vertices[2] = float4(1.0, 1.0, -1.0, 1.0);
    vertices[3] = float4(-1.0, 1.0, -1.0, 1.0);
    vertices[4] = float4(-1.0, -1.0, 1.0, 1.0);
    vertices[5] = float4(1.0, -1.0, 1.0, 1.0);
    vertices[6] = float4(1.0, 1.0, 1.0, 1.0);
    vertices[7] = float4(-1.0, 1.0, 1.0, 1.0);
    
    // -z
    indices[0] = uint3(0, 1, 2);
    indices[1] = uint3(0, 2, 3);
    // +z
    indices[2] = uint3(4, 6, 5);
    indices[3] = uint3(4, 7, 6);
    // +x
    indices[4] = uint3(6, 1, 5);
    indices[5] = uint3(6, 2, 1);
    // -x
    indices[6] = uint3(7, 4, 0);
    indices[7] = uint3(7, 0, 3);
    // +y
    indices[8] = uint3(6, 7, 3);
    indices[9] = uint3(6, 3, 2);
    // -y
    indices[10] = uint3(5, 0, 4);
    indices[11] = uint3(5, 1, 0);
}