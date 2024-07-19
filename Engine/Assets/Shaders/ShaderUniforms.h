#ifdef __cplusplus
typedef DirectX::XMMATRIX  float4x4;
#endif

struct GlobalParameters
{
    float4x4 model;
    float4x4 view;
    float4x4 projection;
    float time;
    //float padding[15]; // The constant buffer is 256-byte aligned.
};
