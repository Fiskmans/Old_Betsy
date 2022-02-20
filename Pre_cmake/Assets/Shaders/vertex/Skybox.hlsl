#include "../ShaderStructs.hlsli"

SamplerState SkyBoxSampler : register(s0);


struct CubeVertexToPixel
{
	float4 myPosition	: SV_POSITION;
	float3 myUV			: UV;
};

CubeVertexToPixel vertexShader(VertexInput input)
{

	CubeVertexToPixel output;

	output.myPosition = mul(float4(input.myPosition.xyz, 0.f), worldToCameraMatrix);
	output.myPosition = mul(output.myPosition, cameraToProjectionMatrix);
	output.myPosition = output.myPosition.xyww;

	output.myUV = input.myPosition.xyz;

	return output;
}