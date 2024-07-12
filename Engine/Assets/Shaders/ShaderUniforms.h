#ifdef __cplusplus
typedef DirectX::XMMATRIX  float4x4;
#endif

struct GlobalParameters
{
    float time;
    float4x4 model;
    float4x4 view;
    float4x4 projection;
#ifdef __cplusplus
    float padding[15]; // The constant buffer is 256-byte aligned.
#endif
};
