#include "../../DeferredShaderStructs.hlsli"

struct TerrainVertexToPixel
{
	float4 myScreenPos		: SV_POSITION;
	float3 myWorldPos		: POSITION;
	float3 myNormal			: NORMAL;
	uint myPrimitiveIndex	: SV_PrimitiveId;
};

GBufferOutput pixelShader(TerrainVertexToPixel input)
{
	GBufferOutput output;
	output.myWorldPosition = float4(input.myWorldPos,1);
	output.myAlbedo.rgba = lerp(float4(0.7, 0.7, 0.7, 1), float4(0.3, 0.3, 0.3, 1), abs(cos(input.myPrimitiveIndex)));
	//dont think about it
	output.myNormal = float4(input.myNormal, 0);
	output.myVertexNormal = float4(input.myNormal, 0);
	output.myMetalness = 0;
	output.myRoughness = 0;
	output.myAmbientOcclusion = 0;
	output.myEmissive = 0;
	return output;
}