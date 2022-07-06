#include "../ShaderStructs.hlsli"

SamplerState SkyBoxSampler : register(s0);


struct CubeVertexToPixel
{
	float4 myPosition	: SV_POSITION;
	float3 myUV			: UV;
};

PixelOutput pixelShader(CubeVertexToPixel input)
{
	PixelOutput returnValue;

	returnValue.myColor = SkyBox.SampleLevel(SkyBoxSampler, input.myUV,0);

	return returnValue;
}