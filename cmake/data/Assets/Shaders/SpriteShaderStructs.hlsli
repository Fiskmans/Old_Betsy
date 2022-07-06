struct VertexInput
{
	uint myIndex		: SV_VertexID;
};

struct VertexToPixel
{
    float4 myPosition	: SV_POSITION;
    float2 myUV			: UV;
};

struct PixelOutput
{
    float4 myColor		: SV_TARGET;
};

cbuffer ObjectBuffer : register(b0)
{
	float4x4 PivotToModel;
    float4x4 ModelToWorld;
	float4 color;
	float2 UVMin;
	float2 UVMax;
	float depth;
	float3 trash_0;
}

Texture2D albedoTexture : register(t0);
SamplerState defaultSampler : register(s0);