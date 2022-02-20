struct VertexInput
{
    float4 myPosition : POSITION;
    float2 myUV : UV;
};

struct VertexToPixel
{
    float4 myPosition : SV_POSITION;
    float2 myUV : UV;
};

struct PixelOutput
{
    float4 myColor : SV_TARGET;
};

cbuffer FrameBuffer : register(b0)
{
	float4x4 toProjection;
}

cbuffer ObjectBuffer : register(b1)
{
    float4x4 toWorld;
}

texture2D albedoTexture : register(t0);
SamplerState defaultSampler : register(s0);