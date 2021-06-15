#include "../../ShaderStructs.hlsli"

struct TerrainVertexInput
{
	float3 myWorldPos	: SV_POSITION;
	float3 myNormal		: NORMAL;
};

struct TerrainVertexToPixel
{
	float4 myScreenPos	: SV_POSITION;
	float3 myWorldPos	: POSITION;
	float3 myNormal		: NORMAL;
};

TerrainVertexToPixel vertexShader(TerrainVertexInput input)
{
	TerrainVertexToPixel output;


	float4x4 packedPosition = {
		input.myWorldPos.x,		input.myWorldPos.y,		input.myWorldPos.z,		1,
		input.myNormal.x,		input.myNormal.y,		input.myNormal.z,		0,
		0,						0,						0,						0,
		0,						0,						0,						0
	};

	float4x4 worldPosition = mul(packedPosition, modelToWorldMatrix);
	float4x4 cameraPosition = mul(worldPosition, worldToCameraMatrix);
	float4x4 screenPosition = mul(cameraPosition, cameraToProjectionMatrix);

	output.myWorldPos = worldPosition[0].xyz;
	output.myScreenPos = screenPosition[0];
	output.myNormal = normalize(worldPosition[1].xyz);

	return output;
}